#include "chip8.h"
#include <_stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX_FILE_SIZE 3584

// initialize new instance of a chip8 emulator
void initialize(Chip8 *c8) {
  // Set all bytes in memory, V regs, and display buffer to 0
  // Set I to 0, SP to 0, and clear the stack array
  resetMemory(c8);

  // Load Font set into start of memory
  loadFontIntoMem(c8);

  // Point the PC to 0x200
  // Use as index instead of a direct pointer to memory
  c8->pc = 0x200;
}

// Read binary ROM file and inject it to the Chip8 memory
void loadROM(Chip8 *c8, char *rom) {
  // Open file and read
  FILE *file = fopen(rom, "rb");

  if (file == NULL) {
    printf("Error: Could not open file\n");
    return;
  }

  // Check the file size, make sure it fits in memory 4096 - 512 = 3584
  long size = checkFileSize(file);

  if (size > MAX_FILE_SIZE) {
    fclose(file);
    printf("File size is to big\n");
    return;
  }

  // Read the file bytes directly into memory, statrting at 0x200
  fread(&c8->memory[c8->pc], sizeof(uint8_t), size, file);
  printf("Loaded file into memory\n");

  // Close file
  fclose(file);
}

void resetMemory(Chip8 *c8) {
  // Set memory and stack array to 0
  memset(c8->memory, 0, sizeof(c8->memory));
  memset(c8->stack, 0, sizeof(c8->stack));

  // Point PC to first instruction
  c8->pc = 0x200;
  c8->I = 0;
  c8->sp = 0;
}

long checkFileSize(FILE *f) {
  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  rewind(f);

  return size;
}

