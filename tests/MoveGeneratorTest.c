#include <stdio.h>
#include <string.h>
#include "./MoveGeneratorTest.h"

TESTS initialize_test(FEN_TEST* test){
    TESTS tests;
    tests.count = 0;
    tests.tests[tests.count++] = *test;
    return tests;
}


