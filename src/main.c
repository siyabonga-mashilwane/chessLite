#include <stdio.h>
#include "./pieces.h"
#include "./types.h"
#include <omp.h>

int main(){
    //omp_set_num_threads(10);
    char board[8][8] = {
        {'r',' ',' ','q','k',' ','n','r'},
        {'p','p','p','p',' ','p','p','p'},
        {' ',' ','n',' ',' ',' ',' ',' '},
        {' ',' ',' ',' ','p',' ',' ',' '},
        {' ',' ',' ',' ','P',' ',' ',' '},
        {'b',' ',' ','B',' ','N',' ','R'},
        {'P','P','P','P',' ','P','P','P'},
        {' ','N','B','Q','K',' ',' ',' '}
    };
    U64 RMagic[64] = {
    0x2000380201000008ULL,
    0x9002041009000030ULL,
    0x1000430100100082ULL,
    0x808008040002020ULL,
    0x801a0105000424ULL,
    0x12000018200000a0ULL,
    0x2000020800220040ULL,
    0x40801910d4004ULL,
    0x1020210000040180ULL,
    0x1915500080810013ULL,
    0x2800000882600ULL,
    0x28140010110aULL,
    0xc0018c1000262001ULL,
    0xa001000200060ULL,
    0xc00000e00a000500ULL,
    0x4050008210040800ULL,
    0x25c080804200101ULL,
    0x14a0000c00001004ULL,
    0x202008011001182ULL,
    0x88d288c004000ULL,
    0xa520042000120000ULL,
    0x80280304a109080ULL,
    0x841801400120030ULL,
    0x800080000100001ULL,
    0x3200a40580200001ULL,
    0x400020500c00200ULL,
    0x2020020410000010ULL,
    0x1000082400000400ULL,
    0x800402808004000ULL,
    0x40000300a102020ULL,
    0x80042002904000ULL,
    0x181080480000041ULL,
    0x540004900470302ULL,
    0x2019004000c382ULL,
    0x1040010001880800ULL,
    0x9000200289008000ULL,
    0x131042080000150ULL,
    0x220800040c04800ULL,
    0x8c02100001200ULL,
    0x2022188000040120ULL,
    0x20020891201800ULL,
    0x608022088440411ULL,
    0x8061840001108005ULL,
    0x870001a040ULL,
    0x7000000009018ULL,
    0x1001064a2250000ULL,
    0x2000820300180020ULL,
    0x2040c09000060020ULL,
    0x2008000004069210ULL,
    0x8b08001220009ULL,
    0x400032040004ULL,
    0x20006200040280c0ULL,
    0x20004403000010ULL,
    0x8000c001020c0001ULL,
    0x12040810902240ULL,
    0x400c802211420200ULL,
    0x1a10004004004ULL,
    0xa09540042500000ULL,
    0x540008008069ULL,
    0x1080c804200ULL,
    0xa000081241180380ULL,
    0x800880000402445ULL,
    0x8040035200202040ULL,
    0x400080002440020ULL,
    };

    U64 BMagic[64] = {
    0x8000900140840092ULL,
    0x804840004c900ULL,
    0x4018a4000100200ULL,
    0x2008028800020000ULL,
    0x4003108c18304c34ULL,
    0x2c08401405280000ULL,
    0x100780600010ULL,
    0xc408004a001040ULL,
    0x800420008018010ULL,
    0x1800012898000018ULL,
    0x62040981c0001032ULL,
    0x102120030402100ULL,
    0x10182840048180ULL,
    0x50405800000840ULL,
    0x4300085008ULL,
    0x11008402150a0200ULL,
    0x840011004044280ULL,
    0x704840810202ULL,
    0x8000020e080001ULL,
    0x8808008200808401ULL,
    0x44c20212800000ULL,
    0x18000d090041800ULL,
    0x10e020504211200ULL,
    0x200021018440400ULL,
    0xaa0001210064048ULL,
    0x241102000008124ULL,
    0x100002c12180204ULL,
    0x880200102070ULL,
    0x40200052008011ULL,
    0x430000d0ULL,
    0x8024c0005408060fULL,
    0x400002000c50060ULL,
    0x422008180c1020ULL,
    0x2308102a00414400ULL,
    0x2000082020004206ULL,
    0x8004000c2024090ULL,
    0x6006501801ULL,
    0x88880004685010ULL,
    0x10000802040003c0ULL,
    0x1010430800024000ULL,
    0x410041240190800ULL,
    0x1804000b00810100ULL,
    0x800002808000284ULL,
    0x648041000ULL,
    0x80010802081044ULL,
    0x8080024009001042ULL,
    0x3000dc020500020ULL,
    0x80042000e00004ULL,
    0x220802468032200ULL,
    0x10858020080a0ULL,
    0x2080604201818000ULL,
    0x8002100214000ULL,
    0x8040000821802000ULL,
    0xa0001200000c020ULL,
    0x100090004e800ULL,
    0x28890000282040ULL,
    0x40502920100000ULL,
    0x8020293001200840ULL,
    0x2605400040003000ULL,
    0x2200002002800446ULL,
    0x200806200a008ULL,
    0x801400600042100ULL,
    0x2900290080208400ULL,
    0x5000020058ULL,
    };
    int RBits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
    };

    int BBits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
    };
    printf("\n White bitboards \n");
    init_rack(board);
    init();
    /*U64 bishop_mask = get_bishop_mask(d4);
    U64 occupancy = set_occupancy((1 << BBits[d4])-1, bishop_mask, BBits[d4]);
    print_matrix(occupancy);
    print_matrix(bishop_attack(d4, occupancy));*/
    /*int square;
    printf("const uint64 RMagic[64] = {\n");
    for(square = 0; square < 64; square++)
        printf("  0x%llxULL,\n", find_magic(square, RBits[square], rook));
    printf("};\n\n");

    printf("const uint64 BMagic[64] = {\n");
    for(square = 0; square < 64; square++)
        printf("  0x%llxULL,\n", find_magic(square, BBits[square], bishop));
    printf("};\n\n");*/
    //print_attacks(d4);
    init_magic_attacks(RMagic, BMagic);
    print_matrix(get_sliding_attack(a1, get_game(),RMagic[a1], rook));
    print_matrix(get_sliding_attack(a1, get_game(),BMagic[a1], bishop));    
    print_matrix(bishop_attack(a1, get_game()));
    
    return 0;
}