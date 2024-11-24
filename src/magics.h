#ifndef MAGICS_H
#define MAGICS_H
#include "types.h"

extern int rook_bits[64];
extern int bishop_bits[64];
extern U64 RMagic[64];
extern U64 BMagic[64];
extern U64 bishop_magic_numbers[64];
extern U64 rook_magic_numbers[64];

int multiply_magic(U64 occupancy, U64 magic, int bits);
unsigned int get_random_U32_number();

U64 get_random_U64_number();


U64 generate_magic_number();

U64 set_occupancy(int index, U64 attack_mask, int bits_in_mask);

//A function to test and generate working magic numbers
U64 find_magic(int square, int bits_in_mask, int piece);
//finding magics
U64 find_magic(int square, int bits_in_mask, int piece);
//Debugging
void print_attacks(Square sq);


#endif