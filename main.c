//
//  main.c
//  Chip8
//
//  Created by Fabio Codiglioni on 26/07/16.
//  Copyright © 2016 Fabio Codiglioni. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Chip8.h"


int main(int argc, const char *argv[]) {
    
    if (argc != 2) {
        puts("MISSING GAME");
        exit(EXIT_FAILURE);
    }
    char filename[1024] = {0};
    strcpy(filename, argv[1]);
    //strcpy(filename, "/Users/Fabio/Library/Mobile Documents/M6HJR9W95L~com~textasticapp~textastic/Documents/myChip8-bin-src/tetris.c8");
    
    Chip8 chip8;
    
    init(&chip8);
    loadGame(&chip8, filename);
    
    while (1) {
        emulateCycle(&chip8);
        if (chip8.drawFlag)
            drawGraphics(&chip8);
        setKeys(&chip8);
    }
    
    return EXIT_SUCCESS;
}
