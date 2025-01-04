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
    
    //char *fen = "8/5k2/3p4/1p1Pp2p/pP2Pp1P/P4P1K/8/8 b - a1 99 50";
    //fen_parser(fen);
    print_chessboard();
    //generate(white);
    printf("%d :\n" , h8/8);
    return 0;
}