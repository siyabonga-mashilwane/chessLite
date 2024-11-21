#include "./pieces.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <omp.h>
#include "./types.h"
#include <string.h>

#define get_bit(bitboard, square) (bitboard & (1ULL << square))
#define pop_bit(bitboard, square) (get_bit(bitboard, square) ? bitboard ^= (1ULL << square) : 0)



//Static function prototypes
static U64 kingMoveTargetsHelper(U64 king);
static U64 bishop_mask_generator(int index);
static U64 rook_mask_generator(int index);

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

static U64 kingMoves[BOARDS_SQUARES];//A list of all possible moves a king can move given a position on the board
static U64 bishopMasks[BOARDS_SQUARES]; //A list of all possible masks a bishop can have given a position on the board
static U64 rookMasks[BOARDS_SQUARES]; //A list of all possible rook masks a rook can have given a position on the booard
U64 bishop_attacks[BOARDS_SQUARES][BISHOP_COMBINATIONS]; //Each square for a bishop has a maximum of 512 occupancy combination combinations  (2^9) to consider hence the 512 
U64 rook_attacks[BOARDS_SQUARES][ROOK_COMBINATIONS]; //Each square for a rook has a maximum of 4096 occupancy combinations (2^12) to consider hence 4096

//*********** */
U64 rook_magic_attacks[BOARDS_SQUARES][ROOK_COMBINATIONS];
U64 bishop_magic_attacks[BOARDS_SQUARES][BISHOP_COMBINATIONS];
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
}
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
//Production of Magic Numbers ************************************************////////////////////////////////
//A table of number of bits within a mask depending on the square
int rook_bits[64] = {
  12, 11, 11, 11, 11, 11, 11, 12,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  12, 11, 11, 11, 11, 11, 11, 12
};

int bishop_bits[64] = {
  6, 5, 5, 5, 5, 5, 5, 6,
  5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5,
  6, 5, 5, 5, 5, 5, 5, 6
};

//A function to return all possible combinations of occupancy given a board position
//attack_mask -> a variable that isolates all possible paths a sliding piece can go, except the edges and the position of the piece e.g b3
//bits_in_mask -> the number of bits that exist in the mask, use bitCount to determine the number of bits in the mask
/*
    index - a value ranging from 0 -> 2**(bits_in_mask -1), represents a state of combination.
    0 gives the zero'th combination you can get in that position.
    1 gives the 1st combination you can get in that position
    2 gives the 2nd combination you can get in that position
    .
    .
    .
    (2**bits_in_mask -1), this will return the last combination you can get at that position, it tends to 
    be the same as the attack_mask bitboard
*/ 
U64 set_occupancy(int index, U64 attack_mask, int bits_in_mask){
    //initialise the occupancy bitboard
    U64 occupancy = 0ULL;

    //Loop through all bits in the mask and check if they exist in the current combination state
    for (int i = 0; i < bits_in_mask; i++)
    {
        //get the ls1b of the attack mask
        int square = debruijn_BitScan(attack_mask);

        //set the ls1b to 0 so that we dont repeat it again
        pop_bit(attack_mask, square);

        //If current bit in the attack_mask exists in the current combination state then insert it into the occupancy
        if(index & (1 << i)){
            occupancy |= (1ULL << square);
        }
    }
    return occupancy;
}

//A function that will perform the multiplication of magic number and occupancy to form index
int multiply_magic(U64 occupancy, U64 magic, int bits){
    return (int)((occupancy*magic) >> (64 - bits));
}
//A random 64bit number generator
U64 xorshift64(U64 *state) {
    U64 x = *state;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    *state = x;
    return x;
}
uint32_t xorshift32(uint32_t* state) {
    uint32_t x;

    x = *state;
    x = x ^ (x << 13);
    x = x ^ (x >> 17);
    x = x ^ (x << 5);
    *state = x;
    return x;
}
U64 random_64(uint32_t *state){
    //Return an aligned random number, this discards unnecessary information in the random number
    U64 u1,u2,u3,u4;
    u1 = (U64)(xorshift32(state) & 0xFFFF);
    u2 = (U64)(xorshift32(state) & 0xFFFF);
    u3 = (U64)(xorshift32(state) & 0xFFFF);
    u4 = (U64)(xorshift32(state) & 0xFFFF);
    return u1 | u2 << 16 | u3 << 32 | u4 << 48;
}

