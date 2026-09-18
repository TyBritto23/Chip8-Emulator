#include <_stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv) {
  srand(time(NULL));
  int opcode = 0xC955;
  uint8_t x = (opcode & 0x0F00) >> 8;
  uint8_t NN = opcode & 0x00FF;

  printf("Ranodm Number: %d\n", (rand() & 0xFF) & NN);
}
