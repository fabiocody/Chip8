//
//  Chip8.h
//  Chip8
//
//  Created by Fabio Codiglioni on 26/07/16.
//  Copyright © 2016 Fabio Codiglioni. All rights reserved.
//

#ifndef CHIP8
#define CHIP8 8


// Constants definition
#define MEMORY_SIZE 4096
#define REGISTERS_NUMBER 16
#define GFX_WIDTH 64
#define GFX_HEIGHT 32
#define GFX_PIXELS GFX_WIDTH * GFX_HEIGHT
#define STACK_SIZE 16
#define KEYS_NUMBER 16
#define KEY_DELAY 25


// Chip8 struct definition
typedef struct {
    
    unsigned short opcode;                          // Store current opcode
    unsigned char memory[MEMORY_SIZE];              // 4K memory
    /* MEMORY MAP
     * 0x000 - 0x1ff  ->  Chip-8 interpreter (contains font set in emu)
     * 0x050 - 0x0a0  ->  Used for the built in 4x5 pixel font set (0-F)
     * 0x200 - 0xfff  ->  Program ROM and work RAM
     */
    unsigned char V[REGISTERS_NUMBER];                            // CPU registers   V0, V1, ..., VF
    unsigned short I;                               // Index register
    unsigned short pc;                              // Program counter
    unsigned char gfx[GFX_WIDTH * GFX_HEIGHT];      // Black and white screen, 64x32 pixels (2048 in total). The array holds the pixel state (0 or 1)
    unsigned char delay_timer;                      // * Timer registers that counts at 60 Hz.
    unsigned char sound_timer;                      // * When set above zero they will count down to zero.
    
    unsigned short stack[STACK_SIZE];                       // The stack is used to remeber the current location before a jump is performed
    unsigned short sp;                              // Stack pointer
    
    unsigned char key[KEYS_NUMBER];                          // Array to store the current state of each of the 16 keys of the HEX based keyboard
    
    unsigned char drawFlag;
    
} Chip8;


// Functions declaration
void init(Chip8 *chip8);
void loadGame(Chip8 *chip8, const char *filename);
void emulateCycle(Chip8 *chip8);
void fetchOpcode(Chip8 *chip8);
void decodeOpcode(Chip8 *chip8);
void updateTimers(Chip8 *chip8);
void drawGraphics(Chip8 *chip8);
void setKeys(Chip8 *chip8);
void keyDump(Chip8 *chip8);


#endif /* CHIP8 */
