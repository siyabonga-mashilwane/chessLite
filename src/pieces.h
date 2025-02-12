#ifndef PIECES_H
#define PIECES_H
#include "./types.h"
#include <wchar.h>



// The following functionsmight need to be moved into the library file in the future

//An array of debruijn possible positions, it received a debruijn sequence (subset) number then returns the index of that position on the
//true bitboards
static int debruijn_hash[] = {
   63,  0, 58,  1, 59, 47, 53,  2,
   60, 39, 48, 27, 54, 33, 42,  3,
   61, 51, 37, 40, 49, 18, 28, 20,
   55, 30, 34, 11, 43, 14, 22,  4,
   62, 57, 46, 52, 38, 26, 32, 41,
   50, 36, 17, 19, 29, 10, 13, 21,
   56, 45, 25, 31, 35, 16,  9, 12,
   44, 24, 15,  8, 23,  7,  6,  5
};

static U64 debruijn_number = 0x07EDD5E59A4E28C2ULL; //A special 64bit debruijn master set/sequence to extract some subsets

//It returns an index of the isolated LSB from the bitboard, receives a chess piece bitboard
static inline int debruijn_BitScan(U64 bitboard){
    return debruijn_hash[((bitboard & -bitboard) * debruijn_number) >> 58];
}

int bitCount(U64 val); //change to inline and remove it from the header
//A function that initialises all pre-calculated data
void init();

static Pieces pieces_by_color[2][6] = {
    {K, Q, B, N, R, P},
    {k, q, b, n, r, p}
};
//The following are getters and setters of the pieces where uppercase is white and lowercase is black
U64 get_K();
U64 get_Q();
U64 get_B();
U64 get_N();
U64 get_R();
U64 get_P();

U64 get_k();
U64 get_q();
U64 get_b();
U64 get_n();
U64 get_r();
U64 get_p();

U64 get_game();
U64 get_side_bitboard(Colour side);

void set_K(U64 K);
void set_Q(U64 Q);
void set_B(U64 B);
void set_N(U64 N);
void set_R(U64 R);
void set_P(U64 P);

void set_k(U64 k);
void set_q(U64 q);
void set_b(U64 b);
void set_n(U64 n);
void set_r(U64 r);
void set_p(U64 p);

void set_game(U64 val);

//The following are the masks of move generators
extern const U64 notAFile;
extern const U64 notHFile;
extern const U64 notBFile;
extern const U64 notGFile;
extern const U64 notABFile;
extern const U64 notGHFile;


//Use this function to create a bitboard of empty spaces
U64 createEmptySquares();
// Use this function to print binary numbers for pieces
void print_binary(U64 n);

//Use this function to print an 8x8 matrix of the bits
void print_matrix(U64 word);

//This function returns an array of bitboards at the start of the game
void init_rack(char board[8][8]);

//This function is the only function that can be used to update the game bitboard
void updateGame();



//Single Move generators
U64 eastOne(U64 bitboard);
U64 westOne(U64 bitboard);
U64 northOne(U64 bitboard);
U64 southOne(U64 bitboard);
U64 northEastOne(U64 bitboard);
U64 southEastOne(U64 bitboard);
U64 northWestOne(U64 bitboard);
U64 southWestOne(U64 bitboard);

//Pawn  pushing generators

U64 wSinglePush(U64 wPawns, U64 emptySquares); //Empty squares is a bitboard that is set to 1 on all empty squares and 0 on all occupied squares
U64 wDoublePush(U64 wPawns, U64 emptySquares); 
U64 bSinglePush(U64 bPawns, U64 emptySquares);
U64 bDoublePush(U64 bPawns, U64 emptySquares);

U64 get_pawn_attack(Colour side, int square); // Returns a pawn attack given a square for the pawn to move
void init_pawn_attacks(); // a function to initialise all the pawn attacks array
U64 wSinglePushSources(U64 wPawns, U64 emptySquares); //Returns all sources of white pawn squares you can single push
U64 wDoublePushSources(U64 wPawns, U64 emptySquares); //Returns all sources of white pawn squares you can double push
U64 bSinglePushSources(U64 bPawns, U64 emptySquares); //Returns all sources of Black pawn squares you can single push
U64 bDoublePushSources(U64 bPawns, U64 emptySquares); //Returns all sources of Black pawn squares you can double push


//Knight moving generators
U64 knightMoveTargets(U64 knights);
U64 get_knight_attack(int square);
void init_knight_attacks();

//King moving generators
void init_king_targets();
U64 get_king_target(int index, U64 empty);

//Bishop utilities
void init_bishop_masks();
U64 get_bishop_mask(int index);
U64 bishop_attack(int index, U64 occupancy);
U64 bishop_magic_attack(int square_index, U64 occupancy);


//Rook utilities
void init_rook_masks();
U64 get_rook_mask();
U64 rook_attack(int square_index, U64 occupancy);
U64 rook_magic_attack(int square_index, U64 occupancy);

//Queen utilities
U64 queen_magic_attack(int square_index, U64 occupancy);

//Sliding pieces utilities
U64 set_occupancy(int index, U64 attack_mask, int bits_in_mask);
void set_sliding_attacks();
void init_bishop_attacks(U64 bishop_magics);
void init_rook_attacks(U64 rook_magics);
U64 get_sliding_attack(Square sq, U64 occupancy, U64 magic, int rook);

void init_magic_attacks(U64 rook_magics[], U64 bishop_magics[]);
/************************************************************************************************************************************************** */
//These functions are undeer review they need to be transported to the generator.h
void print_chessboard();
extern wchar_t* unicode_pieces[12];
extern char ascii_pieces[12];
extern int char_pieces[];

void print_pieces();
void fen_parser(const char* fen);

//A function that has to return a boolean value as to whether that square is attacked by the other side/player
bool is_square_attacked(int square, Colour side, U64 occupancy);
void init_in_between(); //A function to initialize the arr_rectangular array, that holds all in between squares of from(square) and to(square)
U64 inBetween(int sq1, int sq2); //A function to find all the squares between the 2 squares, from and to
U64 get_in_between(int from, int to); //a function to get all the squares in between from and to

//X-ray functions
U64 xray_rook(U64 blockers, U64 occupied, Square rookSq);
U64 xray_bishop(U64 blockers, U64 occupied, Square bishopSq);
void absolute_pins(Pinned_pieces *pinned_pieces,Square kingSq, U64* blockers, U64 occupied,U64 opponent_queens, U64 opponent_bishops, U64 opponent_rooks, Pieces blocker_piece, bool is_pawn);

//Move generation
void generate(Colour side,  Moves *movelist);

//move: its an encoded int value, with a source and a target square, a piece name and flags
static inline void add_move(Moves *list, int move){
    //add move to the given move list
    list->moves[list->count] = move;
    //increase count to point to the next move in the list
    list->count++;
}
void generate_pinned_pawn_moves(Colour side,Moves* movelist, Pinned_pieces* pinned_pawns,U64 push_mask,U64 capture_mask);
void init_pinned_pieces(Pinned_pieces *pinned_pieces);
void encode_quiet_moves(Moves *movelist, Pieces piece, U64 quiet_moves, int from_square, Colour side);
void encode_captures(Moves *movelist, Pieces piece, U64 piece_attacks, int from_square, Colour side);
void print_generated_moves(Moves* movelist);
#endif