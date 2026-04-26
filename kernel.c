#include <stdint.h>
#define VIDEO_MEMORY ((volatile char *)0xB8000)
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

int global_data = 123;
int static_global_data = 0;

int cursor_x = 0;
int cursor_y = 0;

extern char stack_bottom[], stack_top[], multiboot_data[], _start[];

void kmain(void);

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
  print_memory_segment_info("[*]multiboot_data :0x", (uint32_t)multiboot_data);
  print_memory_segment_info("[*]_start_data    :0x", (uint32_t)_start);
  print_memory_segment_info("[*]kmain_data     :0x", (uint32_t)kmain);
  print_memory_segment_info("[*]stack_bottom   :0x", (uint32_t)stack_bottom);
  print_memory_segment_info("[*]stack_top      :0x", (uint32_t)stack_top);
}

void kmain(void) {
  clean();
  const char *message = "[*]Hello, Flux!";
  for (int i = 0; message[i] != '\0'; i++) {
    print_char(message[i]);
  }
  print_memory_info();
}