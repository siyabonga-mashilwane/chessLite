#include "types.h"
#include "pieces.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define get_bit(bitboard, square) ((bitboard) & (1ULL << square))
#define pop_bit(bitboard, square) (get_bit((bitboard), square) ? ((bitboard) ^= (1ULL << square)) : 0)
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

//A function that will perform the multiplication of magic number and occupancy to form index
int multiply_magic(U64 occupancy, U64 magic, int bits){
    return (int)((occupancy*magic) >> (64 - bits));
}

unsigned int random_state = 1804289383;

// generate 32-bit pseudo legal numbers
uint32_t get_random_U32_number()
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
    return (uint32_t)number;
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
    //#pragma omp parallel for num_threads(5) private(used, j, magic, fail) shared(b,a,mask,found_magic) 
    
    for(int k = 0; k < 1000000; k++){
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

//The following magics are the results of the magic generator function
U64 rook_magic_numbers[64] = {
    0x8a80104000800020ULL,
    0x140002000100040ULL,
    0x2801880a0017001ULL,
    0x100081001000420ULL,
    0x200020010080420ULL,
    0x3001c0002010008ULL,
    0x8480008002000100ULL,
    0x2080088004402900ULL,
    0x800098204000ULL,
    0x2024401000200040ULL,
    0x100802000801000ULL,
    0x120800800801000ULL,
    0x208808088000400ULL,
    0x2802200800400ULL,
    0x2200800100020080ULL,
    0x801000060821100ULL,
    0x80044006422000ULL,
    0x100808020004000ULL,
    0x12108a0010204200ULL,
    0x140848010000802ULL,
    0x481828014002800ULL,
    0x8094004002004100ULL,
    0x4010040010010802ULL,
    0x20008806104ULL,
    0x100400080208000ULL,
    0x2040002120081000ULL,
    0x21200680100081ULL,
    0x20100080080080ULL,
    0x2000a00200410ULL,
    0x20080800400ULL,
    0x80088400100102ULL,
    0x80004600042881ULL,
    0x4040008040800020ULL,
    0x440003000200801ULL,
    0x4200011004500ULL,
    0x188020010100100ULL,
    0x14800401802800ULL,
    0x2080040080800200ULL,
    0x124080204001001ULL,
    0x200046502000484ULL,
    0x480400080088020ULL,
    0x1000422010034000ULL,
    0x30200100110040ULL,
    0x100021010009ULL,
    0x2002080100110004ULL,
    0x202008004008002ULL,
    0x20020004010100ULL,
    0x2048440040820001ULL,
    0x101002200408200ULL,
    0x40802000401080ULL,
    0x4008142004410100ULL,
    0x2060820c0120200ULL,
    0x1001004080100ULL,
    0x20c020080040080ULL,
    0x2935610830022400ULL,
    0x44440041009200ULL,
    0x280001040802101ULL,
    0x2100190040002085ULL,
    0x80c0084100102001ULL,
    0x4024081001000421ULL,
    0x20030a0244872ULL,
    0x12001008414402ULL,
    0x2006104900a0804ULL,
    0x1004081002402ULL
};

// bishop magic numbers
U64 bishop_magic_numbers[64] = {
    0x40040844404084ULL,
    0x2004208a004208ULL,
    0x10190041080202ULL,
    0x108060845042010ULL,
    0x581104180800210ULL,
    0x2112080446200010ULL,
    0x1080820820060210ULL,
    0x3c0808410220200ULL,
    0x4050404440404ULL,
    0x21001420088ULL,
    0x24d0080801082102ULL,
    0x1020a0a020400ULL,
    0x40308200402ULL,
    0x4011002100800ULL,
    0x401484104104005ULL,
    0x801010402020200ULL,
    0x400210c3880100ULL,
    0x404022024108200ULL,
    0x810018200204102ULL,
    0x4002801a02003ULL,
    0x85040820080400ULL,
    0x810102c808880400ULL,
    0xe900410884800ULL,
    0x8002020480840102ULL,
    0x220200865090201ULL,
    0x2010100a02021202ULL,
    0x152048408022401ULL,
    0x20080002081110ULL,
    0x4001001021004000ULL,
    0x800040400a011002ULL,
    0xe4004081011002ULL,
    0x1c004001012080ULL,
    0x8004200962a00220ULL,
    0x8422100208500202ULL,
    0x2000402200300c08ULL,
    0x8646020080080080ULL,
    0x80020a0200100808ULL,
    0x2010004880111000ULL,
    0x623000a080011400ULL,
    0x42008c0340209202ULL,
    0x209188240001000ULL,
    0x400408a884001800ULL,
    0x110400a6080400ULL,
    0x1840060a44020800ULL,
    0x90080104000041ULL,
    0x201011000808101ULL,
    0x1a2208080504f080ULL,
    0x8012020600211212ULL,
    0x500861011240000ULL,
    0x180806108200800ULL,
    0x4000020e01040044ULL,
    0x300000261044000aULL,
    0x802241102020002ULL,
    0x20906061210001ULL,
    0x5a84841004010310ULL,
    0x4010801011c04ULL,
    0xa010109502200ULL,
    0x4a02012000ULL,
    0x500201010098b028ULL,
    0x8040002811040900ULL,
    0x28000010020204ULL,
    0x6000020202d0240ULL,
    0x8918844842082200ULL,
    0x4010011029020020ULL
};