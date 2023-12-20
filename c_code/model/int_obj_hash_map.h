//
// Created by rock on 12/20/23.
//

#ifndef C_CODE_INT_OBJ_HASH_MAP_H
#define C_CODE_INT_OBJ_HASH_MAP_H

#define INT_OBJ_HASHMAP_EMPTY_KEY (-1)

struct STRUCT_IntObjHashMap {
    // a list of first indexes
    int* first;
    // an array keys
    int* keySet;
    // an array of objects
    void** valueSet;
    // an array of next offsets for collisions
    int* next;
    // how big the arrays are right now
    int allocatedSize;
    // how much data was allocated
    int initialSize;
    // how much data we have and where the offset is for the next entry
    int size;
};

typedef struct STRUCT_IntObjHashMap IntObjHashMap;

// create a new hash set
IntObjHashMap* iohm_create(int initialSize);

// clear the hash set
void iohm_clear(IntObjHashMap* data);

// de-allocate the hash set
void iohm_free(IntObjHashMap* data);

// add a key/value to the hash map and return 1 if the key wasn't in there already
int iohm_add(IntObjHashMap* data, int key, void* value);

// does the map contain the key?
int iohm_contains(IntObjHashMap* data, int key);

// get the value for the associated key
void* iohm_get(IntObjHashMap* data, int key);

// remove a key from the hash map, returns true if removed
int iohm_remove(IntObjHashMap* data, int key);

#endif //C_CODE_INT_OBJ_HASH_MAP_H
