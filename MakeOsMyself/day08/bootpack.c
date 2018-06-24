#include "bootpack.h"

#define PORT_KEYDAT   0x0060
#define PORT_KEYSTA   0x0064
#define PORT_KEYCMD   0x0064
#define KEYSTA_SEND_NOTREADY  0x02
#define KEYCMD_WRITE_MODE 0x60
#define KBC_MODE      0x47
#define KEYCMD_SENDTO_MOUSE   0xd4
#define MOUSECMD_ENABLE       0xf4

extern struct FIFO8 keyfifo;
extern struct FIFO8 mousefifo;

struct MOUSE_DEC {
  unsigned char buf[3], phase;
  int x, y, btn;
};

void wait_KBC_sendready(void) {
  // Wait for keyboard controller is ready to send data.
  for (;;) {
    if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
      break;
    }
  }
}

void init_keyboard(void) {
  // Initialize keyboard controller
  wait_KBC_sendready();
  io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
  wait_KBC_sendready();
  io_out8(PORT_KEYDAT, KBC_MODE);
}

void enable_mouse(struct MOUSE_DEC *mdec) {
  wait_KBC_sendready();
  io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
  wait_KBC_sendready();
  io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
  // Will recieve ACK (0xfa)
  mdec->phase = 0;
}

int mouse_decode(struct MOUSE_DEC *mdec, unsigned char data) {
  if (mdec->phase == 0) {
    if (data == 0xfa) {
      mdec->phase = 1;
    }
    return 0;
  } 
  if (mdec->phase == 1) {
    if ((data & 0xc8) == 0x08) {
      mdec->buf[0] = data;
      mdec->phase = 2;
    }
    return 0;
  } 
  if (mdec->phase == 2) {
    mdec->buf[1] = data;
    mdec->phase = 3;
    return 0;
  } 
  if (mdec->phase == 3) {
    mdec->buf[2] = data;
    mdec->phase = 1;
    mdec->btn = mdec->buf[0] & 0x07;
    mdec->x = mdec->buf[1];
    mdec->y = mdec->buf[2];
    if ((mdec->buf[0] & 0x10) != 0) {
      mdec->x |= 0xffffff00;
    }
    if ((mdec->buf[0] & 0x20) != 0) {
      mdec->y |= 0xffffff00;
    }
    mdec->y = -mdec->y;
    return 1;
  }
  return -1;
}

void HariMain(void) {
  init_gdtidt();
  init_pic();
  io_sti();

  char keybuf[32], mousebuf[128];
  fifo8_init(&keyfifo, 32, keybuf);
  fifo8_init(&mousefifo, 128, mousebuf);
	io_out8(PIC0_IMR, 0xf9);  // keyboard: IRQ1 (11111001)
	io_out8(PIC1_IMR, 0xef);  // mouse: IRQ12 (11101111)
  init_keyboard();

  struct BOOTINFO *binfo = (struct BOOTINFO*) ADR_BOOTINFO;
  init_palette();
  init_screen(binfo->vram, binfo->scrnx, binfo->scrny);

  char mcursor[256];
	int mx = (binfo->scrnx - 16) / 2;
	int my = (binfo->scrny - 28 - 16) / 2;
	init_mouse_cursor8(mcursor, COL8_008484);
	putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
  char s[40];
	sprintf(s, "(%d, %d)", mx, my);
	putfonts8_ascii(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);

  struct MOUSE_DEC mdec;
  enable_mouse(&mdec);

  for(;;) {
    io_cli();
    if (fifo8_status(&keyfifo) + fifo8_status(&mousefifo) == 0) {
      io_stihlt();
    } else {
      if (fifo8_status(&keyfifo) != 0) {
        int data = fifo8_get(&keyfifo);
        io_sti();
        unsigned char s[4];
        sprintf(s, "%x", data);
        boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
        putfonts8_ascii(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
      } else if (fifo8_status(&mousefifo) != 0) {
        int data = fifo8_get(&mousefifo);
        io_sti();
        if (mouse_decode(&mdec, data) != 0) {
          unsigned char s[32];
          sprintf(s, "[lcr %d %d]", mdec.x, mdec.y);
          if ((mdec.btn & 0x01) != 0) {
            s[1] = 'L';
          }
          if ((mdec.btn & 0x02) != 0) {
            s[3] = 'R';
          }
          if ((mdec.btn & 0x04) != 0) {
            s[2] = 'C';
          }
          boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 32, 16, 32 + 16 * 8 - 1, 31);
          putfonts8_ascii(binfo->vram, binfo->scrnx, 32, 16, COL8_FFFFFF, s);

          // Move mouse cursor
          // Erase mouse cursor
          boxfill8(binfo->vram, binfo->scrnx, COL8_008484, mx, my, mx + 15, my + 15);
          mx += mdec.x / 64;
          my += mdec.y / 64;
          if (mx < 0) mx = 0;
          if (my < 0) my = 0;
          if (mx > binfo->scrnx - 16) mx = binfo->scrnx - 16;
          if (my > binfo->scrny - 16) my = binfo->scrny - 16;
          sprintf(s, "(%d, %d)", mx, my);
          boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 0, 79, 15);  // Erase axis
          putfonts8_ascii(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, s);  // Draw axis
          putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
        }
      }
    }
  }
}