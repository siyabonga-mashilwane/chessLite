#include <stdio.h>
#include "pieces.h"
int main(){
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
    init_rack(board);
    print_binary(K);
    print_binary(Q);
    print_binary(B);
    print_binary(N);
    print_binary(R);
    print_binary(p);
    print_binary(k);
    print_binary(q);
    print_binary(b);
    print_binary(n);
    print_binary(r);
    print_binary(p);
    return 0;
}