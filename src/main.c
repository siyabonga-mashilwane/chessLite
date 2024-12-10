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
    //print_chessboard();
    //print_pieces();
    char *fen = "8/5k2/3p4/1p1Pp2p/pP2Pp1P/P4P1K/8/8 b - a1 99 50";
    fen_parser(fen);
    print_chessboard();
    printf("\n%d ", is_square_attacked(c6, white, get_game()));
    //get_pawn_attack(black, 45);
    
    return 0;
}