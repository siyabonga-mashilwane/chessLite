#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_LENGTH 256
#define MAX_DEPTHS 6

typedef struct {
    char fen[MAX_LINE_LENGTH];
    int depths[MAX_DEPTHS];
} PerftData;

void parse_line(const char *line, PerftData *data) {
    char *token;
    char *line_copy = strdup(line); // Create a modifiable copy of the line

    // Extract FEN
    token = strtok(line_copy, ";");
    if (token != NULL) {
        strncpy(data->fen, token, MAX_LINE_LENGTH);
    }
    

    // Extract depth numbers
    int depth_index = 0;
    while (token != NULL && depth_index < MAX_DEPTHS) {
        if (token[0] == 'D' && token[1] >= '1' && token[1] <= '6') {
            data->depths[depth_index++] = atoi(token + 2);
        }
        token = strtok(NULL, ";");
    }

    free(line_copy); // Free the allocated memory
}
int read_chessLite(){
    FILE *file = fopen("C:/Users/siyabonga/Documents/GitHub/chessLite/tests/chesslite.txt", "r");
    if (!file) {
        printf("Failed to open chesslite file");
        return EXIT_FAILURE;
    }

    char line[MAX_LINE_LENGTH];
    char result[40];
    int number = 0;
    while (fgets(line, sizeof(line), file))
    {
        
        if (sscanf(line, "Nodes searched: %s ", result)) {
            number = atoi(result);
        }
    }
    //printf("Result: %s\n", result);
    //printf("Number: %d\n", number);
    fclose(file);
    return number;
}
int main() {
    FILE *file = fopen("c:/Users/siyabonga/Documents/GitHub/chessLite/tests/perftsuite.epd", "r");
    if (!file) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    char line[MAX_LINE_LENGTH];
    printf("%-50s %-10s %-10s %-10s %-10s %-10s %-10s %-10s %-5s\n", "FEN", "D1(vs Stockfish)", "D2(vs Stockfish)", "D3(vs Stockfish)", "D4(vs Stockfish)", "D5(vs Stockfish)", "D6(vs Stockfish)", "Results", "PASS?");
    printf("---------------------------------------------------------------------------------------------------------------------------------\n");

    while (fgets(line, sizeof(line), file)) {
        PerftData data = { .fen = "", .depths = {0} };
        parse_line(line, &data);

        int my_results[MAX_DEPTHS] = {0};
        int passed_depths = 0;
        bool all_pass = true;
        // Print the extracted data
        for (int i = 0; i < MAX_DEPTHS; i++) {
            char command[MAX_LINE_LENGTH];
            snprintf(command, sizeof(command), "C:\\Users\\siyabonga\\Documents\\GitHub\\chessLite\\bin\\piecesTest.exe -fen \"%s\" -depth %d > \"C:\\Users\\siyabonga\\Documents\\GitHub\\chessLite\\tests\\chesslite.txt\"", data.fen, i+1);
            unsigned int ret = system(command);
            
            if (ret != 0) {
                printf("Error executing command, return code: %u\n", ret);
            }
            my_results[i] = read_chessLite();
            
            if (my_results[i] == data.depths[i]) {
                passed_depths++;
            } else {
                all_pass = 0;
            }
        }
        printf("%-50s ", data.fen);
        for (int i = 0; i < MAX_DEPTHS; i++) {
            printf("%-10d ", my_results[i]);
        }
        printf("%-10d/6 %-5s\n", passed_depths, all_pass ? "PASS" : "FAIL");
    
    }

    fclose(file);
    return 0;
}