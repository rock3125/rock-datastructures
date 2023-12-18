//
// Created by rock on 12/18/23.
//

#include <assert.h>
#include "../model/string_hash_set.h"

// test #1
void string_hash_set_test_1() {
    StringHashSet* map = str_hashset_create(10);
    assert(str_hashset_add(map, "test") == 1);
    assert(map->size == 1);
    assert(str_hashset_contains(map, "test") == 1);
}

// run all tests
void string_hash_set_tests() {
    string_hash_set_test_1();
}

