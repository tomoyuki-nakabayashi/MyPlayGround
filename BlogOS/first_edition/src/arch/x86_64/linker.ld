ENTRY(start)

SECTIONS {
  . = 1M;

  .boot :
  {
    /* Ensure that the multiboot header is at the beginning */
    *(.multiboot_header)
  }

  .text :
  {
    *(.text .text.*)
  }

  .rodata : {
    *(.rodata .rodata.*)
  }

  .data.rel.ro : {
    *(.data.rel.ro.local*) *(.data.rel.ro .data.rel.ro.*)
  }
}