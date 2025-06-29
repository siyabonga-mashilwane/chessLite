#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "pieces.h"

int material_score[12] = {
    10000,  // white king score
    1000,   // white queen score
    350,    // White bishop score
    300,   // White knight score
    500,   // White rook score
    100,   // White pawn score
    -10000, // black king score
    -1000,  // black queen score
    -350,   // Black bishop score
    -300,   // Black knight score
    -500,   // Black rook score
    -100   // Black pawn score
};

// pawn positional score
const int pawn_score[64] = 
{
    90,  90,  90,  90,  90,  90,  90,  90,
    30,  30,  30,  40,  40,  30,  30,  30,
    20,  20,  20,  30,  30,  30,  20,  20,
    10,  10,  10,  20,  20,  10,  10,  10,
     5,   5,  10,  20,  20,   5,   5,   5,
     0,   0,   0,   5,   5,   0,   0,   0,
     0,   0,   0, -10, -10,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0
};

// knight positional score
const int knight_score[64] = 
{
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5,   0,   0,  10,  10,   0,   0,  -5,
    -5,   5,  20,  20,  20,  20,   5,  -5,
    -5,  10,  20,  30,  30,  20,  10,  -5,
    -5,  10,  20,  30,  30,  20,  10,  -5,
    -5,   5,  20,  10,  10,  20,   5,  -5,
    -5,   0,   0,   0,   0,   0,   0,  -5,
    -5, -10,   0,   0,   0,   0, -10,  -5
};

// bishop positional score
const int bishop_score[64] = 
{
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,  20,   0,  10,  10,   0,  20,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,  10,   0,   0,   0,   0,  10,   0,
     0,  30,   0,   0,   0,   0,  30,   0,
     0,   0, -10,   0,   0, -10,   0,   0
};

// rook positional score
const int rook_score[64] =
{
    50,  50,  50,  50,  50,  50,  50,  50,
    50,  50,  50,  50,  50,  50,  50,  50,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,   0,  20,  20,   0,   0,   0

};

// king positional score
const int king_score[64] = 
{
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   5,   5,   5,   5,   0,   0,
     0,   5,   5,  10,  10,   5,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   0,   5,  10,  10,   5,   0,   0,
     0,   5,   5,  -5,  -5,   0,   5,   0,
     0,   0,   5,   0, -15,   0,  10,   0
};

// This array is used to map the square indices of black pieces to their corresponding white squares
const Square mirror_square[64] = {
    h8, g8, f8, e8, d8, c8, b8, a8,
    h7, g7, f7, e7, d7, c7, b7, a7,
    h6, g6, f6, e6, d6, c6, b6, a6,
    h5, g5, f5, e5, d5, c5, b5, a5,
    h4, g4, f4, e4, d4, c4, b4, a4,
    h3, g3, f3, e3, d3, c3, b3, a3,
    h2, g2, f2, e2, d2, c2, b2, a2,
    h1, g1, f1, e1, d1, c1, b1, a1
};


int evaluate(U64 bitboard_pieces[12], Colour side) {
    int score = 0;
    for (Pieces i = K; i < p; i++) {
        U64 piece = bitboard_pieces[i];
        while(piece){ // Loop over all bits
            int square = debruijn_BitScan(piece);
            reset_lsb(piece);
            score += material_score[i];
            switch (i) {
                case P: // White pawn
                    score += pawn_score[square];
                    break;
                case p: // Black pawn
                    score -= pawn_score[mirror_square[square]];
                    break;
                case N: // White knight
                    score += knight_score[square];
                    break;
                case n: // Black knight
                    score -= knight_score[mirror_square[square]];
                    break;
                case B: // White bishop
                    score += bishop_score[square];
                    break;
                case b: // Black bishop
                    score -= bishop_score[mirror_square[square]];
                    break;
                case R: // White rook
                    score += rook_score[square];
                    break;
                case r: // Black rook
                    score -= rook_score[mirror_square[square]];
                    break;
                case Q: // White queen
                    score += 0; // No positional score for queen in this evaluation
                    break;
                case q: // Black queen
                    score -= 0; // No positional score for queen in this evaluation
                    break;
                case K: // White king
                    score += king_score[square];
                    break;
                case k: // Black king
                    score -= king_score[mirror_square[square]];
                    break;
                default:
                    continue; // No score for no_piece or any other piece not handled above
            }
        }
    }
    return (side == white) ? score : -score;
}

int ply = 0;
unsigned int best_move;
int nodes = 0;
int negamax(int depth, int alpha, int beta){
    if (depth == 0){
        return evaluate(get_bitboard_pieces(), get_side());
    }
    nodes++;

    U64* bitboard_pieces = get_bitboard_pieces();

    unsigned int temporal_best = 0;
    int old_alpha = alpha;

    Moves movelist;
    movelist.count = 0;
    memset(movelist.moves, 0, sizeof(movelist.moves));

    generate(&movelist); // Generate moves for the current side
    for (int i = 0; i < movelist.count; i++) {
        unsigned int move = movelist.moves[i];
        ply++;

        Move_History prev_move = make_move(move);
        int score = -negamax(depth - 1, -beta, -alpha);
        unmake_move(&prev_move);

        ply--;
        
        if (score >= beta) {
            return beta;
        }
        if (score > alpha) {
            alpha = score;
            if(ply == 0) {
                temporal_best = move; // Store the best move at the root level
            }
        }
    }
    if(old_alpha != alpha){
        best_move = temporal_best;
    } 
    return alpha;
}

void search_best_move(int depth) {
    best_move = 0;
    nodes = 0;
    ply = 0;

    int alpha = -1000000;
    int beta = 1000000;

    int score = negamax(depth, alpha, beta);

    printf("Best move: %s, Nodes: %d\n", square_to_coordinates[get_source(best_move)], nodes);
}