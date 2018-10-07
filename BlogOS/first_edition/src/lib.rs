#![feature(panic_implementation)]
#![feature(panic_info_message)]
#![no_std]
#![feature(unique)]
#![feature(const_fn)]
#![feature(const_unique_new)]
#![feature(ptr_internals)]

#[macro_use]
mod vga_buffer;
mod memory;

extern crate volatile;
extern crate spin;
extern crate multiboot2;

use memory::FrameAllocator;

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

    let elf_sections_tag = boot_info.elf_sections_tag()
        .expect("Elf-sections tag required");

    println!("kernel sections:");
    for section in elf_sections_tag.sections() {
        println!("    addr: 0x{:x}, size: 0x{:x}, flags: 0x{:x}",
            section.addr, section.size, section.flags);
    }

    let kernel_start = elf_sections_tag.sections().map(|s| s.addr)
        .min().unwrap();
    let kernel_end = elf_sections_tag.sections().map(|s| s.addr + s.size)
        .max().unwrap();
    let multiboot_start = multiboot_information_address;
    let multiboot_end = multiboot_start + (boot_info.total_size as usize);

    let mut frame_allocator = memory::AreaFrameAllocator::new(
        kernel_start as usize, kernel_end as usize, multiboot_start,
        multiboot_end, memory_map_tag.memory_areas()
    );

    println!("{:?}", frame_allocator.allocate_frame());

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