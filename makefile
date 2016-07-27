CC = gcc
CFLAGS = -Wall
SOURCE = Chip8.c Chip8_opcodes.c main.c
HEADERS = Chip8.h

~/chip8 : $(SOURCE) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCE) -o ~/chip8
