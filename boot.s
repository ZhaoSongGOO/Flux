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

section .text
[bits 32]               
global _start
extern kmain           

_start:
    mov esp, stack_top  
    call kmain         

    cli               
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