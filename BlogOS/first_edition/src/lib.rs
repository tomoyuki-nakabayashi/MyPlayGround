#![feature(panic_implementation)]
#![no_std]

#[no_mangle]
pub extern fn rust_main() {
  // ATTENTION: we have a very small stack and no guard page

  let hello = b"Hello World!";
  let color_byte = 0x1f; // white foreground, blue background

  let mut hello_colored = [color_byte; 24];
  for (i, char_byte) in hello.into_iter().enumerate() {
      hello_colored[i*2] = *char_byte;
  }

  // write `Hello World!` to the center of the VGA text buffer
  let buffer_ptr = (0xb8000 + 1988) as *mut _;
  unsafe { *buffer_ptr = hello_colored };

  loop{}
}

use core::panic::PanicInfo;

#[cfg(not(test))] // only compile when the test flag is not set
#[panic_implementation]
#[no_mangle]
pub fn panic(info: &PanicInfo) -> ! {
    loop {}
}