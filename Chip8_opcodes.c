//
//  Chip8_opcodes.c
//  Chip8
//
//  Created by Fabio Codiglioni on 26/07/16.
//  Copyright © 2016 Fabio Codiglioni. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "Chip8.h"


void decodeOpcode(Chip8 *chip8) {
    
    switch (chip8->opcode & 0xF000) {
            
        case 0x0000:
            switch (chip8->opcode & 0x000F) {
                case 0x0000:    // 00E0: Clears the screen
                    for (unsigned short i = 0; i < GFX_PIXELS; i++)
                        chip8->gfx[i] = 0;
                    chip8->drawFlag = 1;
                    chip8->pc += 2;
                    break;
                case 0x000E:    // 00EE: Returns from subroutine
                    chip8->sp--;
                    chip8->pc = chip8->stack[chip8->sp];
                    chip8->pc += 2;
                    break;
                default:
                    printf("\nUNKNOWN OPCODE [0x0000]: 0x%X\nPC: 0x%X\n", chip8->opcode, chip8->pc);
                    exit(EXIT_FAILURE);
            }
            break;
        
        case 0x1000:    // 1NNN: Jumps to address NNN
            chip8->pc = chip8->opcode & 0x0FFF;
            break;
            
        case 0x2000:    // 2NNN: Calls subroutine at NNN
            chip8->stack[chip8->sp] = chip8->pc;
            chip8->sp++;
            chip8->pc = chip8->opcode & 0x0FFF;
            break;
            
        case 0x3000:    // 3XNN: Skips the next instruction if VX equals NN
            if (chip8->V[(chip8->opcode & 0x0F00) >> 8] == (chip8->opcode & 0x00FF))
                chip8->pc += 4;
            else
                chip8->pc += 2;
            break;
            
        case 0x4000:    // 4XNN: Skips the next instruction if VX doesn't equal NN
            if (chip8->V[(chip8->opcode & 0x0F00) >> 8] != (chip8->opcode & 0x00FF))
                chip8->pc += 4;
            else
                chip8->pc += 2;
            break;
            
        case 0x5000:    // 5XY0: Skips the next instruction if VX equals VY
            if (chip8->V[(chip8->opcode & 0x0F00) >> 8] == chip8->V[(chip8->opcode & 0x00F0) >> 4])
                chip8->pc += 4;
            else
                chip8->pc += 2;
            break;
            
        case 0x6000:    // 6XNN: Set VX to NN
            chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->opcode & 0x00FF;
            chip8->pc += 2;
            break;
            
        case 0x7000:    // 7XNN: Adds NN to VX
            chip8->V[(chip8->opcode & 0x0F00) >> 8] += chip8->opcode & 0x00FF;
            chip8->pc += 2;
            break;
            
        case 0x8000:
            switch (chip8->opcode & 0x000F) {
                case 0x0000:    // 8XY0: Sets VX to the value of VY
                    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x00F0) >> 4];
                    chip8->pc += 2;
                    break;
                case 0x0001:    // 8XY1: Sets VX to "VX OR VY"
                    chip8->V[(chip8->opcode & 0x0F00) >> 8] |= chip8->V[(chip8->opcode & 0x00F0) >> 4];
                    chip8->pc += 2;
                    break;
                case 0x0002:    // 8XY2: Sets VX to "VX AND VY"
                    chip8->V[(chip8->opcode & 0x0F00) >> 8] &= chip8->V[(chip8->opcode & 0x00F0) >> 4];
                    chip8->pc += 2;
                    break;
                case 0x0003:    // 8XY3: Sets VX to "VX XOR VY"
                    chip8->V[(chip8->opcode & 0x0F00) >> 8] ^= chip8->V[(chip8->opcode & 0x00F0) >> 4];
                    chip8->pc += 2;
                    break;
                case 0x0004:    // 8XY4: Add VY to VX. VF is set to 1 when there's carry, and to 0 when there's not.
                    if (chip8->V[(chip8->opcode & 0x00F0) >> 4] > (0xFF - chip8->V[(chip8->opcode & 0x0F00) >> 8]))
                        chip8->V[0xF] = 1;
                    else
                        chip8->V[0xF] = 0;
                    chip8->V[(chip8->opcode & 0x0F00) >> 8] += chip8->V[(chip8->opcode & 0x00F0) >> 4];
                    chip8->pc += 2;
                    break;
                case 0x0005:    // 8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there's not
                    if (chip8->V[(chip8->opcode & 0x00F0) >> 4] > chip8->V[(chip8->opcode & 0x0F00) >> 8])
                        chip8->V[0xF] = 0;
                    else
                        chip8->V[0xF] = 1;
                    chip8->V[(chip8->opcode & 0x0F00) >> 8] -= chip8->V[(chip8->opcode & 0x00F0) >> 4];
                    chip8->pc += 2;
                    break;
                case 0x0006:    // 8XY6: Shift VX right by one. VF is set to the value of the least significant bit of VX before the shift
                    chip8->V[0xF] = chip8->V[(chip8->opcode & 0x0F00) >> 8] & 0x1;
                    chip8->V[(chip8->opcode & 0x0F00) >> 8] >>= 1;
                    chip8->pc += 2;
                    break;
                case 0x0007:    // 8XY7: Setx VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there's not
                    if (chip8->V[(chip8->opcode & 0x0F00) >> 8] > chip8->V[(chip8->opcode & 0x00F0) >> 4])
                        chip8->V[0xF] = 0;
                    else
                        chip8->V[0xF] = 1;
                    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x00F0) >> 4] - chip8->V[(chip8->opcode & 0x0F00) >> 8];
                    chip8->pc += 2;
                    break;
                case 0x000E:    // 8XYE: Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift
                    chip8->V[0xF] = chip8->V[(chip8->opcode & 0x0F00) >> 8] >> 7;
                    chip8->V[(chip8->opcode & 0x0F00) >> 8] <<= 1;
                    chip8->pc += 2;
                    break;
                default:
                    printf("\nUNKNOWN OPCODE [0x8000]: 0x%X\nPC: 0x%X\n", chip8->opcode, chip8->pc);
                    exit(EXIT_FAILURE);
            }
            break;
        
        case 0x9000:    // 9XY0: Skips the next instruction if VX doesn't equal VY
            if (chip8->V[(chip8->opcode & 0x0F00) >> 8] != chip8->V[(chip8->opcode & 0x00F0) >> 4])
                chip8->pc += 4;
            else
                chip8->pc += 2;
            break;
            
        case 0xA000:    // ANNN: Sets I to the address NNN
            chip8->I = chip8->opcode & 0x0FFF;
            chip8->pc += 2;
            break;
            
        case 0xB000:    // BNNN: Jumps to the address NNN plus V0
            chip8->pc = (chip8->opcode & 0x0FFF) + chip8->V[0];
            break;
        
        case 0xC000:    // CXNN: Sets VX to a random number and NN
            chip8->V[(chip8->opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (chip8->opcode & 0x00FF);
            chip8->pc += 2;
            break;
            
        case 0xD000: {  /* DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
                         * Each row of 8 pixels is read as bit-coded starting from memory location I;
                         * I value doesn't change after the execution of this instruction.
                         * VF is set to 1 if any screen pixel are flipped from set to unset when the sprite is drawn,
                         * and to 0 if that doesn't happen.
                         */
                unsigned short x = chip8->V[(chip8->opcode & 0x0F00) >> 8];
                unsigned short y = chip8->V[(chip8->opcode & 0x00F0) >> 4];
                unsigned short height = chip8->opcode & 0x000F;
                unsigned short pixel;
                chip8->V[0xF] = 0;
                for (unsigned int yline = 0; yline < height; yline++) {
                    pixel = chip8->memory[chip8->I + yline];
                    for (unsigned int xline = 0; xline < 8; xline++) {
                        if ((pixel & (0x80 >> xline)) != 0) {
                            if (chip8->gfx[(x + xline + ((y + yline) * 64))] == 1)
                                chip8->V[0xF] = 1;
                            chip8->gfx[x + xline + ((y + yline) * 64)] ^= 1;
                        }
                    }
                }
                chip8->drawFlag = 1;
                chip8->pc += 2;
            }
            break;
        
        case 0xE000:
            switch (chip8->opcode & 0x00FF) {
                case 0x009E:    // EX9E: Skips the next instruction fi the key stored in VX is pressed
                    if (chip8->key[chip8->V[(chip8->opcode & 0x0F00) >> 8]] != 0)
                        chip8->pc += 4;
                    else
                        chip8->pc += 2;
                    break;
                case 0x00A1:    // EXA1: Skips the next instruction if the key stored in VX isn't pressed
                    if (chip8->key[chip8->V[(chip8->opcode & 0x0F00) >> 8]] == 0)
                        chip8->pc += 4;
                    else
                        chip8->pc += 2;
                    break;
                    
                default:
                    printf("\nUNKNOWN OPCODE [0xE000]: 0x%X\nPC: 0x%X\n", chip8->opcode, chip8->pc);
                    exit(EXIT_FAILURE);
            }
            break;
            
        case 0xF000:
            switch (chip8->opcode & 0x00FF) {
                case 0x0007:    // FX07: Sets VX to the value of the delay timer
                    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->delay_timer;
                    chip8->pc += 2;
                    break;
                case 0x000A: {  // FX0A: A key press is awaited, and the stored in VX
                        unsigned char keyPress = 0;
                        for (unsigned short i = 0; i < KEYS_NUMBER; i++) {
                            if (chip8->key[i] != 0) {
                                chip8->V[(chip8->opcode & 0x0F00) >> 8] = i;
                                keyPress = 1;
                            }
                        }
                        if (!keyPress)
                            return;
                        chip8->pc += 2;
                    }
                    break;
                case 0x0015:    // FX15: Sets the delay timer to VX
                    chip8->delay_timer = chip8->V[(chip8->opcode & 0x0F00) >> 8];
                    chip8->pc += 2;
                    break;
                case 0x0018:    // FX18: Sets the sound timer to VX
                    chip8->sound_timer = chip8->V[(chip8->opcode & 0x0F00) >> 8];
                    chip8->pc += 2;
                    break;
                case 0x001E:    // FX1E: Adds VX to I
                    if (chip8->I + chip8->V[(chip8->opcode & 0x0F00) >> 8] > 0xFFF) // VF is set to 1 when range overflow (I+VX > 0xFFF)
                        chip8->V[0xF] = 1;
                    else
                        chip8->V[0xF] = 0;
                    chip8->I += chip8->V[(chip8->opcode & 0x0F00) >> 8];
                    chip8->pc += 2;
                    break;
                case 0x0029:    // FX29: Sets I to the location of the sprite for the character in VX
                    chip8->I = chip8->V[(chip8->opcode & 0x0F00) >> 8] * 0x5;
                    chip8->pc += 2;
                    break;
                case 0x0033:
                    chip8->memory[chip8->I] = chip8->V[(chip8->opcode & 0x0F00) >> 8] / 100;
                    chip8->memory[chip8->I + 1] = (chip8->V[(chip8->opcode & 0x0F00) >> 8] / 10) % 10;
                    chip8->memory[chip8->I + 2] = (chip8->V[(chip8->opcode & 0x0F00) >> 8] % 100) % 10;
                    chip8->pc += 2;
                    break;
                case 0x0055:    // FX55: Stores V0 to VX in memory starting at address I
                    for (unsigned short i = 0; i <= ((chip8->opcode & 0x0F00) >> 8); i++)
                        chip8->memory[chip8->I + i] = chip8->V[i];
                    chip8->I += ((chip8->opcode & 0x0F00) >> 8) + 1;
                    chip8->pc += 2;
                    break;
                case 0x0065:    // FX65: Fills V0 to VX with values from memory starting at address I
                    for (unsigned short i = 0; i <= ((chip8->opcode & 0x0F00) >> 8); i++)
                        chip8->V[i] = chip8->memory[chip8->I + i];
                    chip8->I += ((chip8->opcode & 0x0F00) >> 8) + 1;
                    chip8->pc += 2;
                    break;
                default:
                    printf("\nUNKNOWN OPCODE [0xF000]: 0x%X\nPC: 0x%X\n", chip8->opcode, chip8->pc);
                    exit(EXIT_FAILURE);
            }
            break;
            
        default:
            printf("\nUNKNOWN OPCODE: 0x%X\nPC: 0x%X\n", chip8->opcode, chip8->pc);
            exit(EXIT_FAILURE);
    }
}