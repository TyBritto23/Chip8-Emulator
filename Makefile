OBJS = main.o chip8.o
CFLAGS = -Wall -Wextra -std=c23 $(shell pkg-config --cflags sdl2)
LIBS = $(shell pkg-config --libs sdl2)

all : main

# 1. LINKING STEP: This is where -lSDL2 (via LIBS) actually belongs
main : $(OBJS)
	gcc $(OBJS) -o main $(LIBS)

# 2. COMPILATION STEPS: Only use CFLAGS here to locate headers
main.o : main.c chip8.h
	gcc $(CFLAGS) -c main.c -o main.o

chip8.o : chip8.c chip8.h
	gcc $(CFLAGS) -c chip8.c -o chip8.o

test : test.c
	gcc test.c -o test

clean :
	rm -f *.o main test
