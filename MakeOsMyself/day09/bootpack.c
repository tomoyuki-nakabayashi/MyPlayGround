#include "bootpack.h"

#define EFLAGS_AC_BIT   0x00040000
#define CR0_CACHE_DISABLE 0x60000000

extern struct FIFO8 keyfifo;
extern struct FIFO8 mousefifo;

unsigned int memtest_sub(unsigned int start, unsigned int end) {
  unsigned int i, *p, old, pat0 = 0xaa55aa55, pat1 = 0x55aa55aa;
  for (i = start; i <= end; i += 0x1000) {
    p = (unsigned int*)(i + 0xffc);
    old = *p;
    *p = pat0;
    *p ^= 0xffffffff;
    if (*p != pat1) {
not_memory:
      *p = old;
      break;
    }
    *p ^= 0xffffffff;
    if (*p != pat0) {
      goto not_memory;
    }
    *p = old;
  }
  return i;
}

unsigned int memtest(unsigned int start, unsigned int end) {
  char flg486 = 0;
  unsigned int eflg = io_load_eflags();
  eflg |= EFLAGS_AC_BIT;
  io_store_eflags(eflg);
  eflg = io_load_eflags();
  if ((eflg & EFLAGS_AC_BIT) != 0) {
    flg486 = 1;
  }
  eflg &= ~EFLAGS_AC_BIT;
  io_store_eflags(eflg);

  unsigned int cr0 = 0;
  if (flg486 != 0) {
    cr0 = load_cr0();
    cr0 |= CR0_CACHE_DISABLE;
    store_cr0(cr0);
  }

  unsigned int i = memtest_sub(start, end);
  if (flg486 != 0) {
    cr0 = load_cr0();
    cr0 &= ~CR0_CACHE_DISABLE;
    store_cr0(cr0);
  }

  return i;
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

  unsigned int size_mem = memtest(0x00400000, 0xbfffffff) / (1024 * 1024);
  char size_mem_str[40];
  sprintf(size_mem_str, "memory %d MB", size_mem);
  putfonts8_ascii(binfo->vram, binfo->scrnx, 0, 32, COL8_FFFFFF, size_mem_str);

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