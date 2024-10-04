#include "./pieces.h"
#include <stdio.h>
#include <math.h>
#include "./types.h"

//Static function prototypes
static void kingMoveTargetsInitialiser();
static U64 kingMoveTargetsHelper(U64 king);

//Capital letters as White and lowecase letters as black
static U64 K = 0;
static U64 Q = 0;
static U64 B = 0;
static U64 N = 0;
static U64 R = 0;
static U64 P = 0;

static U64 k = 0;
static U64 q = 0;
static U64 b = 0; 
static U64 n = 0;
static U64 r = 0;
static U64 p = 0;

U64 kingMoves[64];//A list of all possible moves a king can move given a position on the board

static U64 game = 0;//This variable is used to store the game state bitboard

//The following are the masks of move generators
    //The masks make sure that the pieces do not go off bound
U64 notAFile = 0xfefefefefefefefe;
U64 notHFile = 0x7f7f7f7f7f7f7f7f;
U64 notBFile = 0xbfbfbfbfbfbfbfbf;
U64 notGFile = 0xfdfdfdfdfdfdfdfd;
U64 notABFile = 0x3f3f3f3f3f3f3f3f;
U64 notGHFile = 0xfcfcfcfcfcfcfcfc;

//Getters and setters for the game state and chess pieces
U64 get_K(){
    return K;
}
U64 get_Q(){
    return Q;
}
U64 get_B(){
    return B;
}
U64 get_N(){
    return N;
}
U64 get_R(){
    return R;
}
U64 get_P(){
    return P;
}

U64 get_k(){
    return k;
}
U64 get_q(){
    return q;
}
U64 get_b(){
    return b;
}
U64 get_n(){
    return n;
}
U64 get_r(){
    return r;
}
U64 get_p(){
    return p;
}

U64 get_game(){
    return game;
}

//setters
void set_K(U64 val){
    K = val;
    updateGame();
}
void set_Q(U64 val){
    Q = val;
    updateGame();
}
void set_B(U64 val){
    B = val;
    updateGame();
}
void set_N(U64 val){
    N = val;
    updateGame();
}
void set_R(U64 val){
    R = val;
    updateGame();
}
void set_P(U64 val){
    P = val;
    updateGame();
}

void set_k(U64 val){
    k = val;
    updateGame();
}
void set_q(U64 val){
    q = val;
    updateGame();
}
void set_b(U64 val){
    b = val;
    updateGame();
}
void set_n(U64 val){
    n = val;
    updateGame();
}
void set_r(U64 val){
    r = val;
    updateGame();
}
void set_p(U64 val){
    p = val;
    updateGame();
}

void set_game(U64 val){
    game = val;
    
}

