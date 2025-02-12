#include "./pieces.h"
#include <stdio.h>
#include <math.h>
#include "types.h"
#include "magics.h"
#include <string.h>
#include <stdint.h>
#include <wchar.h>
#include <locale.h>
#include <windows.h>


#define get_bit(bitboard, square) ((bitboard) & (1ULL << square))
#define pop_bit(bitboard, square) (get_bit((bitboard), square) ? ((bitboard) ^= (1ULL << square)) : 0)
#define set_bit(bitboard, square) ((bitboard) |= (1ULL << square))
#define create_empty_squares(bitboard) (~(bitboard)) //will produce a bitboard of empty spaces on the bitboard.


//Static function prototypes
static U64 kingMoveTargetsHelper(U64 king);
static U64 bishop_mask_generator(int index);
static U64 rook_mask_generator(int index);

//Capital letters as White and lowecase letters as black
U64 bitboard_pieces[12]; //An array to hold all the bitboards of the game

//Pawn attack arrays
U64 pawn_attacks[2][BOARDS_SQUARES];

//Knight attacks
U64 knight_attacks[BOARDS_SQUARES];

//King attack array
static U64 king_attacks[BOARDS_SQUARES];//A list of all possible moves a king can move given a position on the board

//***** Sliding Pieces *****/
static U64 bishopMasks[BOARDS_SQUARES]; //A list of all possible masks a bishop can have given a position on the board
static U64 rookMasks[BOARDS_SQUARES]; //A list of all possible rook masks a rook can have given a position on the booard
U64 bishop_magic_attacks[BOARDS_SQUARES][BISHOP_COMBINATIONS]; //Each square for a bishop has a maximum of 512 occupancy combination combinations  (2^9) to consider hence the 512 
U64 rook_magic_attacks[BOARDS_SQUARES][ROOK_COMBINATIONS];//Each square for a rook has a maximum of 4096 occupancy combinations (2^12) to consider hence 4096
struct Magic_sq magic_bishop[64];
struct Magic_sq magic_rook[64];

//***** Arrays for Rectangular look-up, holds all instances of whether there is an attacked square in between 2 squares *****/
U64 arr_rectangular[BOARDS_SQUARES][BOARDS_SQUARES];
U64 line_rectangular[BOARDS_SQUARES][BOARDS_SQUARES];

//*********** Most likely to be moved to generator.c */
int castle = 0;
int side = -1;
int enpessant = no_sq;
int half_moves = 0;
int full_moves = 0;
//*****************************************/

static U64 game = 0;//This variable is used to store the game state bitboard

//The following are the masks of move generators
 //The masks make sure that the pieces do not go off bound
const U64 notAFile = 0x7f7f7f7f7f7f7f7f;
const U64 notHFile = 0xfefefefefefefefe;
const U64 notBFile = 0xbfbfbfbfbfbfbfbf;
const U64 notGFile = 0xfdfdfdfdfdfdfdfd;
const U64 notABFile = 0x3f3f3f3f3f3f3f3f;
const U64 notGHFile = 0xfcfcfcfcfcfcfcfc;

void init(){
    init_king_targets();
    init_rook_masks();
    init_bishop_masks();
    init_magic_attacks(rook_magic_numbers, bishop_magic_numbers);
    init_pawn_attacks();
    init_knight_attacks();
    init_in_between();
}
//Getters and setters for the game state and chess pieces
U64 get_K(){
    return bitboard_pieces[K];
}
U64 get_Q(){
    return bitboard_pieces[Q];
}
U64 get_B(){
    return bitboard_pieces[B];
}
U64 get_N(){
    return bitboard_pieces[N];
}
U64 get_R(){
    return bitboard_pieces[R];
}
U64 get_P(){
    return bitboard_pieces[P];
}

U64 get_k(){
    return bitboard_pieces[k];
}
U64 get_q(){
    return bitboard_pieces[q];
}
U64 get_b(){
    return bitboard_pieces[b];
}
U64 get_n(){
    return bitboard_pieces[n];
}
U64 get_r(){
    return bitboard_pieces[r];
}
U64 get_p(){
    return bitboard_pieces[p];
}

//Function used to return the game bitboard, both the white and white bitboards
U64 get_game(){
    return game;
}

/*
Function used to return the black or white bitboard, depending on the colour passed in
representing all the black pieces on the board or all the white pieces on the board
*/

U64 get_side_bitboard(Colour side){
    U64 bitboard = 0;
    if(side == white){
        for (Square i = K; i <= P; i++)
        {
            bitboard |= bitboard_pieces[i];
        }
    }else if(side == black){
        for (Square i = k; i <= p; i++)
        {
            bitboard |= bitboard_pieces[i];
        }
    }else{
        printf("\n Invalid side passed in \n");
    }
    return bitboard;
}
//setters
void set_K(U64 val){
    bitboard_pieces[K] = val;
    updateGame();
}
void set_Q(U64 val){
    bitboard_pieces[Q] = val;
    updateGame();
}
void set_B(U64 val){
    bitboard_pieces[B] = val;
    updateGame();
}
void set_N(U64 val){
    bitboard_pieces[N] = val;
    updateGame();
}
void set_R(U64 val){
    bitboard_pieces[R] = val;
    updateGame();
}
void set_P(U64 val){
    bitboard_pieces[P] = val;
    updateGame();
}

void set_k(U64 val){
    bitboard_pieces[k] = val;
    updateGame();
}
void set_q(U64 val){
    bitboard_pieces[q] = val;
    updateGame();
}
void set_b(U64 val){
    bitboard_pieces[b] = val;
    updateGame();
}
void set_n(U64 val){
    bitboard_pieces[n] = val;
    updateGame();
}
void set_r(U64 val){
    bitboard_pieces[r] = val;
    updateGame();
}
void set_p(U64 val){
    bitboard_pieces[p] = val;
    updateGame();
}

void set_game(U64 val){
    game = val;
    
}

//A function that will Update the game bitboard
void updateGame(){
    game = 0;
    for (size_t i = 0; i < 12; i++)
    {
        game |= bitboard_pieces[i];
    }
}
//A counting routine to count the number of bits in a bitboard
int bitCount(U64 val){
    int n = 0;
    while(val){
        val &= (val -1);
        n++;
    }
    return n;
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
    printf("\n");
    for (int row = 7; row >= 0; row--) {
        for (int col = 7; col >= 0; col--) {
            // Extract the bit at position (row * 8 + col)
            int bit_position = row * 8 + col;
            U64 mask = 1ULL << bit_position;
            printf("%d ", (word & mask) ? 1 : 0);
        }
        printf("\n");  // Newline for the next row
    }
    printf("\n");
}

//A function that will trnsform the char array into chess bitboards
void init_rack(char board[8][8])
{
    memset(bitboard_pieces, 0ULL, sizeof(bitboard_pieces));
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
                bitboard_pieces[K] ^= temp;
                break;
            case 'Q':
                bitboard_pieces[Q] ^= temp;
                break;
            case 'B':
                bitboard_pieces[B] ^= temp;
                break;
            case 'N':
                bitboard_pieces[N] ^= temp;
                break;
            case 'R':
                bitboard_pieces[R] ^= temp;
                break;
            case 'P':
                bitboard_pieces[P] ^= temp;
                break;
            case 'k':
                bitboard_pieces[k] ^= temp;
                break;
            case 'q':
                bitboard_pieces[q] ^= temp;
                break;
            case 'b':
                bitboard_pieces[b] ^= temp;
                break;
            case 'n':
                bitboard_pieces[n] ^= temp;
                break;
            case 'r':
                bitboard_pieces[r] ^= temp;
                break;
            case 'p':
                bitboard_pieces[p] ^= temp;
                break;
            default:
                break;
            }
        }
    }
    updateGame();
}



