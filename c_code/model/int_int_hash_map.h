//
// Created by rock on 12/20/23.
//

#ifndef C_CODE_INT_INT_HASH_MAP_H
#define C_CODE_INT_INT_HASH_MAP_H

#define INT_INT_HASHMAP_EMPTY_KEY (-1)

struct STRUCT_IntIntHashMap {
    // a list of first indexes
    int* first;
    // an array keys
    int* keySet;
    // an array of values
    int* valueSet;
    // an array of next offsets for collisions
    int* next;
    // how big the arrays are right now
    int allocatedSize;
    // how much data was allocated
    int initialSize;
    // how much data we have and where the offset is for the next entry
    int size;
};

typedef struct STRUCT_IntIntHashMap IntIntHashMap;

// create a new int-int hash map
IntIntHashMap* iihm_create(int initialSize);

// clear the hash map
void iihm_clear(IntIntHashMap* data);

// de-allocate the hash map
void iihm_free(IntIntHashMap* data);

// add a key/value to the hash map and return 1 if the key wasn't in there already
int iihm_add(IntIntHashMap* data, int key, int value);

// does the map contain the key?
int iihm_contains(IntIntHashMap* data, int key);

// get the value for the associated key
int iihm_get(IntIntHashMap* data, int key);

// remove a key from the hash map
int iihm_remove(IntIntHashMap* data, int key);

#endif //C_CODE_INT_INT_HASH_MAP_H
