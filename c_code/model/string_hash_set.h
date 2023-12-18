//
// Created by rock on 12/18/23.
//

#ifndef C_CODE_STRING_HASH_SET_H
#define C_CODE_STRING_HASH_SET_H

#define STRING_HASHMAP_EMPTY_KEY (-1)

struct STRUCT_StringHashSet {
    // a list of first indexes
    int* first;
    // an array of type 1 hashes identifying a string
    int* intHash1;
    // an array of type 2 hashes identifying a string
    int* intHash2;
    // an array of next offsets for collisions
    int* next;
    // how big the arrays are right now
    int allocatedSize;
    // how much data was allocated
    int initialSize;
    // how much data we have and where the offset is for the next entry
    int size;
};

typedef struct STRUCT_StringHashSet StringHashSet;

// create a new hash set
StringHashSet* str_hashset_create(int initialSize);

// clear the hash set
void str_hashset_clear(StringHashSet* data);

// de-allocate the hash set
void str_hashset_free(StringHashSet* data);

// add a new string into the hash set and return 1 if it wasn't in there already
int str_hashset_add(StringHashSet* data, const char* str);

// does the map contain str?
int str_hashset_contains(StringHashSet* data, const char* str);

// remove a string from the hash set
int str_hashset_remove(StringHashSet* data, const char* str);

#endif //C_CODE_STRING_HASH_SET_H