//One step Move generators
U64 eastOne(U64 bitboard){
    return (bitboard & notHFile) >> 1;
}
U64 northEastOne(U64 bitboard){
    return (bitboard & notHFile) << 7;
}
U64 southEastOne(U64 bitboard){
    return (bitboard & notHFile) >> 9;
}
U64 westOne(U64 bitboard){
    return (bitboard & notAFile) << 1;
}
U64 northWestOne(U64 bitboard){
    return (bitboard & notAFile) << 9;
}
U64 southWestOne(U64 bitboard){
    return (bitboard & notAFile) >> 7;
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
void init_pawn_attacks(){
    //setting up Whites attack table
    for (int i = 0; i < BOARDS_SQUARES; i++)
    {
        U64 board = (1ULL << i); // initialise the boatd to contain a pawn at that square
        pawn_attacks[white][i] = ((northWestOne(board) & (notHFile)) | (northEastOne(board)  & notAFile));     
    }
    
    //setting up Black's attack table
    for (int i = 0; i < BOARDS_SQUARES; i++)
    {
        U64 board = (1ULL << i); // initialise the boatd to contain a pawn at that square
        pawn_attacks[black][i] = (southWestOne(board) | southEastOne(board));     
    }
}
//A function that is used to return the attack bitboard of a pawn on a particular square.
U64 get_pawn_attack(Colour side, int square){
    /*printf("\n The square for the PAWN is: %d \n", square);
    printf("\n The side for the PAWN is: %d \n", side);
    print_matrix(pawn_attacks[side][square]);*/
    return pawn_attacks[side][square];
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
    U64 noNoEast = (knights << 15) & notAFile;
    U64 noNoWest = (knights << 17) & notHFile;
    U64 soSoEast = (knights >> 17) & notAFile;
    U64 soSoWest = (knights >> 15) & notHFile;
    U64 weWENorth = (knights << 10) & notGHFile;
    U64 weWeSouth = (knights >> 6) & notGHFile;
    U64 eaEaNorth = (knights << 6) & notABFile;
    U64 eaEaSouth = (knights >> 10) & notABFile;

    return (noNoEast | noNoWest | soSoEast | soSoWest | weWENorth | weWeSouth | eaEaNorth | eaEaSouth);
}

//Used to initialise the knight attack table
void init_knight_attacks(){
    for (size_t i = 0; i < BOARDS_SQUARES; i++)
    {
        U64 knight = 1ULL << i;
        knight_attacks[i] = knightMoveTargets(knight);
    }
}

//A function that returns the possible moves of a knight given a particular square
U64 get_knight_attack(int square){
    return knight_attacks[square];
}

//King move generators

//Helper function for calculating all the direction moves of a king
static U64 kingMoveTargetsHelper(U64 king){
    U64 noWest = northWestOne(king & notAFile);
    U64 soWest = southWestOne(king & notAFile);
    U64 soEast = southEastOne(king & notHFile);
    U64 noEast = northEastOne(king & notHFile);
    U64 north = northOne(king);
    U64 south = southOne(king);
    U64 west = westOne(king & notAFile);
    U64 east = eastOne(king & notHFile);

    return (noWest | soWest | soEast | noEast | north | south | west | east);
}
//Ran at the start of the game to initialize the king_attacks array which holds all possible moves of a king in a board.
void init_king_targets(){
    U64 king = 1ULL;
    for (size_t i = 0; i < BOARDS_SQUARES; i++, king = king<<1)
    {
        king_attacks[i] = kingMoveTargetsHelper(king);
    }
}

//A function that returns the possible moves of a king in a given game state/position.
U64 get_king_target(int index, U64 empty){
    return king_attacks[index] & empty;
}

//Bishop utilities

// A function that generates bitmasks for a bishop given an index
static U64 bishop_mask_generator(int index){
    U64 mask = 0ULL;
    //break down index to int rank and file
    int r = index/8;
    int f = index % 8;
    int i,j;
    //all the +9 bits from the current index
    for (i = r+1, j = f+1; i <= 6 && j<=6; i++, j++)
    {
        mask |= (1ULL << i*8 + j);
    }
    //all the +7 bits from the current index
    for (i = r+1, j = f-1; i <= 6 && j>0; i++, j--)
    {
        mask |= (1ULL << i*8 + j);
    }
    //all the -7 bits from the current index
    for (i = r-1, j = f+1; i > 0 && j<=6; i--, j++)
    {
        mask |= (1ULL << i*8 + j);
    }
    //all the -9 bits from the current index
    for (i = r-1, j = f-1; i > 0 && j > 0; i--, j--)
    {
        mask |= (1ULL << i*8 + j);
    }
    return mask;

}

void init_bishop_masks(){
    for (int i = 0; i < BOARDS_SQUARES; i++){
        bishopMasks[i] = bishop_mask_generator(i);
    }
}

U64 get_bishop_mask(int index) {
    return bishopMasks[index];
}

U64 bishop_attack(int square_index, U64 occupancy) {
    U64 north_west = 0ULL;
    U64 south_west = 0ULL;
    U64 south_east = 0ULL;
    U64 north_east = 0ULL;


    int rank = square_index / 8;
    int file = square_index % 8;
    int count = square_index;
    //Calculating the north west atatacks 
    for ( count+=9, file+= 1, rank += 1 ; (file <= 7) && (rank <= 7); rank++, file++, count+=9)
    {
        north_west |= 1ULL << count;
        //Break if attack ray meets piece on the board
        if (get_bit(occupancy, count) & get_bit(north_west, count)) break;
        
    }
    //Simply reset the indexing variables for further use in other rays
    rank = square_index / 8;
    file = square_index % 8;
    count = square_index;

    //Calculating the north east atacks
    for (count += 7, file -= 1, rank += 1; (rank <= 7) && (file >= 0); file--, rank++, count += 7)
    {
        north_east |= 1ULL << count;
        //Break if attack ray meets piece on the board
        if (get_bit(occupancy, count) & get_bit(north_east, count)) break;
    }

    //Reseting the indexing variables for further use in other rays
    rank = square_index / 8;
    file = square_index % 8;
    count = square_index;

    //Calculating south west attacks
    for ( count -= 7, rank--, file++; (file <= 7) && (rank >= 0); count -= 7, rank--, file++)
    {
        south_west |= 1ULL << count;
        //Break if attack ray meets piece on the board
        if (get_bit(occupancy, count) & get_bit(south_west, count)) break;
    }

    //Reseting the indexing variables for further use in other rays
    rank = square_index / 8;
    file = square_index % 8;
    count = square_index;

    //Calculating south east attacks
    for( count -= 9, rank--, file--; (file >= 0) && (rank >= 0); count -= 9, rank--, file--){
        south_east |= 1ULL << count;
        //Break if attack ray meets piece on the board
        if (get_bit(occupancy, count) & get_bit(south_east, count)) break;
    }
    
    return south_east | south_west | north_east | north_west;
}

U64 bishop_magic_attack(int square_index, U64 occupancy){
    U64 occupied = occupancy;
    occupied &= magic_bishop[square_index].mask;
    occupied *= magic_bishop[square_index].magic;
    occupied >>= (int)(64 - (magic_bishop[square_index].bits));
    return bishop_magic_attacks[square_index][occupied];
}

//Rook utilities

void init_rook_masks(){
    for (size_t i = 0; i < BOARDS_SQUARES; i++)
    {
        rookMasks[i] = rook_mask_generator(i); 
    }
}

static U64 rook_mask_generator(int index){
    U64 mask = 0ULL;
    //break down index to int rank and file
    int r = index/8;
    int f = index % 8;
    int i,j;
    for (i = r+1; i < 7; i++)
    {
        mask |= (1ULL << i*8 + f);
    }
    for (i = r-1; i > 0; i--)
    {
        mask |= (1ULL << i*8 + f);
    }
    for (j = f+1; j < 7; j++)
    {
        mask |= (1ULL << r*8 + j);
    }
    for (j = f-1; j > 0; j--)
    {
        mask |= (1ULL << r*8 + j);
    }
    return mask;
}

U64 get_rook_mask(int index){
    return rookMasks[index];
}

U64 rook_attack(int square_index, U64 occupancy){
    U64 north = 0ULL;
    U64 south = 0ULL;
    U64 west = 0ULL;
    U64 east = 0ULL;
    occupancy &= get_rook_mask(square_index);
    int count = square_index;
    int rank = square_index / 8; 
    int file = square_index % 8;
    //Starting with north, we calculate the path that a rook can take till its first encountered piece in the board
    for (rank += 1, count += 8; rank <= 7; rank++, count+= 8)
    {
        north |= 1ULL << count;
        //Break if attack ray meets piece on the board
        if (get_bit(occupancy, count) & get_bit(north,count)) break;
    }
    //Resetting the indexing variables for further use in other rays
    count = square_index;
    rank = square_index / 8; 
    file = square_index % 8;

    //calculating south ray
    for ( count -= 8, rank--; rank >= 0; rank--, count -= 8)
    {
        south |= 1ULL << count;
        //Break if attack ray meets piece on the board
        if (get_bit(occupancy, count) & get_bit(south,count)) break;
    }
    
    count = square_index;
    rank = square_index / 8; 
    file = square_index % 8;

    //calculating west ray
    for ( count += 1, file+= 1; file <= 7; file++, count++)
    {
        west |= 1ULL << count;
        //Break if attack ray meets piece on the board
        if (get_bit(occupancy, count) & get_bit(west,count)) break;
    }

    count = square_index;
    rank = square_index / 8; 
    file = square_index % 8;

    //calculating east ray
    for ( count -= 1, file-= 1; file >= 0; file--, count--)
    {
        west |= 1ULL << count;
        //Break if attack ray meets piece on the board
        if (get_bit(occupancy, count) & get_bit(west,count)) break;
        
    }

    return north | west | east | south;
}

U64 rook_magic_attack(int square_index, U64 occupancy){
    U64 occupied = occupancy;
    occupied &= magic_rook[square_index].mask;
    occupied *= magic_rook[square_index].magic;
    (int)(occupied >>= (64 - (magic_rook[square_index].bits)));    
    return rook_magic_attacks[square_index][occupied];
}


//This is the function that returns queen attacks
U64 queen_magic_attack(int square_index, U64 occupancy){
    return rook_magic_attack(square_index, occupancy) | bishop_magic_attack(square_index, occupancy);
}

// Magic bitboard for sliding pieces
void init_magic_attacks(U64 rook_magics[], U64 bishop_magics[]){
    //Initialising the attack tables for rook and bishop
    memset(bishop_magic_attacks, 0ULL, sizeof(bishop_magic_attacks));
    memset(rook_magic_attacks, 0ULL, sizeof(rook_magic_attacks));
    for (size_t i = 0; i < BOARDS_SQUARES; i++)
    {
        //Setup bishop magic-attacks
        U64 bishop_magic = bishop_magics[i];
        for(int j =0; j < (1 << bishop_bits[i]); j++){
            U64 occupancy = set_occupancy(j, get_bishop_mask(i), bishop_bits[i]);
            U64 attack = bishop_attack(i, occupancy);
            int index = multiply_magic(occupancy, bishop_magic, bishop_bits[i]);
            bishop_magic_attacks[i][index] = attack;
        }

        U64 rook_magic = rook_magics[i];
        for(int j =0; j < (1 << rook_bits[i]); j++){
            U64 occupancy = set_occupancy(j, get_rook_mask(i), rook_bits[i]);
            U64 attack = rook_attack(i, occupancy);
            int index = multiply_magic(occupancy, rook_magic, rook_bits[i]);
            rook_magic_attacks[i][index] = attack;
        }
    }
    //Storing all squares as a data structure of MagicSq 
    memset(magic_bishop, 0ULL, sizeof(magic_bishop));
    memset(magic_rook, 0ULL, sizeof(magic_rook));
    //Setting up all  rook squares as magics
    for (size_t i = 0; i < BOARDS_SQUARES; i++)
    {
        //Set up all rook squares
        magic_rook[i].magic =  rook_magics[i];
        magic_rook[i].mask = get_rook_mask(i);
        magic_rook[i].bits = rook_bits[i];
        //Set upp bishop squares
        magic_bishop[i].magic = bishop_magics[i];
        magic_bishop[i].mask = get_bishop_mask(i);
        magic_bishop[i].bits = bishop_bits[i];
    }
    
}

/*U64 get_sliding_attack(Square sq, U64 occupancy, U64 magic, int rook){
    U64 temp = occupancy & (rook ? get_rook_mask(sq) : get_bishop_mask(sq));
    int index = multiply_magic(temp, magic, (rook ? rook_bits[sq] : bishop_bits[sq]));
    printf("\n attack index = %d\n", index);
    return (rook ? rook_magic_attacks[sq][index] : bishop_magic_attacks[sq][index]);
}*/

//Functions that need re-considering in terms of where they should actually be
void print_chessboard(){
    HANDLE cons = GetStdHandle(STD_OUTPUT_HANDLE);
    printf("\n");
    for (int rank = 7; rank >= 0; rank--) {  // Iterate over ranks (rows)
        printf(" %d  ", rank + 1);
        for (int file = 7; file >= 0; file--) {  // Iterate over files (columns)
            int square = rank * 8 + file;  // Calculate square index
            U64 mask = 1ULL << square;     // Create mask for the square
            int empty = 1;                  //A variable to keep track of empty squares, assume square is empty
            for (int i = 0; i < 12; i++)
            {
                if (mask & get_bit(bitboard_pieces[i], square))
                {
                    DWORD n;
                    //wchar_t p[] = L"Queen: \u265B.\n";
                    //WriteConsoleW(cons, p, wcslen(p), &n, NULL );
                    WriteConsoleW(cons, unicode_pieces[i], wcslen(unicode_pieces[i]), &n, NULL);
                    //printf(" %c ", ascii_pieces[i]);
                    //wprintf(L" %lc ", unicode_pieces[i]);
                    empty = 0;
                }
            }
            if(empty){
                printf(" . ");             // Mark empty square
            }
        }
        printf("\n"); // Newline after each rank
    }
    printf("\n");
    printf("     a  b  c  d  e  f  g  h  \n");
    printf("    ------------------------");
    printf("\n");

}

void print_pieces(){
    for(int i = 0; i < 12; i++){
        print_matrix(bitboard_pieces[i]);
    }
}
const char* square_to_coordinates[64] = {
    "h1" , "g1", "f1", "e1", "d1", "c1", "b1", "a1",
    "h2" , "g2", "f2", "e2", "d2", "c2", "b2", "a2",
    "h3" , "g3", "f3", "e3", "d3", "c3", "b3", "a3",
    "h4" , "g4", "f4", "e4", "d4", "c4", "b4", "a4",
    "h5" , "g5", "f5", "e5", "d5", "c5", "b5", "a5",
    "h6" , "g6", "f6", "e6", "d6", "c6", "b6", "a6",
    "h7" , "g7", "f7", "e7", "d7", "c7", "b7", "a7",
    "h8" , "g8", "f8", "e8", "d8", "c8", "b8", "a8"
};


//Note: the colours of the pieces have been switched because of the black backround of the console they dont appear properly hence the colours had to be switched
wchar_t* unicode_pieces[12] = {L" ♚ ", L" ♛ ", L" ♝ ", L" ♞ ", L" ♜ ", L" ♟︎ ", L" ♔ ", L" ♕ ", L" ♗ ", L" ♘ ", L" ♖ ", L" ♙ "};
/*wchar_t* unicode_pieces[12] = {
    L" \u2654 ", //White King
    L" \u2655 ", //White queen
    L" \u2657 ", //White bishop
    L" \u2658 ", //White knight
    L" \u2656 ", //White rook
    L" \u2659 ", //White pawn
    L" \u265A ", //Black King
    L" \u265B ", //Black Queen
    L" \u265D ", //Black Bishop
    L" \u265A ", //Black Knight
    L" \u265C ", //Black rook
    L" \u265F " //Black pawn
};*/

char ascii_pieces[12] = "KQBNRPkqbnrp";
int char_pieces[] = {
    ['K'] = K,
    ['Q'] = Q,
    ['B'] = B,
    ['N'] = N,
    ['R'] = R,
    ['P'] = P,
    ['k'] = k,
    ['q'] = q,
    ['b'] = b,
    ['n'] = n,
    ['r'] = r,
    ['p'] = p
};

//An extensive FEN parser function
void fen_parser(const char* fen){
    memset(bitboard_pieces, 0, sizeof(bitboard_pieces)); // Clear bitboards
    enpessant = 0;
    half_moves = 0;
    full_moves = 0;
    const char* ptr = fen;
    int rank = 7, file = 0;

    // Parse piece placement
    while (*ptr && *ptr != ' ') {
        char c = *ptr;
        if (c == '/') { // Move to the next rank
            rank--;
            file = 0;
        } else if (c >= '1' && c <= '8') { // Empty squares
            file += (c - '0');
        } else { // Piece
            int square = rank*8 + (7-file);
            //printf(" %c, rank: %d , file: %d , square: %d \n", c, rank, file, square);  line used for debugging
            bitboard_pieces[char_pieces[c]] |= 1ULL << square;
            file++;
        }
        ptr++;
    }
    

    // Skip past piece placement
    while (*ptr && *ptr == ' ') {
        ptr++;
    }

    //Evaluate who's turn it is to play
    if(*ptr && *ptr != ' '){
        if(*ptr == 'b'){
            side = black;
        }else if (*ptr == 'w')
        {
            side = white;
        }
        ptr++;
    }

    // Skip past side to move
    while (*ptr && *ptr == ' ') {
        ptr++;
    }
    //Evaluate castling rights

    //A documentation of the castle variable enum is extensively explained in the types.h file
    while (*ptr && *ptr != ' ') {
        switch (*ptr)
        {
        case 'K':
            castle |= wk;
            break;
        case 'Q':
            castle |= wq;
            break;
        case 'k':
            castle |= bk;
            break;
        case 'q':
            castle |= bq;
            break;
        default:
            castle = 0;
            break;
        }
        ptr++;
    }

    //Skip past the castling and move to the enpessant
    while (*ptr && *ptr == ' ') {
        ptr++;
    }

    char c1[3] = "";
    int c1_index = 0;
    while(*ptr && *ptr != ' '){
        if (*ptr == '-')
        {
            enpessant = no_sq;
        }else if (c1_index < 2) { // Ensure c1 doesn't overflow
            c1[c1_index++] = *ptr; // Append the character
        }else{
            // Handle buffer overflow (optional)
            fprintf(stderr, "Buffer overflow detected in c1!\n");
            break;
        }
        ptr++;
    }
    c1[c1_index] = '\0'; // Null-terminate the string
    
    //Simply break down the square from string coordinate into an interger coordinate by normalising the characters into a range we can exploit 
    enpessant = ((int)(c1[0] - 'a') + (int)(c1[1] - '0')*8) - 1;

    //Skip past the enpessant
    while (*ptr && *ptr == ' ') {
        ptr++;
    }

    //Evaluate the halfmoves
    half_moves = 0;
    while (*ptr && *ptr != ' ') {
        char c = *ptr;
        if (c >= '0' && c <= '9') {
            //(c - '0') we use this to normalise the characters into a 0 - 9 range so we can append 
            half_moves = half_moves * 10 + (c - '0'); //normalise the ascii int to be in between 0 and 9;
        }
        ptr++;
    }

    //Skip past the half moves
    while (*ptr && *ptr == ' ') {
        ptr++;
    }

    //evaluate full moves
    full_moves = 0;
    while (*ptr && *ptr != ' ') {
        char c = *ptr;
        if (c >= '0' && c <= '9') {
            //(c - '0') we use this to normalise the characters into a 0 - 9 range so we can append 
            full_moves = full_moves * 10 + (c - '0');
        }
        ptr++;
    }
    updateGame();
}

/*******************\
 * Move Generation *
\*******************/ 
bool is_square_attacked(int square, Colour side, U64 occupancy){
    //Attacked by pawns 
    U64 pawns = (side == white)? bitboard_pieces[P] : bitboard_pieces[p];
    printf("Attacked pawn ");
    if(get_pawn_attack((side ^ 1), square) & pawns) return true;
    // Attacked by knights 
    U64 knights = (side == white)? bitboard_pieces[N] : bitboard_pieces[n];
    printf("Attacked knight ");
    if(get_knight_attack(square) & knights) return true;

    //Attacked by kings
    U64 king = (side == white)? bitboard_pieces[K] : bitboard_pieces[k];
    printf("Attacked king ");
    if(king_attacks[square] & king) return true;

    //Attacked by bishops
    U64 bishopQueens = (side == white)? (bitboard_pieces[Q] | bitboard_pieces[B]): (bitboard_pieces[q] | bitboard_pieces[b]);
    printf("Attacked bishop ");
    if(bishop_magic_attack(square, occupancy) & bishopQueens) return true;

    //Attacked by rooks
    U64 rookQueens = (side == white)? (bitboard_pieces[Q] | bitboard_pieces[R]): (bitboard_pieces[q] | bitboard_pieces[r]);
    printf("Attacked rook ");
    if(rook_magic_attack(square, occupancy) & rookQueens) return true;

    return false;
}


void init_in_between(){
    for(Square s1 = h1; s1 <= a8; s1++) {
        //Calculating the in between square for bishops
        SL_Piece piece[2] = {bishop, rook};
        for (Square s2 = h1; s2 <= a8; s2++) {
            arr_rectangular[s1][s2] = inBetween(s2,s1); 
        }
    }
}

// Thanks to Gerd Isenberg for the logic: https://www.talkchess.com/forum3/viewtopic.php?f=7&t=12499&start=14

//This is a function that can quicly calculate the in between squares of 2 squares from and to
U64 inBetween(int sq1, int sq2)
{
    int delta = sq2 - sq1;
    sq1 += delta & (delta >> 31); // min
    sq2 -= delta & (delta >> 31); // max

    uint32_t rankDiff, fileDiff, antiDiff, diaxDiff;
    rankDiff  = (sq2 >>3) - (sq1 >>3);
    fileDiff  = (sq2 & 7) - (sq1 & 7);
    antiDiff  = (rankDiff + fileDiff);
    fileDiff &= 15;
    antiDiff &= 15;
    diaxDiff  = (rankDiff ^ fileDiff);

    U64 between;
    between   =     (rankDiff-1 & 0x000007E0)  >>  4;
    between  += (U64)fileDiff-1 & 0x0001010101010100;
    between  += (U64)diaxDiff-1 & 0x0040201008040200;
    between  += (U64)antiDiff-1 & 0x0000040810204080;
    between <<= sq1;
    between  &=(1ULL << sq2) - 1;
    return between;
}

U64 get_in_between(int from, int to){
    return arr_rectangular[from][to];
}

/* PINS */

//Below are the different types of PINS, and these pins are distinguised by the "Blocker" variable you pass into the x-ray attacks 
/*
Motiv 	            |   Blocker 	             |   Target behind Blocker
-------------------------------------------------------------------------------
Battery 	        |   own sliding piece 	     |   any square or piece
Discovered Attack   | 	own piece may remove 	 |   any opposing piece
Discovered Check    |	own piece may remove 	 |   opposing king
Pin 	            |   opposing piece 	         |   opposing valuable piece
Absolute Pin 	    |   opposing piece 	         |   opposing king
Partial Pin 	    |   opposing sliding piece 	 |   opposing valuable piece
Skewer 	            |   opposing valuable piece  |   opposing piece
X-ray 	            |   opposing sliding piece 	 |   own piece
-------------------------------------------------------------------------------
*/

// X-ray attacks
U64 xray_rook(U64 blockers, U64 occupied, Square rookSq){
    //Get the normal rook attack to the first blocker in the ray
    U64 attacks = rook_magic_attack(rookSq, occupied);
    //Isolate only the blockers that the rook has encountered
    blockers &= attacks;
    //Remove those blockers from the original occupancy to reveal the pieces behind the first blockers
    //Then return all the pieces that the rook can access behind the first blocker pieces  on the ray
    return attacks ^ rook_magic_attack(rookSq, occupied ^ blockers);
}

U64 xray_bishop(U64 blockers, U64 occupied, Square bishopSq){   
    //Get the normal bishop attack to the first blocker in the ray
    U64 attacks = bishop_magic_attack(bishopSq, occupied);
    //Isolate only the blockers that the bishop has encountered
    blockers &= attacks;
    //Remove those blockers from the original occupancy to reveal the pieces behind the first blockers
    //Then return all the pieces that the bishop can access behind the first blocker pieces  on the ray
    return attacks ^ bishop_magic_attack(bishopSq, occupied ^ blockers);
}

/*
    binary move bits                               hexidecimal constants
    
    0000 0000 0000 0000 0011 1111    source square       0x3f       - source square of piece
    0000 0000 0000 1111 1100 0000    target square       0xfc0      - Piece target square of piece
    0000 0000 1111 0000 0000 0000    piece               0xf000     - Piece moved
    0000 1111 0000 0000 0000 0000    Flags               0xf0000    - Piece move types
    0111 0000 0000 0000 0000 0000    Piece capture       0x700000  - Pieces to be captured
*/

/*
    Pieces to be captured
    ---------------------------------------
    0000    - NO piece capture
    0001    - Queen capture
    0010    - Bishop capture
    0011    - Knight capture
    0100    - Rook capture
    0101    - Pawn capture

*/

/*
        /***********FLAGS and Their Binary Representation************\

    code |	promotion |	capture  |  special 1 | special 0 |	kind of move
    ---------------------------------------------------------------------------------
    0 	    0 	        0 	        0 	        0 	        quiet moves
    1 	    0 	        0 	        0 	        1 	        double pawn push
    2 	    0 	        0 	        1 	        0 	        king castle
    3 	    0 	        0 	        1 	        1 	        queen castle
    4 	    0 	        1 	        0 	        0 	        captures
    5 	    0 	        1 	        0 	        1 	        enpassant-capture
    8 	    1 	        0 	        0 	        0 	        knight-promotion
    9 	    1 	        0 	        0 	        1 	        bishop-promotion
    10 	    1 	        0 	        1 	        0 	        rook-promotion
    11 	    1 	        0 	        1 	        1 	        queen-promotion
    12 	    1 	        1 	        0 	        0 	        knight-promo capture
    13 	    1 	        1 	        0 	        1 	        bishop-promo capture
    14 	    1 	        1 	        1 	        0 	        rook-promo capture
    15 	    1 	        1 	        1 	        1 	        queen-promo capture

*/

//note the flags is a 4-bit necessary int from 0-15 
#define encode(source, target, piece, flags, captured_piece ) ( \
((source) & 0x3f) |                     \
( ((target) << 6 ) & 0xfc0) |           \
(((piece) << 12) & 0xf000)  |           \
(((flags) << 16) & 0xf0000) |           \
(((captured_piece) << 20) & 0x700000)   \
)

