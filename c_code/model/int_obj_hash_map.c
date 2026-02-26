/*
 * Copyright (c) 2024 by Rock de Vocht
 *
 * All rights reserved. No part of this publication may be reproduced, distributed, or
 * transmitted in any form or by any means, including photocopying, recording, or other
 * electronic or mechanical methods, without the prior written permission of the publisher,
 * except in the case of brief quotations embodied in critical reviews and certain other
 * noncommercial uses permitted by copyright law.
 *
 */

/**
 * a memory efficient int -> object hash map
 */


#include <stdlib.h>
#include <string.h>
#include "int_obj_hash_map.h"


/**
 * clear the hash map - remove all data and re-allocate to initial size if need be
 */
void iohm_clear(IntObjHashMap* data) {
    if (data == NULL)
        return;
    data->size = 0;
    // shrink the arrays?
    if (data->allocatedSize > data->initialSize) {
        // first release the allocated data
        if (data->first != NULL) free(data->first);
        if (data->keySet != NULL) free(data->keySet);
        if (data->valueSet != NULL) free(data->valueSet);
        if (data->next != NULL) free(data->next);

        // re-allocate the original sizes
        data->first = calloc(data->initialSize, sizeof(int));
        data->keySet = calloc(data->initialSize, sizeof(int));
        data->valueSet = calloc(data->initialSize, sizeof(void*));
        data->next = calloc(data->initialSize, sizeof(int));
        data->allocatedSize = data->initialSize;
    }

    // clear the arrays with "empty" keys so they appear as empty to our algorithm
    for (int i = 0; i < data->initialSize; i++) {
        data->first[i] = INT_OBJ_HASHMAP_EMPTY_KEY;
        data->keySet[i] = INT_OBJ_HASHMAP_EMPTY_KEY;
        data->valueSet[i] = NULL;
        data->next[i] = INT_OBJ_HASHMAP_EMPTY_KEY;
    }
}


/**
 * free all the data allocated by the IntObjHashMap
 */
void iohm_free_content_only(IntObjHashMap* data) {
    if (data == NULL) return; // no data, don't de-allocate
    // de-allocate the arrays first
    if (data->first != NULL) free(data->first);
    if (data->keySet != NULL) free(data->keySet);
    if (data->valueSet != NULL) free(data->valueSet);
    if (data->next != NULL) free(data->next);
    // set all items in data to NULL and 0
    data->first = NULL;
    data->keySet = NULL;
    data->valueSet = NULL;
    data->next = NULL;
    data->size = 0;
    data->allocatedSize = 0;
}


/**
 * free all the data allocated by the IntObjHashMap
 */
void iohm_free(IntObjHashMap* data) {
    // free content of map
    iohm_free_content_only(data);
    // then free the data itself
    free(data);
}


/**
 * create a new hash map
 */
IntObjHashMap* iohm_create(int initialSize) {
    // allocate the main structure
    IntObjHashMap* data = (IntObjHashMap*) calloc(1, sizeof(IntObjHashMap));
    if (data == NULL) return NULL; // failed?
    data->initialSize = initialSize;
    data->allocatedSize = initialSize;
    // allocate the key arrays
    data->first = calloc(data->initialSize, sizeof(int));
    data->keySet = calloc(data->initialSize, sizeof(int));
    data->valueSet = calloc(data->initialSize, sizeof(void*));
    data->next = calloc(data->initialSize, sizeof(int));
    // set the map size to 0
    data->size = 0;

    // clear all values inside the arrays as empty
    for (int i = 0; i < data->initialSize; i++) {
        data->first[i] = INT_OBJ_HASHMAP_EMPTY_KEY;
        data->keySet[i] = INT_OBJ_HASHMAP_EMPTY_KEY;
        data->valueSet[i] = NULL;
        data->next[i] = INT_OBJ_HASHMAP_EMPTY_KEY;
    }
    // done - return the new data structure
    return data;
}

// help insert a key/value into our map
int iohm_insertHelper(int key, void* value, IntObjHashMap* data) {
    if (data == NULL) return 0; // can't insert
    int firstIndex = abs(key % data->allocatedSize); // calculate the "key" offset
    int newSize = data->size;

    // simplest case - we don't have an entry yet
    if (data->first[firstIndex] == INT_OBJ_HASHMAP_EMPTY_KEY) {
        data->first[firstIndex] = data->size; // first points to the next empty data-slot
        // and the data goes into the slots
        data->keySet[data->size] = key;
        data->valueSet[data->size] = value;
        data->next[data->size] = INT_OBJ_HASHMAP_EMPTY_KEY;
        newSize += 1;

    } else {
        // chain down the colliding items and find the next empty
        int nextIndex = data->first[firstIndex];
        while (data->next[nextIndex] != INT_OBJ_HASHMAP_EMPTY_KEY) {
            if (data->keySet[nextIndex] == key) { // already exists, not added
                data->valueSet[nextIndex] = value;
                return data->size;
            }
            nextIndex = data->next[nextIndex];
        }
        if (data->keySet[nextIndex] == key) { // no new data added, already exists
            data->valueSet[nextIndex] = value;
            return data->size;
        }
        // now prevNext points to the last slot that wasn't empty - chain it in
        data->next[nextIndex] = data->size;
        // and put in our data at the end
        data->keySet[data->size] = key;
        data->valueSet[data->size] = value;
        data->next[data->size] = INT_OBJ_HASHMAP_EMPTY_KEY;
        newSize += 1;
    }
    return newSize;
}

