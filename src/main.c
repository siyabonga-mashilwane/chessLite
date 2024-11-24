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
    print_matrix(bishop_magic_attack(h6, get_game()));
    print_matrix(rook_magic_attack(b8, get_game()));
    return 0;
}