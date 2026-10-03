#include <_stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv) {
  int hex = 0xBEEA;
  uint16_t opcode = (c8.memory[c8.pc] << 8) | (c8.memory[c8.pc + 1]);
  // Temp method to exit loop early
  printf("0x%x", opcode);
  if (opcode == 0x0000) {
    printf("\nEnd of ROM or empty memory reached\n");
  }
}
