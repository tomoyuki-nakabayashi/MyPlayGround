#include "bootpack.h"

extern struct KEYBUF keybuf;

void HariMain(void) {
  init_gdtidt();
  init_pic();
  io_sti();


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

	io_out8(PIC0_IMR, 0xf9);  // keyboard: IRQ1 (11111001)
	io_out8(PIC1_IMR, 0xef);  // mouse: IRQ12 (11101111)

  for(;;) {
    io_cli();
    if (keybuf.flag == 0) {
      io_stihlt();
    } else {
      unsigned char data = keybuf.data;
      keybuf.flag = 0;
      io_sti();
      unsigned char s[4];
      sprintf(s, "%x", data);

      boxfill8(binfo->vram, binfo->scrnx, COL8_008484, 0, 16, 15, 31);
      putfonts8_ascii(binfo->vram, binfo->scrnx, 0, 16, COL8_FFFFFF, s);
    }
  }
}