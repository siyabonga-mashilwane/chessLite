#ifndef GENERATOR_H
#define GENERATOR_H
#include "types.h"

int castle;
int side = -1;
int enpessant = no_sq;


// move list structure
typedef struct {
    // moves
    int moves[256];
    
    // move count
    int count;
} moves;

#endif