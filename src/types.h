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
    h8, g8, f8, e8, d8, c8, b8, a8
} Square;

enum {bishop, rook};
#endif