U64 random_magic(uint32_t *state){
    return random_64(state) & random_64(state) & random_64(state);
}

// pseudo random number state
unsigned int random_state = 1804289383;

// generate 32-bit pseudo legal numbers
unsigned int get_random_U32_number()
{
    // get current state
    unsigned int number = random_state;
    
    // XOR shift algorithm
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;
    
    // update random number state
    random_state = number;
    
    // return random number
    return number;
}

// generate 64-bit pseudo legal numbers
U64 get_random_U64_number()
{
    // define 4 random numbers
    U64 n1, n2, n3, n4;
    
    // init random numbers slicing 16 bits from MS1B side
    n1 = (U64)(get_random_U32_number()) & 0xFFFF;
    n2 = (U64)(get_random_U32_number()) & 0xFFFF;
    n3 = (U64)(get_random_U32_number()) & 0xFFFF;
    n4 = (U64)(get_random_U32_number()) & 0xFFFF;
    
    // return random number
    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

// generate magic number candidate
U64 generate_magic_number()
{
    return get_random_U64_number() & get_random_U64_number() & get_random_U64_number();
}

//piece - 0 if bishop, 1 if rook
U64 find_magic(int square, int bits_in_mask, int piece){
    U64 seeds[] = {8977, 44560, 54343, 38998, 5731, 95205, 104912, 17020};
    U64 mask, b[ROOK_COMBINATIONS], a[ROOK_COMBINATIONS], used[ROOK_COMBINATIONS], magic;
    int i, j, k, fail;
    int max = (1 << bits_in_mask);
    uint32_t seed = 0xA634716A; //A randomly chosen seed
    mask = piece ? get_rook_mask(square) : get_bishop_mask(square);
    for(int i =0; i < max; i++){
        b[i] = set_occupancy(i, mask, bits_in_mask);
        a[i] = piece ? rook_attack(square, b[i]): bishop_attack(square, b[i]);
    }
    U64 found_magic = 0ULL;
    //#pragma omp parallel for num_threads(8) private(used, j, magic, fail) shared(b,a,mask,found_magic) schedule(dynamic, 1000) 
    
    for(int k = 0; k < 100000000; k++){
        //#pragma omp flush(found_magic)
        //if (found_magic) continue;
        magic = generate_magic_number();
        if(bitCount((mask*magic) & 0xFF00000000000000ULL) < 6) continue;
        memset(used, 0ULL, sizeof(used));
        for(int i = 0, fail = 0; !fail && i < max;i++){
            j = multiply_magic(b[i], magic, bits_in_mask);
            if(used[j] == 0ULL){
                used[j] = a[i];
            }else if(used[j] != a[i]){
                fail = 1;
            }
        }
        if(!fail){
            return magic;
        }
    }
    /*if(!fail){
        return found_magic;
    }*/
    printf("*****Failed*****\n");

    return 0ULL;
}


void init_magic_attacks(U64 rook_magics[], U64 bishop_magics[]){
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
    
}

U64 get_sliding_attack(Square sq, U64 occupancy, U64 magic, int rook){
    U64 temp = occupancy & (rook ? get_rook_mask(sq) : get_bishop_mask(sq));
    int index = multiply_magic(temp, magic, (rook ? rook_bits[sq] : bishop_bits[sq]));
    printf("\n attack index = %d\n", index);
    return (rook ? rook_magic_attacks[sq][index] : bishop_magic_attacks[sq][index]);
}

/*{'r','n','b','q','k','b','n','r'},
        {'p','p','p','p','p','p','p','p'},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {'P','P','P','P','P','P','P','P'},
        {'R','N','B','Q','K','B','N','R'}*/