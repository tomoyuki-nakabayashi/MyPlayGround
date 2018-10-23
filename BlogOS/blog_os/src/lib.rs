#![no_std]
#![feature(abi_x86_interrupt)]

extern crate spin;
extern crate volatile;
#[macro_use]
extern crate lazy_static;
extern crate uart_16550;
extern crate x86_64;
extern crate pic8259_simple;

#[cfg(test)]
extern crate array_init;
#[cfg(test)]
extern crate std;

// NEW: We need to add `pub` here to make them accessible from the outside
#[macro_use]
pub mod vga_buffer;  // import this before other modules so its macros may be used
pub mod serial;
pub mod gdt;
pub mod interrupts;

pub unsafe fn exit_qemu() {
    use x86_64::instructions::port::Port;

    let mut port = Port::<u32>::new(0xf4);
    port.write(0);
}

pub fn hlt_loop() -> ! {
    loop {
        x86_64::instructions::hlt();
    }
}