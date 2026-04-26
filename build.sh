# 1. 编译汇编 (输出为 ELF 32位格式)
nasm -f elf32 boot.s -o boot.o

# 2. 编译 C (使用交叉编译器)
i686-elf-gcc -c kernel.c -o kernel.o -ffreestanding -O2 -Wall

# 3. 链接
i686-elf-gcc -T linker.ld -o flux.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc