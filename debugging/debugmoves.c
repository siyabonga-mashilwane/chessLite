#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_MOVES 256
#define MAX_LINE_LENGTH 100

typedef struct {
    char move[10];
    unsigned long long nodes;
} MoveNode;

void run_stockfish(const char* bash,const char *fen, int depth, const char *engine, const char *output_file){
    printf("\nRunning Stockfish at depth %d\n", depth);
    char command[350];
    system("ls -l /c/Users/siyabonga/Documents/GitHub/chessLite/Stockfish/src/stockfish.exe");
    snprintf(command, sizeof(command), "%s -c \"echo -e 'position fen %s\\ngo perft %d' | '%s' | sed '1,5d' > '%s'\"", bash, fen, depth, engine, output_file);
    printf("\n\t command: %llu\n", strlen(command));
    printf("\t Command: %s\n", command);
    int result = system(command);
    printf("\t System call returned: %d\n", result);
}

void run_chesslite(const char* bash, const char *fen, int depth, const char *engine, const char *output_file, const char *cutoff_from, const char *cutoff_to) {
    printf("Running chesslite at depth %d\n", depth);
    char command[500]; // Increased buffer size for safety

    printf("\tAfter command\n");
    printf("\t From %s to %s\n", cutoff_from, cutoff_to);

    if (cutoff_from && cutoff_to && strlen(cutoff_from) > 0 && strlen(cutoff_to) > 0) {
        snprintf(command, sizeof(command), 
            "%s -c \"'%s' -fen \\\"%s\\\" -depth %d -from \\\"%s\\\" -to \\\"%s\\\" > \\\"%s\\\"\"",
            bash, engine, fen, depth, cutoff_from, cutoff_to, output_file);
    } else {
        snprintf(command, sizeof(command), 
            "%s -c \"'%s' -fen \\\"%s\\\" -depth %d > \\\"%s\\\"\"",
            bash, engine, fen, depth, output_file);
    }

    printf("\t Command length: %llu\n", strlen(command));
    printf("\t Command: %s\n", command);

    int result = system(command);
    printf("\t System call returned: %d\n", result);
}

void run_compare_perft(const char* bash, const char *compare_engine, const char *comparison_output) {
    char command[256];
    printf("Running comparePerft\n");
    snprintf(command, sizeof(command), "%s -c \" '%s' > '%s'\"", bash, compare_engine, comparison_output);
    printf("\t Command: %s\n", command);
    int result = system(command);
    printf("\t System call returned: %d\n", result);
}

void drill_down(const char* bash,const char *fen, int depth, const char *engine1, const char *engine2, const char *compare_engine, const char *comparison_output, int cutoff_depth) {
    if(depth == cutoff_depth){
        printf("\nCutoff depth reached\n");
        return;
    }
    printf("\ndrill_down\n");
    if (depth < 1) return;
    char output1[256] = "/c/Users/siyabonga/Documents/GitHub/chessLite/tests/chesslite.txt";
    char output2[256] = "/c/Users/siyabonga/Documents/GitHub/chessLite/tests/stockfish.txt";
    char from[10] = "\0";
    char to[10] = "\0";
    run_chesslite(bash,fen, depth, engine1, output1, from, to);
    run_stockfish(bash,fen, depth, engine2, output2);
    run_compare_perft(bash, compare_engine, comparison_output);
    printf("\nFinished running engines\n");
    //Check if file has invalid moves
    FILE *temp_file = fopen("C:/Users/siyabonga/Documents/GitHub/chessLite/tests/comparisonOutput.txt", "r");
    if (!temp_file) {
        perror("Failed to open comparison results file");
        exit(EXIT_FAILURE);
    }
    char temp_ln[MAX_LINE_LENGTH];
    while (fgets(temp_ln, sizeof(temp_ln), temp_file)) {
        if (strstr(temp_ln, "N/A (Your Perft)") != NULL || strstr(temp_ln, "N/A (Stockfish)") != NULL) {
            printf("Invalid moves detected at depth %d\n", depth);
            return;
        }
    }
    fclose(temp_file);
    // Read the comparison results from comparePerft.exe
    FILE *file = fopen("C:/Users/siyabonga/Documents/GitHub/chessLite/tests/comparisonOutput.txt", "r");
    if (!file) {
        perror("Failed to open comparison results file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    char first_diff_move[10] = "";
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "Difference in nodes for move") != NULL) {
            sscanf(line, "Difference in nodes for move %s:", first_diff_move);
            break;
        }
    }
    fclose(file);

    if (strlen(first_diff_move) > 0) {
        char new_fen[256];
        //now run the new perft with a cutoff, thus producing a new FEN file
        snprintf(from, sizeof(from), "%.2s", first_diff_move);
        snprintf(to, sizeof(to), "%.2s", first_diff_move + 2);
        run_chesslite(bash,fen, depth, engine1, output1, from, to);
        //read the new FEN file
        FILE* fen_file = fopen("C:/Users/siyabonga/Documents/GitHub/chessLite/tests/FENResults.txt", "r");
        fgets(new_fen, sizeof(new_fen), fen_file);
        fclose(fen_file);
        
        drill_down(bash,new_fen, depth - 1, engine1, engine2, compare_engine, comparison_output, cutoff_depth);
    }
}

int main(int argc, char *argv[]) {
    const char *engine1 = "/c/Users/siyabonga/Documents/GitHub/chessLite/bin/piecesTest.exe";
    const char *engine2 = "/c/Users/siyabonga/Documents/GitHub/chessLite/Stockfish/src/stockfish.exe";
    const char *compare_engine = "/c/Users/siyabonga/Documents/GitHub/chessLite/tests/comparePerft.exe";
    const char *comparison_output = "/c/Users/siyabonga/Documents/GitHub/chessLite/tests/comparisonOutput.txt";
    const char* bash = "C:/msys64/usr/bin/bash.exe";
    const char *initial_fen;
    int depth = 0;
    int cutoff_depth = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-fen") == 0 && i + 1 < argc) {
            initial_fen = argv[i + 1];
            //printf("\n fen:  %s\n", initial_fen);
            i++; 
        } else if (strcmp(argv[i], "-depth") == 0 && i + 1 < argc) {
            depth = atoi(argv[i + 1]);
            i++; 
        }else if(strcmp(argv[i], "-cdepth") == 0 && i + 1 < argc){
            cutoff_depth = atoi(argv[i + 1]);
            i++;
        }else {
            printf("Unknown argument: %s\n", argv[i]);
        }
    }
    printf("\n fen:  %s\n depth: %d\n", initial_fen, depth);
    drill_down(bash, initial_fen, depth, engine1, engine2, compare_engine, comparison_output, cutoff_depth);
    printf("\nFinished running drill down\n");
    return 0;
}