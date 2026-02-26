#include <stdio.h>

// declared in string_hash_set_test.c
// these are the unit tests we can run
void string_hash_set_tests();

// we just run the unit tests - this is to be used as a library
int main() {
    string_hash_set_tests();
    return 0;
}