void executeOpcodes(Chip8 *c8, uint16_t opcode) {
  // Variables
  uint8_t X, NN, Y;
  uint16_t sum, NNN;
  switch (opcode & 0xF000) {
  case 0x0000:
    switch (opcode) {
      // Clear Screen
    case 0x00E0:
      break;
    // Pops top of the stack pointer SP and puts it in PC
    case 0x00EE:
      c8->pc = c8->stack[c8->sp];
      break;
    default:
      printf("Unknown 0-type opcode: 0x%x\n", opcode);
      break;
    }
  // 1NNN => Jump to address NNN, set pc to NNN
  case 0x1000:
    c8->pc = (opcode & 0x0FFF);
    break;

  // 2NNN => Call NNN, call subroutine at address NNN
  case 0x2000:
    c8->sp++;
    c8->stack[c8->sp] = c8->pc;
    c8->pc = (opcode & 0x0FFF);
    break;

  // 3XNN => skip next instruction if VX == NN, increment PC by 2
  case 0x3000:
    X = (opcode & 0x0F00) >> 8;
    NN = opcode & 0x00FF;
    if (c8->vRegs[X] == NN) {
      c8->pc += 2;
    }
    break;

  // 4XNN => Skip the next instruction if VX != NN
  case 0x4000:
    X = (opcode & 0x0F00) >> 8;
    NN = opcode & 0x00FF;
    if (c8->vRegs[X] != NN) {
      c8->pc += 2;
    }
    break;
  // 5XY0 => Skip the next instruction if VX == VY
  case 0x5000:
    X = (opcode & 0x0F00) >> 8;
    Y = (opcode & 0x00F0) >> 4;
    if (c8->vRegs[X] == c8->vRegs[Y]) {
      c8->pc += 2;
    }
    break;

  // 6XNN => Load value NN into register VX
  case 0x6000:
    X = (opcode & 0x0F00) >> 8;
    NN = opcode & 0x00FF;
    c8->vRegs[X] = (uint8_t)NN;
    break;

  // 7XNN => Add the value NN to the value of register VX and store result in VX
  case 0x7000:
    X = (opcode & 0x0F00) >> 8;
    NN = opcode & 0x00FF;
    c8->vRegs[X] += (uint8_t)NN;
    break;

  case 0x8000:
    switch (opcode & 0x000F) {
    // 8XY0 => Put the value of register VY into VX
    case 0x0:
      X = (opcode & 0x0F00) >> 8;
      Y = (opcode & 0x00F0) >> 4;
      c8->vRegs[X] = (uint8_t)c8->vRegs[Y];
      break;

    // 8XY1 => Performs a bitwise OR between VX and VY and stores the result in
    // register VX
    case 0x1:
      X = (opcode & 0x0F00) >> 8;
      Y = (opcode & 0x00F0) >> 4;
      c8->vRegs[X] = (uint8_t)(c8->vRegs[X] | c8->vRegs[Y]);
      break;

    // 8XY2 => Performs a bitwise AND between VX and VY and stores the result
    // in register VX
    case 0x2:
      X = (opcode & 0x0F00) >> 8;
      Y = (opcode & 0x00F0) >> 4;
      c8->vRegs[X] = (uint8_t)(c8->vRegs[X] & c8->vRegs[Y]);
      break;

      // 8XY3 => Performs a bitwise XOR between VX and VY and stores result in
      // register VX
    case 0x3:
      X = (opcode & 0x0F00) >> 8;
      Y = (opcode & 0x00F0) >> 4;
      c8->vRegs[X] = (uint8_t)(c8->vRegs[X] ^ c8->vRegs[Y]);
      break;

    // 8XY4 => Add the values of VX and VY and store the result in VX. Put the
    // carry bit in VF(if there is an overflow, set VF to 1 and 0 otherwise)
    case 0x4:
      X = (opcode & 0x0F00) >> 8;
      Y = (opcode & 0x00F0) >> 4;
      sum = c8->vRegs[X] + c8->vRegs[Y];
      if (sum > 0xFF)
        c8->vRegs[0xF] = 1;
      else
        c8->vRegs[0xF] = 0;
      c8->vRegs[X] = (uint8_t)(sum & 0xFF);
      break;

    // 8XY5 => Subtract the values of VX and VY and store the result in VX. Put
    // the carry bit in VF(if there is an overflow, set VF to 1 and 0 otherwise)
    case 0x5:
      X = (opcode & 0x0F00) >> 8;
      Y = (opcode & 0x00F0) >> 4;
      if (c8->vRegs[X] > c8->vRegs[Y])
        c8->vRegs[0xF] = 1;
      else
        c8->vRegs[0xF] = 0;
      sum = c8->vRegs[X] - c8->vRegs[Y];
      c8->vRegs[X] = (uint8_t)(sum & 0xFF);
      break;

    // 8XY6 => Shift right, or divide VX by two. Store the least significant
    // bit of VX in VF, and then divide VX and store its value in VX
    case 0x6:
      X = (opcode & 0x0F00) >> 8;
      c8->vRegs[0xF] = c8->vRegs[X] & 0x01;
      c8->vRegs[X] /= 2;
      break;

    // 8XY7 => Subtract the values of VY and VX and store the result in VX.
    // Put the carry bit in VF(if there is an overflow, set VF to 1 and 0
    // otherwise)
    case 0x7:
      X = (opcode & 0x0F00) >> 8;
      Y = (opcode & 0x00F0) >> 4;
      if (c8->vRegs[Y] > c8->vRegs[X])
        c8->vRegs[0xF] = 1;
      else
        c8->vRegs[0xF] = 0;
      sum = c8->vRegs[Y] - c8->vRegs[X];
      c8->vRegs[X] = (uint8_t)(sum & 0xFF);
      break;

    // 8XYE => Shift left, or multiply by 2, stroe the most significant bit of
    // VX in VF, then multiply VX and store its value in VX
    case 0xE:
      X = (opcode & 0x0F00) >> 8;
      c8->vRegs[0xF] = c8->vRegs[X] & 0x80;
      c8->vRegs[X] *= 2;
      break;
    }

  // 9XY0 => Skip the next instruction if values of VX and VY are not equal
  case 0x9000:
    X = (opcode & 0x0F00) >> 8;
    Y = (opcode & 0x00F0) >> 4;
    if (c8->vRegs[X] != c8->vRegs[Y])
      c8->pc += 2;
    break;

  // ANNN => Set the value of I to the address NNN
  case 0xA000:
    NNN = opcode & 0x0FFF;
    c8->I = (uint16_t)NNN;
    break;

  // BNNN => Jump to location NNN + V0
  case 0xB000:
    NNN = opcode & 0x0FFF;
    c8->pc = (uint16_t)NNN + c8->vRegs[0x0];
    break;

  // CXNN => Generate a random byte (0-255), do a bitwise AND with NN and
  // store value in VX
  case 0xC000:
    X = (opcode & 0x0F00) >> 8;
    NN = opcode & 0x00FF;
    c8->vRegs[X] = (uint16_t)(rand() & 0xFF) & NN;
    break;

  // DXYN => The n-byte sprite starting at the address I is drawn to the display
  // at coordinates (VX, VY), then VF is set to 1 if there has been a collision
  // (display bit was changed from 1 to 0)
  case 0xD000:
    break;
  }
}

