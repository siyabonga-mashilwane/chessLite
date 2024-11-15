#ifndef PIECES_H
#define PIECES_H
#include "./types.h"

int bitCount(U64 val); //change to inline and remove it from the header
//The following are getters and setters of the pieces where uppercase is white and lowercase is black
U64 get_K();
U64 get_Q();
U64 get_B();
U64 get_N();
U64 get_R();
U64 get_P();

U64 get_k();
U64 get_q();
U64 get_b();
U64 get_n();
U64 get_r();
U64 get_p();

U64 get_game();

void set_K(U64 K);
void set_Q(U64 Q);
void set_B(U64 B);
void set_N(U64 N);
void set_R(U64 R);
void set_P(U64 P);

void set_k(U64 k);
void set_q(U64 q);
void set_b(U64 b);
void set_n(U64 n);
void set_r(U64 r);
void set_p(U64 p);

void set_game(U64 val);

//The following are the masks of move generators
extern U64 notAFile;
extern U64 notHFile;
extern U64 notBFile;
extern U64 notGFile;
extern U64 notABFile;
extern U64 notGHFile;


//Use this function to create a bitboard of empty spaces
U64 createEmptySquares();
// Use this function to print binary numbers for pieces
void print_binary(U64 n);

//Use this function to print an 8x8 matrix of the bits
void print_matrix(U64 word);

//A function to find the index of the least significant bit in a bitboard
int debruijn_BitScan(U64 bitboard);

//This function returns an array of bitboards at the start of the game
void init_rack(char board[8][8]);

//This function is the only function that can be used to update the game bitboard
void updateGame();

//Single Move generators
U64 eastOne(U64 bitboard);
U64 westOne(U64 bitboard);
U64 northOne(U64 bitboard);
U64 southOne(U64 bitboard);
U64 northEastOne(U64 bitboard);
U64 southEastOne(U64 bitboard);
U64 northWestOne(U64 bitboard);
U64 southWestOne(U64 bitboard);

//Pawn  pushing generators

U64 wSinglePush(U64 wPawns, U64 emptySquares); //Empty squares is a bitboard that is set to 1 on all empty squares and 0 on all occupied squares
U64 wDoublePush(U64 wPawns, U64 emptySquares); 
U64 bSinglePush(U64 bPawns, U64 emptySquares);
U64 bDoublePush(U64 bPawns, U64 emptySquares);

U64 wSinglePushSources(U64 wPawns, U64 emptySquares); //Returns all sources of white pawn squares you can single push
U64 wDoublePushSources(U64 wPawns, U64 emptySquares); //Returns all sources of white pawn squares you can double push
U64 bSinglePushSources(U64 bPawns, U64 emptySquares); //Returns all sources of Black pawn squares you can single push
U64 bDoublePushSources(U64 bPawns, U64 emptySquares); //Returns all sources of Black pawn squares you can double push

//Knight moving generators
U64 knightMoveTargets(U64 knights);

//King moving generators
void init_king_targets();
U64 kingMoveTargets(U64 king, U64 empty);

//Bishop utilities
void init_bishop_masks();
U64 get_bishop_mask(int index);
U64 bishop_attack(int index, U64 occupancy);


//Rook utilities
void init_rook_masks();
U64 get_rook_mask();
U64 rook_attack(int square_index, U64 occupancy);


//Sliding pieces utilities
U64 set_occupancy(int index, U64 attack_mask, int bits_in_mask);
void set_sliding_attacks();


#endif