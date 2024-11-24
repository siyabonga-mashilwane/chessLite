#include <stdio.h>
#include "./pieces.h"
#include "./types.h"
#include "magics.h"
#include <omp.h>

int main(){
    //omp_set_num_threads(10);
    char board[8][8] = {
        {'r',' ',' ','q','k',' ','n','r'},
        {'p','p','p','p',' ','p','p','p'},
        {' ',' ','n',' ',' ',' ',' ',' '},
        {' ',' ',' ',' ','p',' ',' ',' '},
        {' ',' ',' ',' ','P',' ',' ',' '},
        {'b',' ',' ','B',' ','N',' ','R'},
        {'P','P','P','P',' ','P','P','P'},
        {' ','N','B','Q','K',' ',' ',' '}
    };
    
    printf("\n White bitboards \n");
    init_rack(board);
    init();
    //printf("\n %d\n", char_pieces['P']);
    //printf("\n %c\n", ascii_pieces[P]);
    print_chessboard();
    //print_pieces();
    char *fen = "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1";
    fen_parser(fen);
    return 0;
}