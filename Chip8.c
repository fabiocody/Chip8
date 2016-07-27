//
//  Chip8.c
//  Chip8
//
//  Created by Fabio Codiglioni on 26/07/16.
//  Copyright © 2016 Fabio Codiglioni. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <poll.h>
#include "Chip8.h"


unsigned char chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


void init(Chip8 *chip8) {
    unsigned short i;
    chip8->pc       = 0x200;
    chip8->opcode   = 0;
    chip8->I        = 0;
    chip8->sp       = 0;
    chip8->drawFlag = 1;
    for (i = 0; i < GFX_PIXELS; i++)
        chip8->gfx[i] = 0;
    for (i = 0; i < STACK_SIZE; i++)
        chip8->stack[i] = 0;
    for (i = 0; i < REGISTERS_NUMBER; i++)
        chip8->V[i] = 0;
    for (i = 0; i < KEYS_NUMBER; i++)
        chip8->key[i] = 0;
    for (i = 0; i < MEMORY_SIZE; i++)
        chip8->memory[i] = 0;
    for (i = 0; i < 0x50; i++)
        chip8->memory[i] = chip8_fontset[i];
    chip8->delay_timer = chip8->sound_timer = 0;
    srand((unsigned int)time(NULL));
}


void loadGame(Chip8 *chip8, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("CHIP8");
        exit(EXIT_FAILURE);
    }
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);
    char *buffer = (char *)malloc(sizeof(char) * fileSize);
    if (buffer == NULL) {
        perror("MEMORY ERROR");
        exit(EXIT_FAILURE);
    }
    size_t result = fread(buffer, 1, fileSize, file);
    if (result != fileSize) {
        perror("READING ERROR");
        exit(EXIT_FAILURE);
    }
    
    if ((MEMORY_SIZE - 0x200) > fileSize)
        for (unsigned short i = 0; i < fileSize; i++)
            chip8->memory[i + 0x200] = buffer[i];
    else {
        perror("ROM TOO BIG FOR MEMORY");
        exit(EXIT_FAILURE);
    }
    fclose(file);
    free(buffer);
}


void emulateCycle(Chip8 *chip8) {
    fetchOpcode(chip8);
    decodeOpcode(chip8);
    updateTimers(chip8);
}


void fetchOpcode(Chip8 *chip8) {
    chip8->opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];
}


void updateTimers(Chip8 *chip8) {
    if (chip8->delay_timer > 0)
        chip8->delay_timer--;
    if (chip8->sound_timer > 0) {
        if (chip8->sound_timer == 1)
            system("tput bel");
        chip8->sound_timer--;
    }
}


void drawGraphics(Chip8 *chip8) {
    system("clear");
    //printf("\033[H");
    for (unsigned short i = 0; i < GFX_PIXELS; i++) {
        if (i % GFX_WIDTH == 0)
            putchar('\n');
        if (chip8->gfx[i])
            putchar('*');
            //printf("%s", "\033[07m  \033[m");
        else
            putchar(' ');
        //printf("%s", "\033[E");
    }
    chip8->drawFlag = 0;
    putchar('\n');
    //keyDump(chip8);
    fflush(stdout);
    usleep(2000);
}


char getKey() {
    static struct termios oldt, newt;       // Structs to get all the keystrokes directly to stdin
    struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI };        // Struct to timeout getchar
    // Instructions to get all the keystrokes directly to stdin
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
    char c = 0;
    if (poll(&mypoll, 1, 2))
        c = getchar();
    else
        c = 0;
    fflush(stdout);
    return c;
}


void setKeys(Chip8 *chip8) {
    char c = getKey();
    if (c == '1')  chip8->key[0x1] = KEY_DELAY;     else if (chip8->key[0x1] > 0) chip8->key[0x1]--;
    if (c == '2')  chip8->key[0x2] = KEY_DELAY;     else if (chip8->key[0x2] > 0) chip8->key[0x2]--;
    if (c == '3')  chip8->key[0x3] = KEY_DELAY;     else if (chip8->key[0x3] > 0) chip8->key[0x3]--;
    if (c == '4')  chip8->key[0xC] = KEY_DELAY;     else if (chip8->key[0xC] > 0) chip8->key[0xC]--;
    if (c == 'q')  chip8->key[0x4] = KEY_DELAY;     else if (chip8->key[0x4] > 0) chip8->key[0x4]--;
    if (c == 'w')  chip8->key[0x5] = KEY_DELAY;     else if (chip8->key[0x5] > 0) chip8->key[0x5]--;
    if (c == 'e')  chip8->key[0x6] = KEY_DELAY;     else if (chip8->key[0x6] > 0) chip8->key[0x6]--;
    if (c == 'r')  chip8->key[0xD] = KEY_DELAY;     else if (chip8->key[0xD] > 0) chip8->key[0xD]--;
    if (c == 'a')  chip8->key[0x7] = KEY_DELAY;     else if (chip8->key[0x7] > 0) chip8->key[0x7]--;
    if (c == 's')  chip8->key[0x8] = KEY_DELAY;     else if (chip8->key[0x8] > 0) chip8->key[0x8]--;
    if (c == 'd')  chip8->key[0x9] = KEY_DELAY;     else if (chip8->key[0x9] > 0) chip8->key[0x9]--;
    if (c == 'f')  chip8->key[0xE] = KEY_DELAY;     else if (chip8->key[0xE] > 0) chip8->key[0xE]--;
    if (c == 'z')  chip8->key[0xA] = KEY_DELAY;     else if (chip8->key[0xA] > 0) chip8->key[0xA]--;
    if (c == 'x')  chip8->key[0x0] = KEY_DELAY;     else if (chip8->key[0x0] > 0) chip8->key[0x0]--;
    if (c == 'c')  chip8->key[0xB] = KEY_DELAY;     else if (chip8->key[0xB] > 0) chip8->key[0xB]--;
    if (c == 'v')  chip8->key[0xF] = KEY_DELAY;     else if (chip8->key[0xF] > 0) chip8->key[0xF]--;
    if (c == 0x1B || c == 0x04) {
        putchar('\n');
        exit(EXIT_SUCCESS);
    }
}


void keyDump(Chip8 *chip8) {
    puts("\n");
    for (unsigned short i = 0; i < KEYS_NUMBER; i++)
        printf("%x - %u\n", i, chip8->key[i]);
}
