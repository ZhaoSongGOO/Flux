nasm -f elf32 boot.s -o boot.o

i686-elf-gcc -c kernel.c -o kernel.o -ffreestanding -O2 -Wall

i686-elf-gcc -T linker.ld -o flux.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc