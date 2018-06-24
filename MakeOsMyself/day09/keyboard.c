#include "bootpack.h"

struct FIFO8 keyfifo;

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

// IRQ handler from PS/2 keyboard
void irq_handler21(int *esp) {
  io_out8(PIC0_OCW2, 0x61);  // Let PIC know recieve IRQ-01. 0x60 + IRQ number
  unsigned char data = io_in8(PORT_KEYDAT); 
  fifo8_put(&keyfifo, data);
}