#define get_source(move)( (move) & 0x3f )
#define get_target(move)( ((move) & 0xfc0) >> 6 )
#define get_piece(move) ( ((move) & 0xf000) >> 12 )
#define get_flags(move) ( ((move) & 0xf0000) >> 16 )
#define get_captured_piece(move) ( ((move) & 0x700000) >> 20 )
#define reset_lsb(bitboard) ((bitboard) &= (bitboard) - 1)
#define get_pawn(side) ( bitboard_pieces[pieces_by_color[side][5]] )
#define get_rook(side) ( bitboard_pieces[pieces_by_color[side][4]] )
#define get_knight(side) ( bitboard_pieces[pieces_by_color[side][3]] )
#define get_bishop(side) ( bitboard_pieces[pieces_by_color[side][2]] )
#define get_queen(side) ( bitboard_pieces[pieces_by_color[side][1]] )
#define get_king(side) ( bitboard_pieces[pieces_by_color[side][0]] )

void absolute_pins(Pinned_pieces *pinned_pieces,Square kingSq, U64* blockers, U64 occupied,U64 opponent_queens, U64 opponent_bishops, U64 opponent_rooks, Pieces blocker_piece, bool is_pawn){
    //queens
    while (opponent_queens)
    {
        Square queenSq = debruijn_BitScan(opponent_queens);
        if(is_pawn && enpessant != no_sq){
            occupied ^= get_bit(occupied, enpessant);
        }
        U64 mask = ((1 << kingSq) & (xray_bishop(*blockers, occupied, queenSq) | xray_rook(*blockers, occupied, queenSq))) ? get_in_between(kingSq, queenSq) : 0ULL;
        
        U64 pinned_square = *blockers & mask;
        // Ensure there is exactly one piece between the king and the queen
        if (pinned_square && (pinned_square & (pinned_square - 1)) == 0) {
            *blockers ^= pinned_square;
            Pinned a;
            a.pinned_square = debruijn_BitScan(pinned_square);
            a.piece_push_mask = mask;
            a.piece_capture_mask = 1ULL << queenSq;
            a.piece = blocker_piece;
            pinned_pieces->pieces[pinned_pieces->count++] = a;
        }
        reset_lsb(opponent_queens);
    }
    //rooks
    while (opponent_rooks)
    {
        Square rookSq = debruijn_BitScan(opponent_rooks);
        if(is_pawn && enpessant != no_sq){
            occupied ^= get_bit(occupied, enpessant);
        }
        U64 mask = ((1 << kingSq) & xray_rook(*blockers, occupied, rookSq)) ? get_in_between(kingSq, rookSq) : 0ULL;
        U64 pinned_square = *blockers & mask;
        // Ensure there is exactly one piece between the king and the queen
        if (pinned_square && (pinned_square & (pinned_square - 1)) == 0) {
            *blockers ^= pinned_square; 
            Pinned a;
            a.pinned_square = debruijn_BitScan(pinned_square);
            a.piece_push_mask = mask;
            a.piece_capture_mask = 1ULL << rookSq;
            a.piece = blocker_piece;
            pinned_pieces->pieces[pinned_pieces->count++] = a;
        }
        reset_lsb(opponent_rooks);
    }
    //bishops
    while (opponent_bishops)
    {
        Square bishopSq = debruijn_BitScan(opponent_bishops);
        U64 mask = ((1 << kingSq) & xray_bishop(*blockers, occupied, bishopSq)) ? get_in_between(kingSq, bishopSq) : 0ULL;
        U64 pinned_square = *blockers & mask;
        // Ensure there is exactly one piece between the king and the queen
        if (pinned_square && (pinned_square & (pinned_square - 1)) == 0) {
            *blockers ^= pinned_square;
            Pinned a;
            a.pinned_square = debruijn_BitScan(pinned_square);
            a.piece_push_mask = mask;
            a.piece_capture_mask = 1ULL << bishopSq;
            a.piece = blocker_piece;
            pinned_pieces->pieces[pinned_pieces->count++] = a;
        }
        reset_lsb(opponent_bishops);
    }
}

