MB_ALIGN     equ  1 << 0
MB_MEMINFO   equ  1 << 1
MB_FLAGS     equ  MB_ALIGN | MB_MEMINFO
MB_MAGIC     equ  0x1BADB002 
MB_CHECKSUM  equ -(MB_MAGIC + MB_FLAGS)

section .multiboot
global multiboot_data
multiboot_data:
align 4
    dd MB_MAGIC
    dd MB_FLAGS
    dd MB_CHECKSUM

section .data
align 4096
boot_page_directory:
    dd (boot_page_table1 - 0xC0000000) + 0x003
    times 767 dd 0
    dd (boot_page_table1 - 0xC0000000) + 0x003
    times 255 dd 0

boot_page_table1:
    %assign i 0
    %rep 1024
        dd (i << 12) | 0x003
        %assign i i+1
    %endrep

section .text
[bits 32]               
global _start
extern kmain           

_start:
    %define PHY_ADDR(v) ((v) - 0xC0000000)
    mov eax, PHY_ADDR(boot_page_directory)
    mov cr3, eax

    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    lea eax, [.higher_half]
    jmp eax

.higher_half:
    mov esp, stack_top
    call kmain
.halt:
    hlt
    jmp .halt

section .bss
global stack_bottom
global stack_top
align 16
stack_bottom:
    resb 16384    ; 1024 * 16 Byte
stack_top: