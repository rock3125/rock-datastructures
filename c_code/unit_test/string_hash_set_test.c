//
// Created by rock on 12/18/23.
//

#include <assert.h>
#include <stdio.h>
#include "../model/string_hash_set.h"

// test #1
void string_hash_set_test_1() {
    // create a small string map (10 items)
    StringHashSet* map = str_hashset_create(10);
    // must add
    assert(str_hashset_add(map, "test") == 1);
    assert(map->size == 1); // right size
    assert(str_hashset_contains(map, "test") == 1); // contains works
    str_hashset_clear(map); // clear the map
    // should be clear now
    assert(map->size == 0);
    // de-alloc map
    str_hashset_free(map);
}

// string test 2
void string_hash_set_test_2() {
    // create a small string map (10 items)
    StringHashSet* map = str_hashset_create(10);
    assert(str_hashset_add(map, "test") == 1); // must add
    assert(str_hashset_add(map, "test") == 0); // already exists, not added again
    assert(str_hashset_add(map, "test") == 0); // try once more
    assert(map->size == 1); // only one item
    assert(str_hashset_contains(map, "test") == 1); // must have that string
    str_hashset_clear(map); // clear the map
    // should be clear now
    assert(map->size == 0);
    assert(str_hashset_contains(map, "test") == 0); // no longer contains this item
    // de-alloc
    str_hashset_free(map);
}

// third string test
void string_hash_set_test_3() {
    // create a small string map (10 items)
    StringHashSet* map = str_hashset_create(10);
    assert(str_hashset_add(map, "test") == 1); // new item
    assert(str_hashset_add(map, "test1") == 1); // new item
    assert(str_hashset_add(map, "test1") == 0); // already exists
    assert(map->size == 2); // two items
    assert(str_hashset_contains(map, "test") == 1); // must exist
    assert(str_hashset_contains(map, "test1") == 1); // must exist
    // de-alloc
    str_hashset_free(map);
}

// string test 4
void string_hash_set_test_4() {
    // create a small string map (100 items)
    StringHashSet* map = str_hashset_create(100);
    // add 99 strings
    for (int i = 0; i < 99; i++) {
        char buf[256];
        sprintf(buf, "https://some.com/test_%d.html", i);
        assert(str_hashset_add(map, buf) == 1); // must be added!
    }
    assert(map->size == 99); // must now have 99 items
    for (int i = 0; i < 99; i++) {
        char buf[256];
        sprintf(buf, "https://some.com/test_%d.html", i);
        assert(str_hashset_contains(map, buf) == 1); // must contain this item
    }
    // de-alloc
    str_hashset_free(map);
}

// string test 5
void string_hash_set_test_5() {
    // create a small string map (10 items)
    StringHashSet* map = str_hashset_create(10);
    // add more items that allocated (dynamic grow)
    for (int i = 0; i < 99; i++) {
        char buf[256];
        sprintf(buf, "https://some.com/test_%d.html", i);
        assert(str_hashset_add(map, buf) == 1); // must add
    }
    assert(map->size == 99);
    for (int i = 0; i < 99; i++) {
        char buf[256];
        sprintf(buf, "https://some.com/test_%d.html", i);
        assert(str_hashset_contains(map, buf) == 1); // must contain
    }
    // de-alloc
    str_hashset_free(map);
}

// some helper strings
const char* extensions[9] = {"txt", "eml", "mp3", "doc", "docx", "avi", "xls", "xlsx", "ppt"};

// some more extensive testing - helper, creates unique file-like https string
void generate_test_string(char* str, int i) {
    int folder = i / 1000;
    const char* ext = extensions[i % 9];
    sprintf(str, "https://dataset.rock.co.nz/gov-docs-%d/file-%d.%s", folder, i, ext);
}

