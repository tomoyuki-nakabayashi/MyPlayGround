#define COL8_000000   0
#define COL8_FF0000   1
#define COL8_00FF00   2
#define COL8_FFFF00   3
#define COL8_0000FF   4
#define COL8_FF00FF   5
#define COL8_00FFFF   6
#define COL8_FFFFFF   7
#define COL8_C6C6C6   8
#define COL8_840000   9
#define COL8_008400   10
#define COL8_848400   11
#define COL8_000084   12
#define COL8_840084   13
#define COL8_008484   14
#define COL8_848484   15

extern void io_hlt(void);
extern void io_cli(void);
extern void io_out8(int port, int data);
extern int io_load_eflags(void);
extern void io_store_eflags(int eflags);

struct BOOTINFO {
  char cyls, leds, vmode, reserve;
  short scrnx, scrny;
  char *vram;
};

static char font_A[16] = {
  0x00, 0x18, 0x18, 0x18, 0x18, 0x24, 0x24, 0x24,
  0x24, 0x7e, 0x42, 0x42, 0x42, 0xe7, 0x00, 0x00
};

void putfont8(char *vram, int xsize, int x, int y, char c, char *font) {
  for (int i = 0; i < 16; i++) {
    char *p = vram + (y + i) * xsize + x;
    char d = font[i];
    if ((d & 0x80) != 0) { p[0] = c; }
    if ((d & 0x40) != 0) { p[1] = c; }
    if ((d & 0x20) != 0) { p[2] = c; }
    if ((d & 0x10) != 0) { p[3] = c; }
    if ((d & 0x08) != 0) { p[4] = c; }
    if ((d & 0x04) != 0) { p[5] = c; }
    if ((d & 0x02) != 0) { p[6] = c; }
    if ((d & 0x01) != 0) { p[7] = c; }
  }
}

void set_palette(int start, int end, unsigned char *rgb) {
  int i, eflags;
  eflags = io_load_eflags();  // save interrupt enable flag
  io_cli();  // disable interrupt
  io_out8(0x03c8, start);
  for (i = start; i <= end; i++) {
    io_out8(0x03c9, rgb[0] / 4);
    io_out8(0x03c9, rgb[1] / 4);
    io_out8(0x03c9, rgb[2] / 4);
    rgb += 3;
  }
  io_store_eflags(eflags);
  return;
}

void init_palette(void) {
  static unsigned char table_rgb[16*3] = {
    0x00, 0x00, 0x00,  // black
    0xff, 0x00, 0x00,  // light red
    0x00, 0xff, 0x00,  // light green
    0xff, 0xff, 0x00,  // light yellow
    0x00, 0x00, 0xff,  // light blue
    0xff, 0x00, 0xff,  // light purple
    0x00, 0xff, 0xff,  // turquoise
    0xff, 0xff, 0xff,  // white
    0xc6, 0xc6, 0xc6,  // light grey
    0x84, 0x00, 0x00,  // dark red
    0x00, 0x84, 0x00,  // dark green
    0x84, 0x84, 0x00,  // dark yellow
    0x00, 0x00, 0x84,  // dark blue
    0x84, 0x00, 0x84,  // dark purple
    0x00, 0x84, 0x84,  // dark turquoise
    0x84, 0x84, 0x84,  // dark grey
  };
  set_palette(0, 15, table_rgb);
  return;
}

void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1) {
  int x, y;
  for (y = y0; y <= y1; y++) {
    for (x = x0; x <= x1; x++)
      vram[y * xsize + x] = c;
  }
  return;
}

void init_screen(unsigned char *vram, int xsize, int ysize) {
	boxfill8(vram, xsize, COL8_008484,  0,         0,          xsize -  1, ysize - 29);
	boxfill8(vram, xsize, COL8_C6C6C6,  0,         ysize - 28, xsize -  1, ysize - 28);
	boxfill8(vram, xsize, COL8_FFFFFF,  0,         ysize - 27, xsize -  1, ysize - 27);
	boxfill8(vram, xsize, COL8_C6C6C6,  0,         ysize - 26, xsize -  1, ysize -  1);

	boxfill8(vram, xsize, COL8_FFFFFF,  3,         ysize - 24, 59,         ysize - 24);
	boxfill8(vram, xsize, COL8_FFFFFF,  2,         ysize - 24,  2,         ysize -  4);
	boxfill8(vram, xsize, COL8_848484,  3,         ysize -  4, 59,         ysize -  4);
	boxfill8(vram, xsize, COL8_848484, 59,         ysize - 23, 59,         ysize -  5);
	boxfill8(vram, xsize, COL8_000000,  2,         ysize -  3, 59,         ysize -  3);
	boxfill8(vram, xsize, COL8_000000, 60,         ysize - 24, 60,         ysize -  3);

	boxfill8(vram, xsize, COL8_848484, xsize - 47, ysize - 24, xsize -  4, ysize - 24);
	boxfill8(vram, xsize, COL8_848484, xsize - 47, ysize - 23, xsize - 47, ysize -  4);
	boxfill8(vram, xsize, COL8_FFFFFF, xsize - 47, ysize -  3, xsize -  4, ysize -  3);
	boxfill8(vram, xsize, COL8_FFFFFF, xsize -  3, ysize - 24, xsize -  3, ysize -  3);
}

void HariMain(void) {
  init_palette();

  struct BOOTINFO *binfo = (struct BOOTINFO*) 0x0ff0;

  init_screen(binfo->vram, binfo->scrnx, binfo->scrny);
  putfont8(binfo->vram, binfo->scrnx, 20, 20, COL8_FFFFFF, font_A);

  for(;;) {
    io_hlt();
  }
}