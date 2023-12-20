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
 * a memory efficient mostly accurate String hash set
 * checking for the presence / existence of a string
 * based on double hash values 0 allocating 20 bytes per string total
 *
 */


#include <stdlib.h>
#include <string.h>
#include "int_int_hash_map.h"


/**
 * clear the hash set - remove all data
 */
void iihm_clear(IntIntHashMap* data) {
    if (data == NULL)
        return;
    data->size = 0;
    // shrink the arrays?
    if (data->allocatedSize > data->initialSize) {
        if (data->first != NULL) free(data->first);
        if (data->keySet != NULL) free(data->keySet);
        if (data->valueSet != NULL) free(data->valueSet);
        if (data->next != NULL) free(data->next);

        data->first = calloc(data->initialSize, sizeof(int));
        data->keySet = calloc(data->initialSize, sizeof(int));
        data->valueSet = calloc(data->initialSize, sizeof(int));
        data->next = calloc(data->initialSize, sizeof(int));
        data->allocatedSize = data->initialSize;
    }

    for (int i = 0; i < data->initialSize; i++) {
        data->first[i] = INT_INT_HASHMAP_EMPTY_KEY;
        data->keySet[i] = INT_INT_HASHMAP_EMPTY_KEY;
        data->valueSet[i] = INT_INT_HASHMAP_EMPTY_KEY;
        data->next[i] = INT_INT_HASHMAP_EMPTY_KEY;
    }
}


/**
 * free all the data allocated by the IntIntHashMap
 */
void iihm_free_content_only(IntIntHashMap* data) {
    if (data->first != NULL) free(data->first);
    if (data->keySet != NULL) free(data->keySet);
    if (data->valueSet != NULL) free(data->valueSet);
    if (data->next != NULL) free(data->next);
    data->first = NULL;
    data->keySet = NULL;
    data->valueSet = NULL;
    data->next = NULL;
    data->size = 0;
    data->allocatedSize = 0;
}


/**
 * free all the data allocated by the IntIntHashMap
 */
void iihm_free(IntIntHashMap* data) {
    iihm_free_content_only(data);
    free(data);
}


/**
 * create a new hash set
 */
IntIntHashMap* iihm_create(int initialSize) {
    IntIntHashMap* data = (IntIntHashMap*) calloc(1, sizeof(IntIntHashMap));
    data->initialSize = initialSize;
    data->allocatedSize = initialSize;
    data->first = calloc(data->initialSize, sizeof(int));
    data->keySet = calloc(data->initialSize, sizeof(int));
    data->valueSet = calloc(data->initialSize, sizeof(int));
    data->next = calloc(data->initialSize, sizeof(int));
    data->size = 0;

    for (int i = 0; i < data->initialSize; i++) {
        data->first[i] = INT_INT_HASHMAP_EMPTY_KEY;
        data->keySet[i] = INT_INT_HASHMAP_EMPTY_KEY;
        data->valueSet[i] = INT_INT_HASHMAP_EMPTY_KEY;
        data->next[i] = INT_INT_HASHMAP_EMPTY_KEY;
    }

    return data;
}

// help insert a value into our map
int iihm_insertHelper(int keySetValue, int valueSetValue, IntIntHashMap* data) {
    if (data == NULL)
        return 0;
    int firstIndex = abs(keySetValue % data->allocatedSize);
    int newSize = data->size;

    // simplest case - we don't have an entry yet
    if (data->first[firstIndex] == INT_INT_HASHMAP_EMPTY_KEY) {
        data->first[firstIndex] = data->size; // first points to the next empty data-slot
        // and the data goes into the slots
        data->keySet[data->size] = keySetValue;
        data->valueSet[data->size] = valueSetValue;
        data->next[data->size] = INT_INT_HASHMAP_EMPTY_KEY;
        newSize += 1;

    } else {
        // chain down the colliding items and find the next empty
        int nextIndex = data->first[firstIndex];
        while (data->next[nextIndex] != INT_INT_HASHMAP_EMPTY_KEY) {
            if (data->valueSet[nextIndex] == valueSetValue) // already exists, not added
                return data->size;
            nextIndex = data->next[nextIndex];
        }
        if (data->keySet[nextIndex] == keySetValue && data->valueSet[nextIndex] == valueSetValue) // already exists, not added
            return data->size;
        // now prevNext points to the last slot that wasn't empty - chain it in
        data->next[nextIndex] = data->size;
        // and put in our data at the end
        data->keySet[data->size] = keySetValue;
        data->valueSet[data->size] = valueSetValue;
        data->next[data->size] = INT_INT_HASHMAP_EMPTY_KEY;
        newSize += 1;
    }
    return newSize;
}


