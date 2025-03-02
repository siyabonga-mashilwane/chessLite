#include "../src/types.h"
#include "../src/pieces.h"
U64 Perft(int depth, Colour side)
{
    Moves movelist;
    movelist.count = 0;
    int n_moves, i;
    U64 nodes = 0;

    if (depth == 0) 
        return 1ULL;
    generate(side, &movelist);
    n_moves = movelist.count;
    printf("Generated %d moves at depth %d, side is %s\n", n_moves, depth, (side==white?"white":"black:"));

    for (i = 0; i < n_moves; i++) {
        Move_History prev_move = make_move(movelist.moves[i], side);
        nodes += Perft(depth - 1, side^1);
        unmake_move(&prev_move);
    }
    return nodes;
}