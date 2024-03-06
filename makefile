all: loader

loader: loader.o start.o
	ld -o loader loader.o start.o startup.o -L/usr/lib32 -lc -T linking_script -dynamic-linker /lib32/ld-linux.so.2

loader.o: loader.c
	gcc -m32 -Wall -g -c -o loader.o loader.c

start.o: start.s
	nasm -f elf32 start.s -o start.o

.PHONY: clean

clean:
	rm -f *.o loader