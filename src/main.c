#include <stdio.h>
#include "./pieces.h"
#include "./types.h"
#include "magics.h"
#include <omp.h>
#include <locale.h>
void func_a(int num){
    printf("\n %d\n", num);
    num = 95;
    printf("\n %d\n", num);
}

void func_b(int* num){
    printf("\n %d\n", num);
    printf("\n %d\n", *num);
    *num = 10;
    func_a(*num);
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

    printf("\n White bitboards \n");
    init_rack(board);
    init();
    
    //char *fen = "8/5k2/3p4/1p1Pp2p/pP2Pp1P/P4P1K/8/8 b - a1 99 50";
    char* fen = "8/8/8/8/k2Pp2Q/8/8/3K4 w - 0 1";
    fen_parser(fen);
    print_chessboard();
    Moves movelist;
    movelist.count = 0;
    
    printf("\n %d \n", movelist.count);
    generate(black, &movelist);
    print_generated_moves(&movelist);
    printf("\n THE FINAL NUBER OF MOVES GENERATED IS %d \n", movelist.count);
    //generate(white);
    return 0;
}