// grow the map by 50%
void iohm_grow(IntObjHashMap* data) {
    if (data == NULL) return; // empty map, can't grow
    int oldSize = data->allocatedSize;
    int growSize = ((oldSize * 3) / 2) + 1; // 50% growth

    IntObjHashMap* newData = iohm_create(growSize);
    if (newData == NULL) return;

    // copy existing data into the new array
    for (int i = 0; i < data->allocatedSize; i++) {
        int oldFirst = data->first[i];
        int oldNextIndex = oldFirst;
        while (oldNextIndex != INT_OBJ_HASHMAP_EMPTY_KEY) {
            int key = data->keySet[oldNextIndex];
            void* value = data->valueSet[oldNextIndex];
            newData->size = iohm_insertHelper(key, value, newData);
            oldNextIndex = data->next[oldNextIndex];
        }

    } // for each existing entry

    // transfer the newly mapped data
    iohm_free_content_only(data);
    // copy newData into data, as this is what is being replaced / grown
    data->first = newData->first;
    data->next = newData->next;
    data->keySet = newData->keySet;
    data->valueSet = newData->valueSet;
    data->size = newData->size;
    data->initialSize = newData->initialSize;
    data->allocatedSize = newData->allocatedSize;
    free(newData); // de-allocate temporary helper data
}


/**
 * add a key / value
 * @return true if a new item was added, false if the item already existed
 */
int iohm_add(IntObjHashMap* data, int key, void* value) {
    // we can never insert an "empty key" value - or into a NULL data array
    if (key == INT_OBJ_HASHMAP_EMPTY_KEY || data == NULL)
        return 0;

    // do we need to grow our arrays and remap all existing data?
    if (data->size + 1 >= data->allocatedSize) {
        iohm_grow(data);
    }

    // get an index into the first array
    int oldSize = data->size;
    data->size = iohm_insertHelper(key, value, data);
    return data->size > oldSize;
}


/**
 * is key inside the map (does it exist)
 */
int iohm_contains(IntObjHashMap* data, int key) {
    // we can never insert an "empty key" value - or into a NULL data array
    if (key == INT_OBJ_HASHMAP_EMPTY_KEY || data == NULL) return 0;
    int firstIndex = abs(key % data->allocatedSize); // start location
    int nextIndex = data->first[firstIndex]; // the data at that location
    if (nextIndex == INT_OBJ_HASHMAP_EMPTY_KEY) // no data
        return 0; // nothing to remove
    while (data->next[nextIndex] != INT_OBJ_HASHMAP_EMPTY_KEY) {
        if (data->keySet[nextIndex] == key)
            return 1; // found it!
        nextIndex = data->next[nextIndex];
    }
    // found that key?
    return (data->keySet[nextIndex] == key);
}


/**
 * remove a key from the map (delete)
 * @return 1 if an item was removed, 0 otherwise
 */
int iohm_remove(IntObjHashMap* data, int key) {
    // can't remove something from a NULL data structure, or an empty key
    if (data == NULL || key == INT_INT_HASHMAP_EMPTY_KEY) return 0;
    int firstIndex = abs(key % data->allocatedSize); // start location
    int nextIndex = data->first[firstIndex]; // data at location
    if (nextIndex == INT_OBJ_HASHMAP_EMPTY_KEY) // no data
        return 0; // nothing to remove
    int prevIndex = nextIndex;
    while (data->next[nextIndex] != INT_OBJ_HASHMAP_EMPTY_KEY) {
        if (data->keySet[nextIndex] == key)
            break; // found it!
        prevIndex = nextIndex;
        nextIndex = data->next[nextIndex];
    }
    // found?
    if (data->keySet[nextIndex] == key) {
        // first update?
        if (nextIndex == data->first[firstIndex]) {
            data->first[firstIndex] = data->next[nextIndex]; // unchain first item
            // does it remove the item completely?
            if (data->next[nextIndex] == INT_OBJ_HASHMAP_EMPTY_KEY) {
                data->keySet[nextIndex] = INT_OBJ_HASHMAP_EMPTY_KEY;
                data->valueSet[nextIndex] = NULL;
            }
        } else {
            data->next[prevIndex] = data->next[nextIndex]; // skip one in the chain
        }
        data->size -= 1; // decrease size of map
        return 1; // success
    }
    return 0; // not found
}
