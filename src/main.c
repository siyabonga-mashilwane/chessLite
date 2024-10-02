#include <stdio.h>
#include "./pieces.h"
#include "./types.h"

int main(){
    char board[8][8] = {
        {'r',' ','b','q','k',' ','n','r'},
        {'p','p','p','p',' ','p','p','p'},
        {' ',' ','n',' ',' ',' ',' ',' '},
        {' ','B',' ',' ','p',' ',' ',' '},
        {' ',' ',' ',' ','P',' ',' ',' '},
        {'b',' ',' ',' ',' ','N',' ',' '},
        {'P','P','P','P',' ','P','P','P'},
        {'R','N','B','Q','K',' ',' ','R'}
    };
    printf("\n White bitboards \n");
    init_rack(board);
    print_matrix(get_K());
    print_matrix(get_Q());
    print_matrix(get_B());
    print_matrix(get_N());
    print_matrix(get_R());
    print_matrix(get_P());
    printf("\n Black bitboards \n");
    print_matrix(get_k());
    print_matrix(get_q());
    print_matrix(get_b());
    print_matrix(get_n());
    print_matrix(get_r());
    print_matrix(get_p());
    printf("\n");
    print_matrix(get_game());
    printf("\n");
    print_matrix(createEmptySquares());
    printf("\n");
    print_matrix(bDoublePushSources(get_p(), createEmptySquares()));
    printf("\n");
    print_matrix(bSinglePushSources(get_p(), createEmptySquares()));

    return 0;
}