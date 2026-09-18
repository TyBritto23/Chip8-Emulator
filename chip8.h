#ifndef CHIP8_H_
#define CHIP8_H_
#include "stdint.h"
#include "stdio.h"

struct Chip8 {
  // Determines if chip 8 program is running
  uint16_t running;

  // 35 opcodes, all 2 bytes long
  unsigned short opcode;

  // Had direct access to 7 kilobytes of RAM
  // 0x000 - 0x1FF - Chip 8 interpreter (contains font set)
  // 0x050 - 0x0A0 - Built in 4x5 pixel font set (0-F)
  // 0x200 - 0xFFF - Prgram ROM and work RAM
  uint16_t memory[4096];

  // One 16-bit index register called "I" points at locations in memory
  // "PC" program count
  unsigned short I;
  unsigned short pc;

  // unsigned char graphics[64 * 32];
  uint16_t graphics[64 * 32];

  // 16 8-bit general purpose variable regerstes numbered 0-F (V0 - VF)
  // unsigned char vRegs[16];
  uint16_t vRegs[16];

  // Should screen be updated
  uint16_t draw;

  // 2 timer registers that count at 60Hz
  // When set above zero they will count down to zero
  // The systems buzzer sounds whenever the sound timer reaches zero
  uint16_t delayTimer;
  uint16_t soundTimer;

  // 16 bit addresses for the stack to call subroutines / functions
  // To remember which address of the stack is used we use the stack pointer sp
  unsigned short stack[16];
  unsigned short sp;

  // Hex based keypad (0x0 - 0xF), use an array to store the current state of
  // the key
  uint16_t key[16];
};

typedef struct Chip8 Chip8;

void initialize(Chip8 *c8);
void resetMemory(Chip8 *c8);
void loadFontIntoMem(Chip8 *c8);
void loadROM(Chip8 *c8, char *rom);
long checkFileSize(FILE *f);
void executeOpcodes(Chip8 *c8, uint16_t opcode);

#endif
