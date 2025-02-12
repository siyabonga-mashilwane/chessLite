#ifndef MOVEGENERATORTEST_H
#define MOVEGENERATORTEST_H
#define MAX_TESTS 100 // Define a maximum number of tests
struct FEN_TEST{
    char* fen;
    unsigned long long expected;
    int results;
};

struct TESTS {
    FEN_TEST tests[MAX_TESTS];
    int count;
};
TESTS initialize_test(FEN_TEST* test);
#endif
