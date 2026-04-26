ENTRY(_start)
SECTIONS {
    . = 0xC0100000; /* Virtual Memory 3G + 1M  */

    .text ALIGN(4K) : AT(ADDR(.text) - 0xC0000000) {
        *(.multiboot)
        *(.text)
    }
    .data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000) { *(.data) }
    .bss  ALIGN(4K) : AT(ADDR(.bss)  - 0xC0000000) { *(.bss)  }
}