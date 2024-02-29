all: loader

loader: loader.o
	gcc -m32 -Wall -g -o loader loader.o

loader.o: loader.c
	gcc -m32 -Wall -g -c -o loader.o loader.c

.PHONY: clean

clean:
	rm -f *.o loader