// big string test
void string_hash_set_test_6() {
    // many items - big array
    int insertSize = 1000000;
    StringHashSet* map = str_hashset_create(insertSize / 2); // only half the size
    char str[256];
    for (int i = 1; i <= insertSize; i++) {
        generate_test_string(str, i);
        assert(str_hashset_add(map, str) == 1); // insert must work
    }
    assert(insertSize == map->size); // must be the right size

    // check contains works
    for (int i = 1; i <= insertSize; i++) {
        generate_test_string(str, i);
        assert(str_hashset_contains(map, str) == 1);
    }
    str_hashset_clear(map);
    // should be clear
    assert(map->size == 0);
    assert(str_hashset_contains(map, "test") == 0);
    // de-alloc
    str_hashset_free(map);
}

// string test 7
void string_hash_set_test_7() {
    // small map
    StringHashSet* map = str_hashset_create(100);
    char str[256];
    // add 99 items
    for (int i = 0; i < 99; i++) {
        generate_test_string(str, i);
        assert(str_hashset_add(map, str) == 1); // must insert
    }
    assert(99 == map->size); // check size
    for (int i = 0; i < 99; i++) {
        generate_test_string(str, i);
        assert(str_hashset_remove(map, str) == 1); // check contains
    }
    assert(0 == map->size);
    // de-alloc
    str_hashset_free(map);
}

// string test 8
void string_hash_set_test_8() {
    // small map
    StringHashSet* map = str_hashset_create(100);
    char str[256];
    // insert 99 strings
    for (int i = 0; i < 99; i++) {
        generate_test_string(str, i);
        assert(str_hashset_add(map, str) == 1);
    }
    assert(99 == map->size); // check size
    for (int i = 0; i < 50; i++) {
        generate_test_string(str, i);
        assert(str_hashset_remove(map, str) == 1); // remove first 50 strings
    }
    assert(49 == map->size); // size must match
    for (int i = 50; i < 99; i++) {
        generate_test_string(str, i);
        assert(str_hashset_contains(map, str) == 1); // must contain all remaining strings
    }
    // de-alloc
    str_hashset_free(map);
}

// string map test 9
void string_hash_set_test_9() {
    // create a small map
    StringHashSet* map = str_hashset_create(10);
    char str[256];
    // insert 99 items
    for (int i = 0; i < 99; i++) {
        generate_test_string(str, i);
        assert(str_hashset_add(map, str) == 1); // must insert
    }
    assert(99 == map->size); // must match
    for (int i = 0; i < 99; i++) {
        generate_test_string(str, i);
        assert(str_hashset_remove(map, str) == 1); // must remove
    }
    assert(0 == map->size); // empty now
    for (int i = 0; i < 99; i++) {
        generate_test_string(str, i);
        assert(str_hashset_add(map, str) == 1); // must add
    }
    for (int i = 0; i < 99; i++) {
        generate_test_string(str, i);
        assert(str_hashset_contains(map, str) == 1); // must contain
    }
    // de-alloc
    str_hashset_free(map);
}

// run all the above tests
void string_hash_set_tests() {
    printf("string_hash_set_test_1: ");
    string_hash_set_test_1();
    printf("passed\n");

    printf("string_hash_set_test_2: ");
    string_hash_set_test_2();
    printf("passed\n");

    printf("string_hash_set_test_3: ");
    string_hash_set_test_3();
    printf("passed\n");

    printf("string_hash_set_test_4: ");
    string_hash_set_test_4();
    printf("passed\n");

    printf("string_hash_set_test_5: ");
    string_hash_set_test_5();
    printf("passed\n");

    printf("string_hash_set_test_6: ");
    string_hash_set_test_6();
    printf("passed\n");

    printf("string_hash_set_test_7: ");
    string_hash_set_test_7();
    printf("passed\n");

    printf("string_hash_set_test_8: ");
    string_hash_set_test_8();
    printf("passed\n");

    printf("string_hash_set_test_9: ");
    string_hash_set_test_9();
    printf("passed\n");
}

