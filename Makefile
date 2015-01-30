CC=gcc
all:	calcbase tweakcrc
calcvect.o: calcvect.c
	$(CC) calcvect.c -O2 -c -Wall
crc32.o:  crc32.c
	$(CC) crc32.c -O3 -c -Wall -s
calcbase: calcbase.c calcvect.o crc32.o
	$(CC) calcbase.c calcvect.o crc32.o -Wall -O2 -o calcbase
tweakcrc: tweakcrc.c crc32.o calcvect.o
	$(CC) tweakcrc.c  crc32.o calcvect.o -O2 -Wall -o tweakcrc
clean:
	rm -rf crc32.o calcvect.o calcbase tweakcrc
