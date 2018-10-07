#![feature(panic_implementation)]
#![feature(panic_info_message)]
#![no_std]
#![feature(unique)]
#![feature(const_fn)]
#![feature(const_unique_new)]
#![feature(ptr_internals)]

#[macro_use]
mod vga_buffer;

extern crate volatile;
extern crate spin;
extern crate multiboot2;

#[no_mangle]
pub extern fn rust_main(multiboot_information_address: usize) {
    vga_buffer::clear_screen();
    let boot_info = unsafe{ multiboot2::load(multiboot_information_address) };
    let memory_map_tag = boot_info.memory_map_tag()
        .expect("Memory map tag required");
    
    println!("memory areas:");
    for area in memory_map_tag.memory_areas() {
        println!("    start: 0x{:x}, length: 0x{:x}",
            area.base_addr, area.length);
    }
    
    loop{}
}

use core::panic::PanicInfo;

#[cfg(not(test))] // only compile when the test flag is not set
#[panic_implementation]
#[no_mangle]
pub fn panic(info: &PanicInfo) -> ! {
    if let Some(location) = info.location() {
        println!("panic occured in file '{}' at line {}",
            location.file(), location.line());
    } else {
        println!("panic occurred but can't get location information...");
    }

    if let Some(message) = info.message() {
        println!("    {}", message);
    } else {
        println!("panic occurred but can't get any message...");
    }

    loop {}
}