#![feature(panic_implementation)]
#![feature(panic_info_message)]
#![feature(alloc)]
#![feature(alloc_error_handler)]
#![feature(allocator_api)]
#![no_std]
#![feature(const_fn)]
#![feature(ptr_internals)]

#[macro_use]
mod vga_buffer;
mod memory;

extern crate rlibc;
extern crate volatile;
extern crate spin;
extern crate multiboot2;
#[macro_use]
extern crate bitflags;
extern crate x86_64;
extern crate alloc;
#[macro_use]
extern crate once;

use memory::heap_allocator::BumpAllocator;

pub const HEAP_START: usize = 0o_000_001_000_000_0000;
pub const HEAP_SIZE: usize = 100 * 1024;  // 100KiB

#[global_allocator]
static HEAP_ALLOCATOR: BumpAllocator = BumpAllocator::new(HEAP_START,
    HEAP_START + HEAP_SIZE);

#[no_mangle]
pub extern fn rust_main(multiboot_information_address: usize) {
    vga_buffer::clear_screen();
    println!("Hello World{}", "!");

    let boot_info = unsafe{ multiboot2::load(multiboot_information_address) };
    enable_nxe_bit();
    enable_write_protect_bit();

    // set up guard page and map the heap pages
    memory::init(boot_info);

    use alloc::boxed::Box;
    use alloc::vec;
    let mut heap_test = Box::new(42);
    *heap_test -= 15;
    let heap_test2 = Box::new("hello");
    println!("{:?} {:?}", heap_test, heap_test2);

    let mut vec_test = vec![1,2,3,4,5,6,7];
    vec_test[3] = 42;
    for i in &vec_test {
        print!("{} ", i);
    }

    println!("It did not crash!");

    loop{}
}

fn enable_nxe_bit() {
    use x86_64::registers::msr::{IA32_EFER, rdmsr, wrmsr};

    let nxe_bit = 1 << 11;
    unsafe {
        let efer = rdmsr(IA32_EFER);
        wrmsr(IA32_EFER, efer | nxe_bit);
    }
}

fn enable_write_protect_bit() {
    use x86_64::registers::control_regs::{cr0, cr0_write, Cr0};

    unsafe { cr0_write(cr0() | Cr0::WRITE_PROTECT) };
}

use core::panic::PanicInfo;

#[cfg(not(test))] // only compile when the test flag is not set
#[panic_handler]
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

use core::alloc::Layout;
#[cfg(not(test))]
#[alloc_error_handler]
pub fn alloc_error(_: Layout) -> ! {
    loop {}
}