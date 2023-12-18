//
// Created by rock on 12/18/23.
//

#include <assert.h>
#include <stdio.h>
#include "../model/string_hash_set.h"

// test #1
void string_hash_set_test_1() {
    StringHashSet* map = str_hashset_create(10);
    assert(str_hashset_add(map, "test") == 1);
    assert(map->size == 1);
    assert(str_hashset_contains(map, "test") == 1);
    str_hashset_clear(map);
    // should be clear
    assert(map->size == 0);
    // de-alloc
    str_hashset_free(map);
}

void string_hash_set_test_2() {
    StringHashSet* map = str_hashset_create(10);
    assert(str_hashset_add(map, "test") == 1);
    assert(str_hashset_add(map, "test") == 0); // already exists
    assert(str_hashset_add(map, "test") == 0);
    assert(map->size == 1);
    assert(str_hashset_contains(map, "test") == 1);
    str_hashset_clear(map);
    // should be clear
    assert(map->size == 0);
    assert(str_hashset_contains(map, "test") == 0);
    // de-alloc
    str_hashset_free(map);
}

void string_hash_set_test_3() {
    StringHashSet* map = str_hashset_create(10);
    assert(str_hashset_add(map, "test") == 1);
    assert(str_hashset_add(map, "test1") == 1);
    assert(str_hashset_add(map, "test1") == 0); // already exists
    assert(map->size == 2);
    assert(str_hashset_contains(map, "test") == 1);
    assert(str_hashset_contains(map, "test1") == 1);
    // de-alloc
    str_hashset_free(map);
}

void string_hash_set_test_4() {
    StringHashSet* map = str_hashset_create(100);
    for (int i = 0; i < 99; i++) {
        char buf[256];
        sprintf(buf, "https://some.com/test_%d.html", i);
        assert(str_hashset_add(map, buf) == 1);
    }
    assert(map->size == 99);
    for (int i = 0; i < 99; i++) {
        char buf[256];
        sprintf(buf, "https://some.com/test_%d.html", i);
        assert(str_hashset_contains(map, buf) == 1);
    }
    // de-alloc
    str_hashset_free(map);
}

void string_hash_set_test_5() {
    StringHashSet* map = str_hashset_create(10);
    for (int i = 0; i < 99; i++) {
        char buf[256];
        sprintf(buf, "https://some.com/test_%d.html", i);
        assert(str_hashset_add(map, buf) == 1);
    }
    assert(map->size == 99);
    for (int i = 0; i < 99; i++) {
        char buf[256];
        sprintf(buf, "https://some.com/test_%d.html", i);
        assert(str_hashset_contains(map, buf) == 1);
    }
    // de-alloc
    str_hashset_free(map);
}

const char* extensions[9] = {"txt", "eml", "mp3", "doc", "docx", "avi", "xls", "xlsx", "ppt"};

void generate_test_string(char* str, int i) {
    int folder = i / 1000;
    const char* ext = extensions[i % 9];
    sprintf(str, "https://dataset.rock.co.nz/gov-docs-%d/file-%d.%s", folder, i, ext);
}

void string_hash_set_test_6() {
    int insertSize = 1000000;
    StringHashSet* map = str_hashset_create(500000);
    char str[256];
    for (int i = 1; i <= insertSize; i++) {
        generate_test_string(str, i);
        assert(str_hashset_add(map, str) == 1);
    }
    assert(insertSize == map->size);

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

void string_hash_set_test_7() {
    StringHashSet* map = str_hashset_create(100);
    char str[256];
    for (int i = 0; i < 99; i++) {
        generate_test_string(str, i);
        assert(str_hashset_add(map, str) == 1);
    }
    assert(99 == map->size);
    for (int i = 0; i < 99; i++) {
        generate_test_string(str, i);
        assert(str_hashset_remove(map, str) == 1);
    }
    assert(0 == map->size);
    // de-alloc
    str_hashset_free(map);
}

void string_hash_set_test_8() {
    StringHashSet* map = str_hashset_create(100);
    char str[256];
    for (int i = 0; i < 99; i++) {
        generate_test_string(str, i);
        assert(str_hashset_add(map, str) == 1);
    }
    assert(99 == map->size);
    for (int i = 0; i < 50; i++) {
        generate_test_string(str, i);
        assert(str_hashset_remove(map, str) == 1);
    }
    assert(49 == map->size);
    for (int i = 50; i < 99; i++) {
        generate_test_string(str, i);
        assert(str_hashset_contains(map, str) == 1);
    }
    // de-alloc
    str_hashset_free(map);
}

void string_hash_set_test_9() {
    StringHashSet* map = str_hashset_create(10);
    char str[256];
    for (int i = 0; i < 99; i++) {
        generate_test_string(str, i);
        assert(str_hashset_add(map, str) == 1);
    }
    assert(99 == map->size);
    for (int i = 0; i < 99; i++) {
        generate_test_string(str, i);
        assert(str_hashset_remove(map, str) == 1);
    }
    assert(0 == map->size);
    for (int i = 0; i < 99; i++) {
        generate_test_string(str, i);
        assert(str_hashset_add(map, str) == 1);
    }
    for (int i = 0; i < 99; i++) {
        generate_test_string(str, i);
        assert(str_hashset_contains(map, str) == 1);
    }
    // de-alloc
    str_hashset_free(map);
}

// run all tests
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

