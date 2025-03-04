#include <stdio.h>
#include "./pieces.h"
#include "./types.h"
#include "magics.h"
#include <omp.h>
#include <locale.h>
#include <string.h>




int main(){
    //omp_set_num_threads(10);
    char board[8][8] = {
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', 'r', 'p', ' ', ' ', ' ', ' '},
        {' ', ' ', 'P', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
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
    //printf("\n Number of nodes is: %llu \n", Perft(5, white));
    //fen_parser("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/P1N2Q1p/1PPBBPPP/1R2K2R w Kkq - 0 1");
    fen_parser("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    //fen_parser("r3k2r/p1ppqpb1/bn2pnp1/1B1PN3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R b KQkq - 0 1");
    /*print_chessboard();
    Moves temp;
    temp.count = 0;
    memset(temp.moves, 0, sizeof(temp.moves));
    generate(white, &temp);
    printf("number of moves %d\n", temp.count);
    print_generated_moves(&temp);*/
    printf("\nNodes searched: %llu \n", Divide(1));
    
    return 0;
}

/*"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1"
"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1"
"r3k2r/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1"
"r3k2r/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1"*/