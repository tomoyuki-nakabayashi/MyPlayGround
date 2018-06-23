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

void enable_mouse(void) {
  wait_KBC_sendready();
  io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
  wait_KBC_sendready();
  io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
}

void HariMain(void) {
  init_gdtidt();
  init_pic();
  io_sti();

  char keybuf[32];
  fifo8_init(&keyfifo, 32, keybuf);
	io_out8(PIC0_IMR, 0xf9);  // keyboard: IRQ1 (11111001)
	io_out8(PIC1_IMR, 0xef);  // mouse: IRQ12 (11101111)
  init_keyboard();

  struct BOOTINFO *binfo = (struct BOOTINFO*) ADR_BOOTINFO;
  init_palette();
  init_screen(binfo->vram, binfo->scrnx, binfo->scrny);

  char mcursor[256];
  int mx = 160, my = 100;
  init_mouse_cursor8(mcursor, COL8_008484);
  putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);

  unsigned char s[16];
  sprintf(s, "scrnx = %d", binfo->scrnx);
  putfonts8_ascii(binfo->vram, binfo->scrnx, 16, 64, COL8_FFFFFF, s);

  enable_mouse();

  for(;;) {
    io_cli();
    if (fifo8_status(&keyfifo) == 0) {
      io_stihlt();
    } else {
      unsigned char data = fifo8_get(&keyfifo);
      io_sti();
      unsigned char s[4];
      sprintf(s, "%x", data);

      boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
      putfonts8_ascii(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
    }
  }
}