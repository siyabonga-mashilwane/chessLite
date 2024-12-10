#ifndef TYPES_H
#define TYPES_H

typedef unsigned long long U64;
#define BOARDS_SQUARES  64
#define ROOK_COMBINATIONS 4096
#define BISHOP_COMBINATIONS 512
//setting up the proper square data type for the chess board
typedef enum {
    h1, g1, f1, e1, d1, c1, b1, a1,
    h2, g2, f2, e2, d2, c2, b2, a2,
    h3, g3, f3, e3, d3, c3, b3, a3,
    h4, g4, f4, e4, d4, c4, b4, a4,
    h5, g5, f5, e5, d5, c5, b5, a5,
    h6, g6, f6, e6, d6, c6, b6, a6,
    h7, g7, f7, e7, d7, c7, b7, a7,
    h8, g8, f8, e8, d8, c8, b8, a8, no_sq
} Square;


//All the uppercase letters are white pieces and all the lowercase letters are black pieces
typedef enum {K, Q, B, N, R, P, k, q, b, n, r, p} Pieces;

//Defines the sliding pieces
typedef enum {bishop, rook} SL_Piece;

//Which colour/side to move
typedef enum { white, black} Colour;

typedef enum {false, true} bool;

struct Magic_sq {
   U64 mask;  // to mask relevant squares of both lines (no outer squares)
   U64 magic; // magic 64-bit factor
   int bits; // number of bits of the mask at that square
};

//Castling enum
// Bit explanations, if :
//  0001 = 1 -> White king can catle on the king side
//  0010 = 2 -> White king can catle on the queen side
//  0100 = 4 -> Black king can catle on the king side
//  1000 = 8 -> Black king can castle on the queen side
// thus 1111 = 16 -> every king has castling rights in all directions
enum {wk = 1, wq = 2, bk = 4, bq = 8};
#endif