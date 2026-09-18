
OBJS = main.o chip8.o

all : main

main : $(OBJS)
	gcc  $(OBJS) -o main

main.o : main.c chip8.h
	gcc -c main.c -o main.o

chip8.o : chip8.c chip8.h
	gcc -c chip8.c -o chip8.o

test : test.c
	gcc test.c -o test

clean :
	rm -f *.o main test
