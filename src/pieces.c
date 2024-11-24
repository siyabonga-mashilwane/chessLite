#include "./pieces.h"
#include <stdio.h>
#include <math.h>
#include "types.h"
#include "magics.h"
#include <string.h>

#define get_bit(bitboard, square) ((bitboard) & (1ULL << square))
#define pop_bit(bitboard, square) (get_bit((bitboard), square) ? ((bitboard) ^= (1ULL << square)) : 0)



//Static function prototypes
static U64 kingMoveTargetsHelper(U64 king);
static U64 bishop_mask_generator(int index);
static U64 rook_mask_generator(int index);

//Capital letters as White and lowecase letters as black
U64 bitboard_pieces[12]; //An array to hold all the bitboards of the game

static U64 kingMoves[BOARDS_SQUARES];//A list of all possible moves a king can move given a position on the board
static U64 bishopMasks[BOARDS_SQUARES]; //A list of all possible masks a bishop can have given a position on the board
static U64 rookMasks[BOARDS_SQUARES]; //A list of all possible rook masks a rook can have given a position on the booard
U64 bishop_magic_attacks[BOARDS_SQUARES][BISHOP_COMBINATIONS]; //Each square for a bishop has a maximum of 512 occupancy combination combinations  (2^9) to consider hence the 512 
U64 rook_magic_attacks[BOARDS_SQUARES][ROOK_COMBINATIONS];//Each square for a rook has a maximum of 4096 occupancy combinations (2^12) to consider hence 4096
struct Magic_sq magic_bishop[64];
struct Magic_sq magic_rook[64];

//*********** */


static U64 game = 0;//This variable is used to store the game state bitboard

//The following are the masks of move generators
 //The masks make sure that the pieces do not go off bound
const U64 notAFile = 0xfefefefefefefefe;
const U64 notHFile = 0x7f7f7f7f7f7f7f7f;
const U64 notBFile = 0xbfbfbfbfbfbfbfbf;
const U64 notGFile = 0xfdfdfdfdfdfdfdfd;
const U64 notABFile = 0x3f3f3f3f3f3f3f3f;
const U64 notGHFile = 0xfcfcfcfcfcfcfcfc;

void init(){
    init_king_targets();
    init_rook_masks();
    init_bishop_masks();
    init_magic_attacks(rook_magic_numbers, bishop_magic_numbers);
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

U64 get_game(){
    return game;
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
    for (size_t i = 0; i < 12; i++)
    {
        game |= bitboard_pieces[i];
    }
}
//A function that will produce a bitboard of empty spaces on the bitboard.
U64 createEmptySquares(){
    return ~game;
}



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
void init_king_targets(){
    U64 king = 1;
    for (size_t i = 0; i < BOARDS_SQUARES; i++, king = king<<1)
    {
        kingMoves[i] = kingMoveTargetsHelper(king);
    }
}

//A function that returns the possible moves of a king in a given game state/position.
U64 get_king_target(U64 king, U64 empty){
    int index = (int)log2(king);
    return kingMoves[index] & empty;
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
    printf("\n we are here \n");
    U64 occupied = occupancy;
    occupied &= magic_bishop[square_index].mask;
    occupied *= magic_bishop[square_index].magic;
    occupied >>= (int)(64 - (magic_bishop[square_index].bits));
    printf("\n Index is : %d\n", occupied);
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
                    printf(" %c ", ascii_pieces[i]);
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
//char* unicode_pieces[12] = {L"♚", L"♛", L"♝", L"♞", L"♜", L"♟︎", L"♔", L"♕", L"♗", L"♘", L"♖"};

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

/*{'r','n','b','q','k','b','n','r'},
        {'p','p','p','p','p','p','p','p'},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {'P','P','P','P','P','P','P','P'},
        {'R','N','B','Q','K','B','N','R'}*/