// Generate is a move generator function
void generate(Colour side, Moves *movelist){
    U64 opponent_piece_attacks[6]; // Array to store attacks by each type of opponent piece
    //These arrays are used to keep tabs on positions of opponent pieces, the last index is used for numer of pieces
    //int queens[11]; // maximum number of queens in a gamre per player is 9
   // int rooks[11];  // maximum number of rooks in a gamre per player is 10
    //int bishops[11]; // maximum number of bishops in a gamre per player is 10
    unsigned int from_square, to_square;

    Colour opponent_side = side ^ 1;

    U64 current_bitboard, all_opponent_attacks, occupied_without_king; 
    int no_checks = 0; // A variable to control the number of checks generated
    //Remove king from the occupied bitboard
    occupied_without_king = get_game() ^ get_king(side); //The king is the first bitboard
    //initialise the opponents piece attacks to 0
    all_opponent_attacks = 0ULL;
    //Generate all attacks from the opponent
    printf("generating all opponent attacks \n");
    for (Pieces i = pieces_by_color[opponent_side][0]; i <= pieces_by_color[opponent_side][5]; i++)
    {
        printf("generating attacks for piece %d \n", i);
        U64 temp = 0;
        if(i == pieces_by_color[opponent_side][5]){
            //Generate white pawn attacks
            temp = bitboard_pieces[i];
            temp = (i == P) ? (northEastOne(temp) | northWestOne(temp)) : (southEastOne(temp) | southWestOne(temp));
            opponent_piece_attacks[5] = temp;
        }
        else if(i == pieces_by_color[opponent_side][3]){
            //Generate knight attacks
            temp = bitboard_pieces[i];
            U64 knights = 0;
            while (temp)
            {
                int source = debruijn_BitScan(temp);
                knights |= get_knight_attack(source);
                reset_lsb(temp);
            }
            temp = knights;
            opponent_piece_attacks[3] = temp;
        }
        else if(i == pieces_by_color[opponent_side][2]){
            //Generate bishop attacks
            temp = bitboard_pieces[i];
            U64 bishops = 0;
            while (temp)
            {
                bishops |= bishop_magic_attack(debruijn_BitScan(temp), occupied_without_king);
                reset_lsb(temp);
            }
            temp = bishops;
            opponent_piece_attacks[2] = temp;
        }
        else if(i == pieces_by_color[opponent_side][4]){
            //Generate rook attacks
            temp = bitboard_pieces[i];
            U64 rooks = 0;
            while (temp)
            {
                rooks |= rook_magic_attack(debruijn_BitScan(temp), occupied_without_king);
                reset_lsb(temp);
            }
            temp = rooks;
            opponent_piece_attacks[4] = temp;
        }
        else if(i == pieces_by_color[opponent_side][1]){
            //Generate queen attacks
            temp = bitboard_pieces[i];
            U64 queens = 0;
            while (temp)
            {
                queens |= queen_magic_attack(debruijn_BitScan(temp), occupied_without_king);
                reset_lsb(temp);
            }
            temp = queens;
            opponent_piece_attacks[1] = temp;
        }else if(i == pieces_by_color[opponent_side][0]){
            //Generate king attacks
            temp = bitboard_pieces[i];
            temp = king_attacks[debruijn_BitScan(temp)];
            //temp &= create_empty_squares(occupied_without_king); //Make sure the king attacks are legal and quiet
            opponent_piece_attacks[0] = temp;
        }
        all_opponent_attacks |= temp;
    }
    printf("DONE Generating all opponent attacks \n");
    //initialise king
    unsigned short who_is_checking = 0; // A variable to keep track of pieces checking the king

    /*
        Pieces that are checking the king
        ---------------------------------------
        0000 0000    - NO piece checking    = 0
        0000 0001    - Queen checking       = 1
        0000 0010    - Bishop checking      = 2
        0000 0100    - Knight checking      = 4
        0000 1000    - Rook checking        = 8
        0001 0000    - Pawn checking        = 16
    */


    from_square = debruijn_BitScan(get_king(side));
    int king_square = debruijn_BitScan(get_king(side));

    U64 push_mask = create_empty_squares(0ULL); //Push anywhere
    U64 capture_mask = create_empty_squares(0ULL); // Capture anything/anywhere
    U64 enpessant_check_mask = 0ULL; //No enpessant captures can be allowed

    // sliding piece checks:
    //  - qeeen check
    printf("Genereating qeeen check\n");
    if(opponent_piece_attacks[1] & get_king(side)){
        who_is_checking |= 1;
        //Search for the actual queen producing the check
        U64 queen = get_queen(opponent_side);
        while (queen){
            int destination = debruijn_BitScan(queen);
            //Check if found queen is the checker
            if(queen_magic_attack(destination, get_game()) & get_king(side)){
                push_mask = get_in_between(from_square, destination);
                capture_mask = 1ULL << destination;
            }
            reset_lsb(queen);
        }
        no_checks++;
    }
    //  - bishop check
    printf("Genereating bishop check\n");
    if(opponent_piece_attacks[2] & get_king(side)){
        who_is_checking |= 2;
        //Search for the actual bishop producing the check
        U64 bishop = get_bishop(opponent_side);
        while (bishop){
            int destination = debruijn_BitScan(bishop);
            //Check if found bishop is the checker
            if(bishop_magic_attack(destination, get_game()) & get_king(side)){
                push_mask = get_in_between(from_square, destination);
                capture_mask = 1ULL << destination;
            }
            reset_lsb(bishop);
        }
        no_checks++;
    }
    //  - Rook check
    printf("Genereating rook check\n");
    if (opponent_piece_attacks[4] & get_king(side))
    {
        who_is_checking |= 8;
        //Search for the actual rook producing the check
        U64 rook = get_rook(opponent_side);
        while (rook)
        {
            int destination = debruijn_BitScan(rook);
            //Check if found queen is the checker
            if(rook_magic_attack(destination, get_game()) & get_king(side)){
                push_mask = get_in_between(from_square, destination);
                capture_mask = 1ULL << destination;
            } 
            reset_lsb(rook);
        }
        no_checks++;
    }
    // Knight piece checks:
    printf("Genereating knight check\n");
    if(opponent_piece_attacks[3] & get_king(side)){
        who_is_checking |= 4;
        //Search for the actual knight producing the check
        U64 knight = get_knight(opponent_side);
        while (knight)
        {
            int destination = debruijn_BitScan(knight);
            //Check if found knight is the checker
            if(get_knight_attack(destination) & get_king(side)){
                push_mask = 0ULL; //Cant block a knight check
                capture_mask = 1ULL << destination;
            }
            reset_lsb(knight);
        }
        no_checks++;
    }

    // pawn piece checks:
    printf("Genereating pawn check\n");
    if (opponent_piece_attacks[5] & get_king(side))
    {
        who_is_checking |= 16;
        //Evaluate if it is an enpessant check
        enpessant_check_mask = (get_king_target(from_square, (1ULL << enpessant)))? (1ULL << enpessant): 0ULL;
        push_mask = 0;
        U64 opponent_pawns = get_pawn(opponent_side);
        capture_mask =(side == white)? (get_pawn_attack(white,from_square) & opponent_pawns) : (get_pawn_attack(black,from_square) & opponent_pawns);
        no_checks++;
    }
    printf("generate king quite moves\n"); 
    //generate king quite moves
    U64 king_quiet_moves = get_king_target(from_square, create_empty_squares(all_opponent_attacks | get_game()));
    print_matrix(create_empty_squares(all_opponent_attacks | get_game()));
    print_matrix( king_attacks[from_square] );
    print_matrix(king_quiet_moves);
    (king_quiet_moves) ? encode_quiet_moves(movelist,pieces_by_color[side][0],king_quiet_moves, from_square, side): (void)0;
    //generate King captures, which may capture checking piece
    U64 protected_pieces = get_side_bitboard(opponent_side) & all_opponent_attacks; // Check if the pieces to capture are protected
    U64 king_capture_moves = get_side_bitboard(opponent_side) ^ protected_pieces; // Remove all the protected pieces from the king potential attack
    //king_capture_moves = king_capture_moves & get_king_target(from_square, create_empty_squares(get_side_bitboard(side))); //Remove king allies(pieces) from the capture moves
    king_capture_moves = get_king_target(from_square, king_capture_moves);
    encode_captures(movelist, pieces_by_color[side][0], king_capture_moves, from_square, side);

    if(no_checks > 1){
        printf("The king is in check \n"); 
        return; 
    }

    //generate castling moves
    printf("Generate castling moves \n");
    if(no_checks == 0){
        if (side == white)
        {
            ((castle & wk) && !(get_in_between(e1, h1) & (get_game() | all_opponent_attacks))) ? add_move(movelist, encode(e1, g1, pieces_by_color[side][0], 2, 0)): (void)0;
            ((castle & wq) && !(get_in_between(e1, a1) & (get_game())) && !(get_in_between(b1, e1) & all_opponent_attacks)) ? add_move(movelist, encode(e1, c1, pieces_by_color[side][0], 3, 0)): (void)0;
        }
        else
        {
            ((castle & bk) && !(get_in_between(e8, h8) & (get_game() | all_opponent_attacks))) ? add_move(movelist, encode(e1, g1, pieces_by_color[side][0], 2, 0)): (void)0;
            ((castle & bq) && !(get_in_between(e8, a8) & (get_game())) && !(get_in_between(b8, e8) & all_opponent_attacks)) ? add_move(movelist, encode(e8, c8, pieces_by_color[side][0], 3, 0)): (void)0;

        }
    }
    // generate all pawn moves
    U64 pawns = get_pawn(side);
    Pinned_pieces pinned_pawns;
    init_pinned_pieces(&pinned_pawns);
    printf("Generating absolute pins \n");
    absolute_pins(&pinned_pawns, king_square, &pawns, get_game(), get_queen(opponent_side), get_bishop(opponent_side), get_rook(opponent_side), pieces_by_color[side][5], true);
    // generate quiet pawn pushes
    U64 single_push = (side == white)? wSinglePush(pawns, create_empty_squares(get_game())) : bSinglePush(pawns, create_empty_squares(get_game()));
    U64 double_push = (side == white)? wDoublePush(pawns, create_empty_squares(get_game())) : bDoublePush(pawns, create_empty_squares(get_game()));
    //genereate double pushes
    printf("Generating double Push pawn \n");
    while (double_push)
    {
        to_square = debruijn_BitScan(double_push) & push_mask; //Only filter out legal moves
        from_square = (side == white)? to_square - 16 : to_square + 16;
        add_move(movelist, encode(from_square, to_square, pieces_by_color[side][5] , 1, 0));
        reset_lsb(double_push);
    }
    
    //generate single pushes
    printf("Generating single Push pawn \n");
    while (single_push)
    {
        to_square = debruijn_BitScan(single_push) & push_mask; //Only filter out legal moves
        int rank = to_square/8;
        from_square = (side == white)? to_square - 8 : to_square + 8;
        if((side == white) && (rank == 7) || (side == black) && (rank == 0)){
            //Promotions
            add_move(movelist, encode(from_square, to_square, pieces_by_color[side][5] , 8, 0));
            add_move(movelist, encode(from_square, to_square, pieces_by_color[side][5] , 9, 0));
            add_move(movelist, encode(from_square, to_square, pieces_by_color[side][5] , 10, 0));
            add_move(movelist, encode(from_square, to_square, pieces_by_color[side][5] , 11, 0));
        }else{
            add_move(movelist, encode(from_square, to_square, pieces_by_color[side][5], 0, 0)); //quiet move
        }
        reset_lsb(single_push);
    }
    
    //generate pawn attacks
    printf("Generating pawn attacks \n");
    while (pawns)
    {
        from_square = debruijn_BitScan(pawns);
        U64 attacks = get_pawn_attack(side, from_square) & capture_mask;
        attacks ^= get_side_bitboard(side);
        printf("Withing encode captures");
        encode_captures(movelist, pieces_by_color[side][5], attacks, from_square, side);
        printf("After encode captures");
        reset_lsb(pawns);
    }
    
    printf(" \n DONE Generating pawn ATTACKS \n");

    //generate pinned pawns
    generate_pinned_pawn_moves(side,movelist,&pinned_pawns,push_mask,capture_mask);
    
    //generate knight attacks
    U64 knights = get_knight(side);
    Pinned_pieces pinned_knights;
    init_pinned_pieces(&pinned_knights);
    
    //remove pinned knight but dont generate attacks for it since a pinned knight cant move in the direction of the pinner
    absolute_pins(&pinned_knights, king_square, &knights, get_game(), get_queen(opponent_side), get_bishop(opponent_side), get_rook(opponent_side), pieces_by_color[side][3], false);
    
    printf("Generating knight attacks \n");
    while (knights)
    {
        from_square = debruijn_BitScan(knights);
        U64 attacks = get_knight_attack(from_square) & capture_mask;
        U64 quiets = (attacks & create_empty_squares(get_game())) & push_mask;
        (quiets) ? encode_quiet_moves(movelist, pieces_by_color[side][3], quiets, from_square, side): (void)0 ;
        attacks &= (get_side_bitboard(opponent_side) & capture_mask);
        (attacks) ? encode_captures(movelist, pieces_by_color[side][3], attacks, from_square, side): (void)0 ;
        reset_lsb(knights);
    }
    
    
    //generate bishop attacks
    U64 bishops = get_bishop(side);
    Pinned_pieces pinned_bishops;
    init_pinned_pieces(&pinned_bishops);
    absolute_pins(&pinned_bishops, king_square, &bishops, get_game(), get_queen(opponent_side), get_bishop(opponent_side), get_rook(opponent_side), pieces_by_color[side][2], false);
    printf("Generating bishop attacks \n");
    while(bishops){
        from_square = debruijn_BitScan(bishops);
        U64 attacks = bishop_magic_attack(from_square, get_game());
        U64 quiets = (attacks & create_empty_squares(get_game())) & push_mask;
        (quiets) ? encode_quiet_moves(movelist, pieces_by_color[side][2], quiets, from_square, side): (void)0 ;
        attacks &= (get_side_bitboard(opponent_side) & capture_mask);
        (attacks) ? encode_captures(movelist, pieces_by_color[side][2], attacks, from_square, side): (void)0 ;
        reset_lsb(bishops);
    }
    
    //generate attacks for bishop pinned pieces
    for(int i = 0; i < pinned_bishops.count; i++){
        from_square = pinned_bishops.pieces[i].pinned_square;
        U64 attacks = bishop_magic_attack(from_square, get_game());
        U64 quiets = (attacks & create_empty_squares(get_game())) & push_mask & pinned_bishops.pieces[i].piece_push_mask;
        (quiets) ? encode_quiet_moves(movelist, pieces_by_color[side][2], quiets, from_square, side): (void)0 ;
        attacks &= (get_side_bitboard(opponent_side) & capture_mask & pinned_bishops.pieces[i].piece_capture_mask);
        (attacks) ? encode_captures(movelist, pieces_by_color[side][2], attacks, from_square, side): (void)0 ;
    }
    
    //generate rook attacks
    U64 rooks = get_rook(side);
    Pinned_pieces pinned_rooks;
    init_pinned_pieces(&pinned_rooks);
    absolute_pins(&pinned_rooks, king_square, &rooks, get_game(), get_queen(opponent_side), get_bishop(opponent_side), get_rook(opponent_side), pieces_by_color[side][4], false);
    printf("Generating rook attacks \n");
    while (rooks)
    {
        from_square = debruijn_BitScan(rooks);
        U64 attacks = rook_magic_attack(from_square, get_game());
        U64 quiets = (attacks & create_empty_squares(get_game())) & push_mask;
        (quiets) ? encode_quiet_moves(movelist, pieces_by_color[side][4], quiets, from_square, side): (void)0 ;
        attacks &= (get_side_bitboard(opponent_side) & capture_mask);
        (attacks) ? encode_captures(movelist, pieces_by_color[side][4], attacks, from_square, side): (void)0 ;
        reset_lsb(rooks);
    }
    
    //generate attacks for rook pinned pieces
    for (int i = 0; i < pinned_rooks.count; i++)
    {
        from_square = pinned_rooks.pieces[i].pinned_square;
        U64 attacks = rook_magic_attack(from_square, get_game());
        U64 quiets = (attacks & create_empty_squares(get_game())) & push_mask & pinned_rooks.pieces[i].piece_push_mask;
        (quiets) ? encode_quiet_moves(movelist, pieces_by_color[side][4], quiets, from_square, side): (void)0 ;
        attacks &= (get_side_bitboard(opponent_side) & capture_mask & pinned_rooks.pieces[i].piece_capture_mask);
        (attacks) ? encode_captures(movelist, pieces_by_color[side][4], attacks, from_square, side): (void)0 ;
    }
    
    //generate queen attacks
    U64 queens = get_queen(side);
    Pinned_pieces pinned_queens;
    init_pinned_pieces(&pinned_queens);
    absolute_pins(&pinned_queens, king_square, &queens, get_game(), get_queen(opponent_side), get_bishop(opponent_side), get_rook(opponent_side), pieces_by_color[side][1], false);
    printf("Generating queen attacks \n");
    while (queens)
    {
        from_square = debruijn_BitScan(queens);
        U64 attacks = queen_magic_attack(from_square, get_game());
        print_matrix(attacks);
        U64 quiets = (attacks & create_empty_squares(get_game())) & push_mask;
        (quiets) ? encode_quiet_moves(movelist, pieces_by_color[side][1], quiets, from_square, side): (void)0 ;
        attacks &= (get_side_bitboard(opponent_side) & capture_mask);
        
        (attacks) ? encode_captures(movelist, pieces_by_color[side][1], attacks, from_square, side): (void)0 ;
        reset_lsb(queens);
    }
    //generate attacks for queen pinned pieces
    for (int i = 0; i < pinned_queens.count; i++)
    {
        from_square = pinned_queens.pieces[i].pinned_square;
        U64 attacks = queen_magic_attack(from_square, get_game());
        U64 quiets = (attacks & create_empty_squares(get_game())) & push_mask & pinned_queens.pieces[i].piece_push_mask;
        (quiets) ? encode_quiet_moves(movelist, pieces_by_color[side][1], quiets, from_square, side): (void)0 ;
        attacks &= (get_side_bitboard(opponent_side) & capture_mask & pinned_queens.pieces[i].piece_capture_mask);
        
        (attacks) ? encode_captures(movelist, pieces_by_color[side][1], attacks, from_square, side): (void)0 ;
    }

    /*
        check if capture
        - check if normal capture
        - check if enpassant capture
        - check if enpassant capture is a pin
        - example below: although black is at an enpassant capture it is pinned 
        
            8  .  .  .  .  .  .  .  .
            7  .  .  .  .  .  .  .  .
            6  .  .  .  .  .  .  .  .
            5  .  .  .  .  .  .  .  .
            4  ♔ .  .  ♟︎ ♙  .  . ♛
            3  .  .  .  .  .  .  .  .
            2  .  .  .  .  .  .  .  .
            1  .  .  .  ♚ .  .  .  .
               a  b  c  d  e  f  g  h
            
        
        check if pinned piece
        check for castling
        - cant castle if king is in check
        - check if castling ray is attacked
    */
   printf("\n DONE GENERATING MOVES \n");
}
void init_pinned_pieces(Pinned_pieces *pinned_pieces) {
    pinned_pieces->count = 0;
    memset(pinned_pieces->pieces, 0, sizeof(pinned_pieces->pieces));
}
void generate_pinned_pawn_moves(Colour side,Moves* movelist, Pinned_pieces* pinned_pawns,U64 push_mask,U64 capture_mask){
    printf("\n GENERATING PINNED PAWNS \n");
    for (int i = 0; i < pinned_pawns->count; i++) {
        printf("\n \t Checking PAWN %d \n", i);
        int from_square = pinned_pawns->pieces[i].pinned_square;
        U64 piece_push_mask = pinned_pawns->pieces[i].piece_push_mask;
        U64 piece_capture_mask = pinned_pawns->pieces[i].piece_capture_mask;

        // Generate quiet pawn pushes
        U64 single_push = (side == white) ? wSinglePush(1ULL << from_square, create_empty_squares(get_game())) : bSinglePush(1ULL << from_square, create_empty_squares(get_game()));
        single_push &= piece_push_mask & push_mask; // Filter out illegal moves
        while (single_push) {
            int to_square = debruijn_BitScan(single_push);
            int rank = to_square / 8;
            if ((side == white && rank == 7) || (side == black && rank == 0)) {
                // Promotions
                add_move(movelist, encode(from_square, to_square, pieces_by_color[side][5], 8, 0));
                add_move(movelist, encode(from_square, to_square, pieces_by_color[side][5], 9, 0));
                add_move(movelist, encode(from_square, to_square, pieces_by_color[side][5], 10, 0));
                add_move(movelist, encode(from_square, to_square, pieces_by_color[side][5], 11, 0));
            } else {
                add_move(movelist, encode(from_square, to_square, pieces_by_color[side][5], 0, 0)); // Quiet move
            }
            reset_lsb(single_push);
        }

        // Generate double pawn pushes
        U64 double_push = (side == white) ? wDoublePush(1ULL << from_square, create_empty_squares(get_game())) : bDoublePush(1ULL << from_square, create_empty_squares(get_game()));
        double_push &= piece_push_mask & push_mask; // Filter out illegal moves
        while (double_push) {
            int to_square = debruijn_BitScan(double_push);
            from_square = (side == white) ? to_square - 16 : to_square + 16;
            add_move(movelist, encode(from_square, to_square, pieces_by_color[side][5], 1, 0));
            reset_lsb(double_push);
        }

        // Generate pawn captures
        U64 attacks = get_pawn_attack(side, from_square) & piece_capture_mask & capture_mask;
        attacks ^= get_side_bitboard(side);
        encode_captures(movelist, pieces_by_color[side][5], attacks, from_square, side);
    }
}

