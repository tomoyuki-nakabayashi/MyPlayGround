#include "bootpack.h"

struct FIFO8 mousefifo;

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

// IRQ handler from PS/2 Mouse
void irq_handler2c(int *esp)
{
  io_out8(PIC1_OCW2, 0x64);  // Let PIC know recieve IRQ-12. 0x60 + IRQ number
  io_out8(PIC0_OCW2, 0x62);  // Let PIC know recieve IRQ-02. 0x60 + IRQ number
  unsigned char data = io_in8(PORT_KEYDAT); 
  fifo8_put(&mousefifo, data);
}
