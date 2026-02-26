//
// Created by rock on 12/20/23.
//

#ifndef C_CODE_INT_INT_HASH_MAP_H
#define C_CODE_INT_INT_HASH_MAP_H

// this is the only value that can't be used in the map of the entire INT range
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

// define a nice name for the data structure
typedef struct STRUCT_IntIntHashMap IntIntHashMap;

// fn. to create a new int-int hash map
IntIntHashMap* iihm_create(int initialSize);

// fn. to clear the hash map (ungrow and remove all data)
void iihm_clear(IntIntHashMap* data);

// fn. to de-allocate the hash map (data can't be used anymore after calling this!)
void iihm_free(IntIntHashMap* data);

// fn. to add a key/value to the hash map and return 1 if the key wasn't in there already
int iihm_add(IntIntHashMap* data, int key, int value);

// fn. to check if the map contain the key given key, returns 1 if it does
int iihm_contains(IntIntHashMap* data, int key);

// fn. to get the value for the associated key
int iihm_get(IntIntHashMap* data, int key);

// fn. to remove a key from the hash map, returns 1 if the value was removed
int iihm_remove(IntIntHashMap* data, int key);

#endif //C_CODE_INT_INT_HASH_MAP_H