void iih_grow(IntIntHashMap* data) {
    int oldSize = data->allocatedSize;
    int growSize = ((oldSize * 3) / 2) + 1; // 50% growth

    IntIntHashMap* newData = iihm_create(growSize);

    for (int i = 0; i < data->allocatedSize; i++) {
        int oldFirst = data->first[i];
        int oldNextIndex = oldFirst;
        while (oldNextIndex != INT_INT_HASHMAP_EMPTY_KEY) {
            int keySetValue = data->keySet[oldNextIndex];
            int valueSetValue = data->valueSet[oldNextIndex];
            newData->size = iihm_insertHelper(keySetValue, valueSetValue, newData);
            oldNextIndex = data->next[oldNextIndex];
        }

    } // for each existing entry

    // transfer the newly mapped data
    iihm_free_content_only(data);
    data->first = newData->first;
    data->next = newData->next;
    data->keySet = newData->keySet;
    data->valueSet = newData->valueSet;
    data->size = newData->size;
    data->initialSize = newData->initialSize;
    data->allocatedSize = newData->allocatedSize;
    free(newData);
}


/**
 * add a new string into the set
 * @return true if a new item was added, false if the item already existed
 */
int iihm_add(IntIntHashMap* data, int key, int value) {
    if (key == INT_INT_HASHMAP_EMPTY_KEY || data == NULL)
        return 0;

    // do we need to grow our arrays and remap all existing data?
    if (data->size + 1 >= data->allocatedSize) {
        iih_grow(data);
    }

    // get an index into the first array
    int oldSize = data->size;
    data->size = iihm_insertHelper(key, value, data);
    return data->size > oldSize;
}


/**
 * is key inside the map (does it exist)
 */
int iihm_contains(IntIntHashMap* data, int key) {
    if (key == INT_INT_HASHMAP_EMPTY_KEY || data == NULL)
        return 0;
    int firstIndex = abs(key % data->allocatedSize);
    int nextIndex = data->first[firstIndex];
    if (nextIndex == INT_INT_HASHMAP_EMPTY_KEY)
        return 0;
    while (data->next[nextIndex] != INT_INT_HASHMAP_EMPTY_KEY) {
        if (data->keySet[nextIndex] == key)
            return 1;
        nextIndex = data->next[nextIndex];
    }
    return (data->keySet[nextIndex] == key);
}


/**
 * remove a str from the set
 */
int iihm_remove(IntIntHashMap* data, int key) {
    int firstIndex = abs(key % data->allocatedSize);
    int nextIndex = data->first[firstIndex];
    if (nextIndex == INT_INT_HASHMAP_EMPTY_KEY)
        return 0; // nothing to remove
    int prevIndex = nextIndex;
    while (data->next[nextIndex] != INT_INT_HASHMAP_EMPTY_KEY) {
        if (data->keySet[nextIndex] == key)
            break;
        prevIndex = nextIndex;
        nextIndex = data->next[nextIndex];
    }
    // found?
    if (data->keySet[nextIndex] == key) {
        // first update?
        if (nextIndex == data->first[firstIndex]) {
            data->first[firstIndex] = data->next[nextIndex]; // unchain first item
            // does it remove the item completely?
            if (data->next[nextIndex] == INT_INT_HASHMAP_EMPTY_KEY) {
                data->keySet[nextIndex] = INT_INT_HASHMAP_EMPTY_KEY;
                data->valueSet[nextIndex] = INT_INT_HASHMAP_EMPTY_KEY;
            }
        } else {
            data->next[prevIndex] = data->next[nextIndex]; // skip one in the chain
        }
        data->size -= 1;
        return 1;
    }
    return 0; // not found
}
