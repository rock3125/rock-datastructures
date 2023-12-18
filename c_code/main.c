#include <stdio.h>
#include <assert.h>

#include "model/string_hash_set.h"

void string_hash_set_tests() {
    StringHashSet* map = str_hashset_create(10);
    assert(str_hashset_add(map, "test") == 1);
    assert(map->size == 1);
    assert(str_hashset_contains(map, "test") == 1);
}


int main() {
    string_hash_set_tests();
    return 0;
}
