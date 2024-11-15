#include <stdio.h>
#include "./pieces.h"
#include "./types.h"

int main(){
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
    /*print_matrix(get_K());
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
    print_matrix(bSinglePushSources(get_p(), createEmptySquares()));*/
    //print_matrix(0x123456789ABCDEF0ULL);
    //print_matrix(__builtin_bswap64(0x123456789ABCDEF0ULL));
    //U64 rook_mask = get_rook_mask(7);
    //print_matrix(rook_mask);
    //printf("\n");
    U64 bishop_mask = get_bishop_mask(28);
    print_matrix(set_occupancy(511,bishop_mask,bitCount(bishop_mask)));
    //printf("\n");
    //print_matrix(rook_attack(16, get_game()));
    return 0;
}