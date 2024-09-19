#ifndef PIECES_H
#define PIECES_H
//Capital letters as White and lowecase letters as black
extern unsigned long long K;
extern unsigned long long Q;
extern unsigned long long B;
extern unsigned long long N;
extern unsigned long long R;
extern unsigned long long P;

extern unsigned long long k;
extern unsigned long long q;
extern unsigned long long b;
extern unsigned long long n;
extern unsigned long long r;
extern unsigned long long p;

// Use this function to print binary numbers for pieces
void print_binary(unsigned long long n);

//This function returns an array of bitboards at the start of the game
void init_rack(char board[8][8]);

#endif