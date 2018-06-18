#include "bootpack.h"

void HariMain(void) {
  init_gdtidt();
  init_palette();

  struct BOOTINFO *binfo = (struct BOOTINFO*) ADR_BOOTINFO;

  init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
  putfonts8_ascii(binfo->vram, binfo->scrnx, 8, 8, COL8_FFFFFF, "ABC 123");
  putfonts8_ascii(binfo->vram, binfo->scrnx, 31, 31, COL8_000000, "Haribote OS.");
  putfonts8_ascii(binfo->vram, binfo->scrnx, 30, 30, COL8_FFFFFF, "Haribote OS.");

  char mcursor[256];
  int mx = 160, my = 100;
  init_mouse_cursor8(mcursor, COL8_008484);
  putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);

  unsigned char s[16];
  sprintf(s, "scrnx = %d", binfo->scrnx);
  putfonts8_ascii(binfo->vram, binfo->scrnx, 16, 64, COL8_FFFFFF, s);

  for(;;) {
    io_hlt();
  }
}