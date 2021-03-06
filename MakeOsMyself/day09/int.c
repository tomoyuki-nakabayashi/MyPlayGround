#include "bootpack.h"

#define PORT_KEYDAT   0x0060

void init_pic(void) {
  io_out8(PIC0_IMR, 0xff);  // disable interrupt
  io_out8(PIC1_IMR, 0xff);  // disable interrupt

  io_out8(PIC0_ICW1, 0x11);  // edge trigger mode
  io_out8(PIC0_ICW2, 0x20);  // Recieve IRQ0-7 on INT20-27
  io_out8(PIC0_ICW3, 1 << 2);  // PIC1 connects to IRQ2
  io_out8(PIC0_ICW4, 0x01);  // Non-buffer mode

  io_out8(PIC1_ICW1, 0x11);  // edge trigger mode
  io_out8(PIC1_ICW2, 0x28);  // Recieve IRQ8-15 on INT28-2f
  io_out8(PIC1_ICW3, 2);  // PIC1 connects to IRQ2
  io_out8(PIC1_ICW4, 0x01);  // Non-buffer mode

  io_out8(PIC0_IMR, 0xfb);  // 11111011 disable except PIC1
  io_out8(PIC1_IMR, 0xff);  // disalbe interrupt
}