void loadFontIntoMem(Chip8 *c8) {
  // Load Font set into memory array from
  // 0
  c8->memory[0x000] = 0xF0;
  c8->memory[0x001] = 0x90;
  c8->memory[0x002] = 0x90;
  c8->memory[0x003] = 0x90;
  c8->memory[0x004] = 0xF0;
  // 1
  c8->memory[0x005] = 0x20;
  c8->memory[0x006] = 0x60;
  c8->memory[0x007] = 0x20;
  c8->memory[0x008] = 0x20;
  c8->memory[0x009] = 0x70;
  // 2
  c8->memory[0x00A] = 0xF0;
  c8->memory[0x00B] = 0x10;
  c8->memory[0x00C] = 0xF0;
  c8->memory[0x00D] = 0x80;
  c8->memory[0x00E] = 0xF0;
  // 3
  c8->memory[0x00F] = 0xF0;
  c8->memory[0x010] = 0x10;
  c8->memory[0x011] = 0xF0;
  c8->memory[0x012] = 0x10;
  c8->memory[0x013] = 0xF0;
  // 4
  c8->memory[0x014] = 0x90;
  c8->memory[0x015] = 0x90;
  c8->memory[0x016] = 0xF0;
  c8->memory[0x017] = 0x10;
  c8->memory[0x018] = 0x10;
  // 5
  c8->memory[0x019] = 0xF0;
  c8->memory[0x01A] = 0x80;
  c8->memory[0x01B] = 0xF0;
  c8->memory[0x01C] = 0x10;
  c8->memory[0x01D] = 0xF0;
  // 6
  c8->memory[0x01E] = 0xF0;
  c8->memory[0x01F] = 0x80;
  c8->memory[0x020] = 0xF0;
  c8->memory[0x021] = 0x90;
  c8->memory[0x022] = 0xF0;
  // 7
  c8->memory[0x023] = 0xF0;
  c8->memory[0x024] = 0x10;
  c8->memory[0x025] = 0x20;
  c8->memory[0x026] = 0x40;
  c8->memory[0x028] = 0x40;
  // 8
  c8->memory[0x029] = 0xF0;
  c8->memory[0x02A] = 0x90;
  c8->memory[0x02B] = 0xF0;
  c8->memory[0x02C] = 0x90;
  c8->memory[0x02D] = 0xF0;
  // 9
  c8->memory[0x02E] = 0xF0;
  c8->memory[0x02F] = 0x90;
  c8->memory[0x030] = 0xF0;
  c8->memory[0x031] = 0x10;
  c8->memory[0x032] = 0xF0;
  // A
  c8->memory[0x033] = 0xF0;
  c8->memory[0x034] = 0x90;
  c8->memory[0x035] = 0xF0;
  c8->memory[0x036] = 0x90;
  c8->memory[0x037] = 0x90;
  // B
  c8->memory[0x038] = 0xE0;
  c8->memory[0x039] = 0x90;
  c8->memory[0x03A] = 0xE0;
  c8->memory[0x03B] = 0x90;
  c8->memory[0x03C] = 0xE0;
  // C
  c8->memory[0x03D] = 0xF0;
  c8->memory[0x03E] = 0x80;
  c8->memory[0x03F] = 0x80;
  c8->memory[0x040] = 0x80;
  c8->memory[0x041] = 0xF0;
  // D
  c8->memory[0x042] = 0xE0;
  c8->memory[0x043] = 0x90;
  c8->memory[0x044] = 0x90;
  c8->memory[0x045] = 0x90;
  c8->memory[0x046] = 0xE0;
  // E
  c8->memory[0x047] = 0xF0;
  c8->memory[0x048] = 0x80;
  c8->memory[0x049] = 0xF0;
  c8->memory[0x04A] = 0x80;
  c8->memory[0x04B] = 0xF0;
  // F
  c8->memory[0x04C] = 0xF0;
  c8->memory[0x04D] = 0x80;
  c8->memory[0x04E] = 0xF0;
  c8->memory[0x04F] = 0x80;
  c8->memory[0x050] = 0x80;
}
