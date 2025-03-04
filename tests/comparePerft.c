#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_MOVES 256
#define MAX_LINE_LENGTH 100

typedef struct {
    char move[10];
    unsigned long long nodes;
} MoveNode;

int compare_moves(const void *a, const void *b) {
    return strcmp(((MoveNode *)a)->move, ((MoveNode *)b)->move);
}

void read_file(const char *filename, MoveNode *moves, int *count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    *count = 0;
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%s %llu", moves[*count].move, &moves[*count].nodes) == 2) {
            (*count)++;
        }
    }

    fclose(file);
}

void compare_results(MoveNode *moves1, int count1, MoveNode *moves2, int count2) {
    qsort(moves1, count1, sizeof(MoveNode), compare_moves);
    qsort(moves2, count2, sizeof(MoveNode), compare_moves);

    int i = 0, j = 0;
    while (i < count1 && j < count2) {
        int cmp = strcmp(moves1[i].move, moves2[j].move);
        if (cmp == 0) {
            if (moves1[i].nodes != moves2[j].nodes) {
                printf("Difference in nodes for move %s: %llu (Your Perft) vs %llu (Stockfish)\n", moves1[i].move, moves1[i].nodes, moves2[j].nodes);
            }
            i++;
            j++;
        } else if (cmp < 0) {
            printf("Move %s: %llu (Your Perft) vs N/A (Stockfish)\n", moves1[i].move, moves1[i].nodes);
            i++;
        } else {
            printf("Move %s: N/A (Your Perft) vs %llu (Stockfish)\n", moves2[j].move, moves2[j].nodes);
            j++;
        }
    }

    while (i < count1) {
        printf("Move %s: %llu (Your Perft) vs N/A (Stockfish)\n", moves1[i].move, moves1[i].nodes);
        i++;
    }

    while (j < count2) {
        printf("Move %s: N/A (Your Perft) vs %llu (Stockfish)\n", moves2[j].move, moves2[j].nodes);
        j++;
    }
}

int main() {
    MoveNode moves1[MAX_MOVES], moves2[MAX_MOVES];
    int count1, count2;

    read_file("chesslite.txt", moves1, &count1);
    read_file("stockfish.txt", moves2, &count2);

    compare_results(moves1, count1, moves2, count2);

    return 0;
}