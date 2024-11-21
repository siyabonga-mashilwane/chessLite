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
    int RBits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
    };

    int BBits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
    };
    printf("\n White bitboards \n");
    init_rack(board);

    int square;
    printf("const uint64 RMagic[64] = {\n");
    for(square = 0; square < 64; square++)
        printf("  0x%llxULL,\n", find_magic(square, RBits[square], 1));
    printf("};\n\n");

    printf("const uint64 BMagic[64] = {\n");
    for(square = 0; square < 64; square++)
        printf("  0x%llxULL,\n", find_magic(square, BBits[square], 0));
    printf("};\n\n");
    
    return 0;
}