void encode_quiet_moves(Moves *movelist, Pieces piece, U64 quiet_moves, int from_square, Colour side){
    while (quiet_moves)
    {
        int to_square = debruijn_BitScan(quiet_moves);
        add_move(movelist, encode(from_square, to_square, piece, 0, 0));
        reset_lsb(quiet_moves);
    }
}
//This function is used to check pieces we are capturing then encode the relevant information
void encode_captures(Moves *movelist, Pieces piece, U64 piece_attacks, int from_square, Colour side){
    Colour opponent_side = side ^ 1;
    printf(" \n \t ENCODING CAPTURES FOR %d \n", piece);
    while(piece_attacks){
        int to_square = debruijn_BitScan(piece_attacks);
        U64 attack = 1 << to_square;
        int flags = 4; //assign the capture flag
        //Attacking opponent queen
        if(attack & get_queen(opponent_side)){
            //Handle pawn promotion capture
            if( (piece ==  P && to_square/8 == 7) || (piece ==  p && to_square/8 == 0))    flags = 15; //Queen promotion capture
            add_move(movelist, encode(from_square, to_square, piece, flags, 1));
            
        }
        //attacking oponent bishop
        else if(attack & get_bishop(opponent_side)){
            //Handle pawn promotion capture
            if( (piece ==  P && to_square/8 == 7) || (piece ==  p && to_square/8 == 0))    flags = 13; //Bishop promotion capture
            add_move(movelist, encode(from_square, to_square, piece, flags, 2));
        }
        //attacking opponent knight
        else if(attack & get_knight(opponent_side)){
            //Handle pawn promotion capture
            if( (piece ==  P && to_square/8 == 7) || (piece ==  p && to_square/8 == 0))    flags = 12; //Knight promotion capture
            add_move(movelist, encode(from_square, to_square, piece, flags, 3));
        }
        //attacking opponent rook
        else if(attack & get_rook(opponent_side)){
            //Handle pawn promotion capture
            if( (piece ==  P && to_square/8 == 7) || (piece ==  p && to_square/8 == 0))    flags = 14; //Rook promotion capture

            add_move(movelist, encode(from_square, to_square, piece, flags, 4));
        }else if(attack & get_pawn(opponent_side)){
            add_move(movelist, encode(from_square, to_square, piece, flags, 5));
        }
        //Handle all attacking opponent pawn
        //  - Handle enpessant
        else if( piece == P && (to_square - 8) == enpessant){
            flags = 5; //turn on the enpessant capture flag
            add_move(movelist, encode(from_square, to_square, piece, flags, 5));
        }
        else if( piece == p && (to_square + 8) == enpessant){
            flags = 5; //turn on the enpessant capture flag
            add_move(movelist, encode(from_square, to_square, piece, flags, 5));
        }
        reset_lsb(piece_attacks);
    }
    printf(" \n \t DONE ENCODING CAPTURES \n");
}

void print_generated_moves(Moves* movelist){
    printf("generated moves");
    for(int i = 0; i < movelist->count; i++){
        int from = get_source(movelist->moves[i]);
        int to = get_target(movelist->moves[i]);
        int piece = get_piece(movelist->moves[i]);
        int flags = get_flags(movelist->moves[i]);
        int captured_piece = get_captured_piece(movelist->moves[i]);
        printf("\nFrom: %s, To: %s, Piece: %d, Flags: %d, Captured: %d..\n", square_to_coordinates[from], square_to_coordinates[to], piece, flags, captured_piece);
    }
}

/*  
    {'r','n','b','q','k','b','n','r'},
    {'p','p','p','p','p','p','p','p'},
    {' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' '},
    {'P','P','P','P','P','P','P','P'},
    {'R','N','B','Q','K','B','N','R'}
*/