//A function that will Update the game bitboard
void updateGame(){
    game = K | Q | B | N | R | P | k | q | b | n | r | p;
}
//A function that will produce a bitboard of empty spaces on the bitboard.
U64 createEmptySquares(){
    return ~game;
}
//A function that will print the U64 into a binary string
void print_binary(U64 n) {
    for (int i = sizeof(n) * 8 - 1; i >= 0; i--) {
        printf("%llu", (n >> i) & 1);  // Print each bit from MSB to LSB
    }
    printf("\n");
}
//A function that will print the U64 into a binary matrix/chess board
void print_matrix(U64 word) {
    for (int row = 7; row >= 0; row--) {
        for (int col = 7; col >= 0; col--) {
            // Extract the bit at position (row * 8 + col)
            int bit_position = row * 8 + col;
            U64 mask = (U64)1 << bit_position;
            printf("%d ", (word & mask) ? 1 : 0);
        }
        printf("\n");  // Newline for the next row
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
            U64 temp = 1;
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
    kingMoveTargetsInitialiser();
    updateGame();
}

//One step Move generators
U64 eastOne(U64 bitboard){
    return (bitboard & notAFile) >> 1;
}
U64 northEastOne(U64 bitboard){
    return (bitboard & notAFile) << 7;
}
U64 southEastOne(U64 bitboard){
    return (bitboard & notAFile) >> 9;
}
U64 westOne(U64 bitboard){
    return (bitboard & notHFile) << 1;
}
U64 northWestOne(U64 bitboard){
    return (bitboard & notHFile) << 9;
}
U64 southWestOne(U64 bitboard){
    return (bitboard & notHFile) >> 7;
}
U64 northOne(U64 bitboard){
    return bitboard << 8;
}
U64 southOne(U64 bitboard){
    return bitboard >> 8;
}

//Pawn push move generation

//Returns possible moves for a white pawn in one push
U64 wSinglePush(U64 wPawns, U64 emptySquares){
    return northOne(wPawns) & emptySquares;
}
//Returns possible moves for a white pawn in double push
U64 wDoublePush(U64 wPawns, U64 emptySquares){
    U64 row4 = 0x00000000FF000000; //sets all values in row 4 to be 1
    U64 pushOne = wSinglePush(wPawns,emptySquares);
    return northOne(pushOne) & emptySquares & row4;
}

//Returns possible moves for a black pawn in one push
U64 bSinglePush(U64 bPawns, U64 emptySquares){
    return southOne(bPawns) & emptySquares;
}
//Returns possible moves for a black pawn in double push
U64 bDoublePush(U64 bPawns, U64 emptySquares){
    U64 row5 = 0x000000FF00000000; //sets all values in row 4 to be 1
    U64 pushOne = bSinglePush(bPawns,emptySquares);
    return southOne(pushOne) & emptySquares & row5;
}

//Analogy, if moving the white pawns up against the empty squares gives destination squares then moving 
//the empty squares against the white pawn will return the source positions so:
U64 wSinglePushSources(U64 wPawns, U64 emptySquares){
    return southOne(emptySquares) & wPawns;
}
U64 wDoublePushSources(U64 wPawns, U64 emptySquares){
    U64 row4 = 0x00000000FF000000;
    U64 emptyRow3 = southOne(emptySquares & row4) & emptySquares; //If the empty squares on rank 4 can be moved to rank3 
    return wSinglePushSources(wPawns, emptyRow3);
}


U64 bSinglePushSources(U64 bPawns, U64 emptySquares){
    return northOne(emptySquares) & bPawns;
}

U64 bDoublePushSources(U64 bPawns, U64 emptySquares){
    U64 row5 = 0x000000FF00000000;
    U64 emptyRow6 = northOne(emptySquares & row5) & emptySquares;
    return bSinglePushSources(bPawns, emptyRow6);
}

//Knight moving generators
    
//Returns all the possible moves for all the knights on a particular bitboard
U64 knightMoveTargets(U64 knights){
    U64 noNoEast = (knights << 17) & notAFile;
    U64 noNoWest = (knights << 18) & notHFile;
    U64 soSoEast = (knights >> 18) & notAFile;
    U64 soSoWest = (knights >> 17) & notHFile;
    U64 weWENorth = (knights << 10) & notGHFile;
    U64 weWeSouth = (knights >> 6) & notGHFile;
    U64 eaEaNorth = (knights << 6) & notABFile;
    U64 eaEaSouth = (knights >> 10) & notABFile;

    return (noNoEast | noNoWest | soSoEast | soSoWest | weWENorth | weWeSouth | eaEaNorth | eaEaSouth);
}

//King move generators

//Helper function for calculating all the direction moves of a king
static U64 kingMoveTargetsHelper(U64 king){
    U64 noWest = northWestOne(king);
    U64 soWest = southWestOne(king);
    U64 soEast = southEastOne(king);
    U64 noEast = northEastOne(king);
    U64 north = northOne(king);
    U64 south = southOne(king);
    U64 west = westOne(king);
    U64 east = eastOne(king);

    return (noWest | soWest | soEast | noEast | north | south | west | east);
}
//Ran at the start of the game to initialize the kingmoves array which holds all possible moves of a king in a board.
static void kingMoveTargetsInitialiser(){
    U64 king = 1;
    for (size_t i = 0; i < 64; i++, king = king<<1)
    {
        kingMoves[i] = kingMoveTargetsHelper(king);
    }
}
//A function that returns the possible moves of a king in a given game state/position.
U64 kingMoveTargets(U64 king, U64 empty){
    int index = (int)log2(king);
    return kingMoves[index] & empty;
}



/*{'r','n','b','q','k','b','n','r'},
        {'p','p','p','p','p','p','p','p'},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {'P','P','P','P','P','P','P','P'},
        {'R','N','B','Q','K','B','N','R'}*/