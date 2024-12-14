#include <stdio.h>
#include "./pieces.h"
#include "./types.h"
#include "magics.h"
#include <omp.h>
#include <locale.h>


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
    setlocale(LC_ALL, "en_US.UTF-16");

    printf("\n White bitboards \n");
    init_rack(board);
    init();
    //printf("\n %d\n", char_pieces['P']);
    //printf("\n %c\n", ascii_pieces[P]);
    //print_chessboard();
    //print_pieces();
    /*char *fen = "8/5k2/3p4/1p1Pp2p/pP2Pp1P/P4P1K/8/8 b - a1 99 50";
    fen_parser(fen);
    print_chessboard();*/

    //printf("\n%d ", is_square_attacked(c6, white, get_game()));
    //get_pawn_attack(black, 45);
    /*printf("\n a2 - a7 \n");
    print_matrix(0x0001010101010100);
    printf("\n b2 - g7 \n");
    print_matrix(0x0040201008040200);
    printf("\n h1 - b7 \n");
    print_matrix(0x0002040810204080);*/
    print_matrix(get_in_between(c1, f1));
    return 0;
}