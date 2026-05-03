#include <stdint.h>
#define VIDEO_MEMORY ((volatile char *)0xB8000)
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

int global_data = 123;
int static_global_data = 0;

int cursor_x = 0;
int cursor_y = 0;

extern char stack_bottom[], stack_top[], multiboot_data[], _start[],
    _kernel_end[], _kernel_start[];

void kmain(void);

uint32_t CR0() {
  uint32_t cr0;
  asm volatile("mov %%cr0, %0" : "=r"(cr0));
  return cr0;
}

void print_char(char c) {
  if (c == '\n') {
    cursor_x = 0;
    cursor_y += 1;
    return;
  }
  if (cursor_x >= SCREEN_WIDTH) {
    cursor_x = 0;
    cursor_y += 1;
  }
  if (cursor_y >= SCREEN_HEIGHT) {
    cursor_y = 0;
    cursor_x = 0;
  }
  int index = (cursor_y * SCREEN_WIDTH + cursor_x) * 2;
  VIDEO_MEMORY[index] = c;
  VIDEO_MEMORY[index + 1] = 0x0A;
  cursor_x += 1;
}

void clean() {
  for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT * 2; i += 2) {
    VIDEO_MEMORY[i] = ' ';
    VIDEO_MEMORY[i + 1] = 0x07;
  }
  cursor_x = 0;
  cursor_y = 0;
}

void print_addr(uint32_t addr) {
  for (int i = 28; i >= 0; i -= 4) {
    int u = (addr >> i) & 0xF;
    if (u < 10) {
      print_char(u + '0');
    } else {
      print_char(u - 10 + 'A');
    }
  }
}

void print_memory_segment_info(const char *title, uint32_t addr) {
  print_char('\n');
  for (int i = 0; title[i] != '\0'; i++) {
    print_char(title[i]);
  }
  print_addr(addr);
}

void print_memory_info() {
  print_memory_segment_info("[*]multiboot      :0x", (uint32_t)multiboot_data);
  print_memory_segment_info("[*]_start         :0x", (uint32_t)_start);
  print_memory_segment_info("[*]kmain          :0x", (uint32_t)kmain);
  print_memory_segment_info("[*]stack_bottom   :0x", (uint32_t)stack_bottom);
  print_memory_segment_info("[*]stack_top      :0x", (uint32_t)stack_top);
}

void print_message(const char *msg) {
  print_char('\n');
  for (int i = 0; msg[i] != '\0'; i++) {
    print_char(msg[i]);
  }
}

void print_cr0() {
  uint16_t cr0 = CR0();
  print_message("[*]CR0: ");
  print_addr(cr0);
  if (cr0 & 0x1) {
    print_message("[*]Protected Mode");
  } else {
    print_message("[*]Real Mode");
  }
}

// GDT: global description table
void print_gdt() {
  struct {
    uint16_t limit;
    uint32_t base;
  } __attribute__((packed)) gdtr;
  asm volatile("sgdt %0" : "=m"(gdtr));
  print_message("[*]GDT.base: ");
  print_addr(gdtr.base);
  print_message("[*]GDT.limit: ");
  print_addr(gdtr.limit);
}

void reset_gdt() {
  uint64_t my_gdt[3];
  my_gdt[0] = 0;                  // Null description
  my_gdt[1] = 0x00CF9A000000FFFF; // Code: Base=0, Limit=4G, Type=Read/Exe
  my_gdt[2] = 0x00CF92000000FFFF; // Data: Base=0, Limit=4G, Type=Read/Write

  struct {
    uint16_t limit;
    uint32_t base;
  } __attribute__((packed)) gdtr = {sizeof(my_gdt) - 1, (uint32_t)my_gdt};

  asm volatile("lgdt %0" : : "m"(gdtr));
}

void print_kernel_info() {
  print_memory_segment_info("[*]Kernel start addr   :0x",
                            (uint32_t)_kernel_start);
  print_memory_segment_info("[*]Kernel end addr     :0x",
                            (uint32_t)_kernel_end);
  print_memory_segment_info("[*]Kernel size(Byte)   :0x",
                            (uint32_t)(_kernel_end - _kernel_start));
}

void kmain(void) {
  clean();
  const char *message = "[*]Hello, Flux!";
  for (int i = 0; message[i] != '\0'; i++) {
    print_char(message[i]);
  }
  print_kernel_info();
  print_cr0();
  print_gdt();
  reset_gdt();
  print_gdt();
  print_memory_info();
}