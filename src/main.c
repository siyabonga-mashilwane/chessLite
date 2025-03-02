#include <stdio.h>
#include "./pieces.h"
#include "./types.h"
#include "magics.h"
#include <omp.h>
#include <locale.h>
U64 Perft(int depth, Colour side)
{
    Moves movelist;
    movelist.count = 0;
    int n_moves, i;
    U64 nodes = 0;

    if (depth == 0) 
        return 1ULL;
    generate(side, &movelist);
    n_moves = movelist.count;
    printf("Generated %d moves at depth %d, side is %s\n", n_moves, depth, (side==white?"white":"black:"));
    for (i = 0; i < n_moves; i++) {
        Move_History prev_move = make_move(movelist.moves[i], side);
        nodes += Perft(depth-1, side^1);
        unmake_move(&prev_move);
    }
    
    return nodes;
}
int main(){
    //omp_set_num_threads(10);
    char board[8][8] = {
        {'r','n','b','q','k','b','n','r'},
        {'p','p','p','p','p','p','p','p'},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {'P','P','P','P','P','P','P','P'},
        {'R','N','B','Q','K','B','N','R'}
    };
    setlocale(LC_ALL, "en_US.UTF-16");

    //printf("\n White bitboards \n");
    init_rack(board);
    init();
    //print_chessboard();
    
    //char *fen = "8/5k2/3p4/1p1Pp2p/pP2Pp1P/P4P1K/8/8 b - a1 99 50";
    //char* fen = "8/8/8/8/k2Pp2Q/8/8/3K4 w - 0 1";
    //fen_parser(fen);
    /*print_chessboard();
    Moves movelist;
    movelist.count = 0;
    
    printf("\n %d \n", movelist.count);
    generate(black, &movelist);
    print_generated_moves(&movelist);
    printf("\n THE FINAL NUBER OF MOVES GENERATED IS %d \n", movelist.count);
    //generate(white);
    */
    printf("\n Number of nodes is: %llu \n", Perft(3, white));
    return 0;
}