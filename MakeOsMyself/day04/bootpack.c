extern void io_hlt(void);
extern void io_cli(void);
extern void io_out8(int port, int data);
extern int io_load_eflags(void);
extern void io_store_eflags(int eflags);

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

void HariMain(void) {
  init_palette();

  int i;
  char *p = (char*)0xa0000;
  for (i = 0; i < 0xffff; i++) {
    p[i] = i & 0x0f;
  }
  for(;;) {
    io_hlt();
  }
}