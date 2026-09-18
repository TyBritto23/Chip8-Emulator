#include "chip8.h"
#include <_stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: ./main <CHIP-8 rom>\n");
    return 1;
  }
  // Create and initialize Chip8 struct
  Chip8 c8;
  initialize(&c8);
  srand(time(NULL));

  char *rom = argv[1];

  loadROM(&c8, rom);

  printf("ROM Loaded\n");

  while (1) {
    // Step 3: Create the "Fetch" StageIn your main execution loop, you need to
    // pull instructions out of memory. Because CHIP-8 memory is an array of
    // 8-bit bytes, but opcodes are 16-bit values, you must combine two
    // consecutive bytes using bitwise shifts.Inside your main cycle function,
    // implement this exact bitwise logic:c// Fetch the 16-bit opcode
    // (Big-Endian format) uint16_t opcode = (chip8->memory[chip8->PC] << 8) |
    // chip8->memory[chip8->PC
    // + 1];

    // Move PC ahead by 2 bytes so it points to the next instruction
    // chip8->PC += 2;
    uint16_t opcode = (c8.memory[c8.pc] << 8) | (c8.memory[c8.pc + 1]);
    // Temp method to exit loop early
    if (opcode == 0x0000) {
      printf("\nEnd of ROM or empty memory reached\n");
      break;
    }
    c8.pc += 2;
    printf("0x%x", opcode);
  }
}
