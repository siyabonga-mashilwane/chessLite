#include "pieces.h"
#include <stdio.h>
unsigned long long K = 0;
unsigned long long Q = 0;
unsigned long long B = 0;
unsigned long long N = 0;
unsigned long long R = 0;
unsigned long long P = 0;

unsigned long long k = 0;
unsigned long long q = 0;
unsigned long long b = 0; 
unsigned long long n = 0;
unsigned long long r = 0;
unsigned long long p = 0;

void print_binary(unsigned long long n) {
    for (int i = sizeof(n) * 8 - 1; i >= 0; i--) {
        printf("%llu", (n >> i) & 1);  // Print each bit from MSB to LSB
    }
    printf("\n");
}
//A function that will trnsform the char array into chess bitboards
void init_rack(char board[8][8])
{
    for (int i = 0; i < 8; i++)
    {
        
        for (int j = 0; j < 8; j++)
        {
            unsigned long long temp = 1;
            int shift = 63 - ((i*8) + j); //There are 64 squares and on those 1 of them is occupied by 1 so 63  available i*8+j are offsets to normalise to left bitshift
            temp = temp << shift;
            switch (board[i][j])
            {
            case 'K':
                K = K ^ temp;
                break;
            case 'Q':
                Q = Q ^ temp;
                break;
            case 'B':
                B = B ^ temp;
                break;
            case 'N':
                N = N ^ temp;
                break;
            case 'R':
                R = R ^ temp;
                break;
            case 'P':
                P = P ^ temp;
                break;
            case 'k':
                k = k ^ temp;
                break;
            case 'q':
                q = q ^ temp;
                break;
            case 'b':
                b = b ^ temp;
                break;
            case 'n':
                n = n ^ temp;
                break;
            case 'r':
                r = r ^ temp;
                break;
            case 'p':
                p = p ^ temp;
                break;
            default:
                break;
            }
        }
    }
    
}
