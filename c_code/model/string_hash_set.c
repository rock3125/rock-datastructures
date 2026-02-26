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
#include <zlib.h>
#include "string_hash_set.h"


/**
 * clear the hash set - remove all data
 */
void str_hashset_clear(StringHashSet* data) {
    if (data == NULL) // not set - just return
        return;
    data->size = 0; // empty data
    // shrink the arrays?
    if (data->allocatedSize > data->initialSize) { // if we've grown beyond the initial size
        // first release the allocated data
        if (data->first != NULL) free(data->first);
        if (data->intHash1 != NULL) free(data->intHash1);
        if (data->intHash2 != NULL) free(data->intHash2);
        if (data->next != NULL) free(data->next);

        // re-allocate the original sizes
        data->first = calloc(data->initialSize, sizeof(int));
        data->intHash1 = calloc(data->initialSize, sizeof(int));
        data->intHash2 = calloc(data->initialSize, sizeof(int));
        data->next = calloc(data->initialSize, sizeof(int));
        data->allocatedSize = data->initialSize;
    }

    // clear the arrays with "empty" keys so they appear as empty to our algorithm
    for (int i = 0; i < data->initialSize; i++) {
        data->first[i] = STRING_HASHMAP_EMPTY_KEY;
        data->intHash1[i] = STRING_HASHMAP_EMPTY_KEY;
        data->intHash2[i] = STRING_HASHMAP_EMPTY_KEY;
        data->next[i] = STRING_HASHMAP_EMPTY_KEY;
    }
}


/**
 * free all the data allocated by the StringHashSet
 */
void str_hashset_free_content_only(StringHashSet* data) {
    if (data == NULL) return; // no data, don't de-allocate
    // de-allocate the arrays first
    if (data->first != NULL) free(data->first);
    if (data->intHash1 != NULL) free(data->intHash1);
    if (data->intHash2 != NULL) free(data->intHash2);
    if (data->next != NULL) free(data->next);
    // set all items in data to NULL and 0
    data->first = NULL;
    data->intHash1 = NULL;
    data->intHash2 = NULL;
    data->next = NULL;
    data->size = 0;
    data->allocatedSize = 0;
}


/**
 * free all the data allocated by the StringHashSet
 */
void str_hashset_free(StringHashSet* data) {
    // free content of map
    str_hashset_free_content_only(data);
    // then free the data itself
    free(data);
}


/**
 * create a new hash set
 */
StringHashSet* str_hashset_create(int initialSize) {
    // allocate the main structure
    StringHashSet* data = (StringHashSet*) calloc(1, sizeof(StringHashSet));
    if (data == NULL) return NULL; // failed?
    // set the initial size
    data->initialSize = initialSize;
    data->allocatedSize = initialSize;
    // allocate the key arrays
    data->first = calloc(data->initialSize, sizeof(int));
    data->intHash1 = calloc(data->initialSize, sizeof(int));
    data->intHash2 = calloc(data->initialSize, sizeof(int));
    data->next = calloc(data->initialSize, sizeof(int));
    // set the map size to 0
    data->size = 0;

    // clear all values inside the arrays as empty
    for (int i = 0; i < data->initialSize; i++) {
        data->first[i] = STRING_HASHMAP_EMPTY_KEY;
        data->intHash1[i] = STRING_HASHMAP_EMPTY_KEY;
        data->intHash2[i] = STRING_HASHMAP_EMPTY_KEY;
        data->next[i] = STRING_HASHMAP_EMPTY_KEY;
    }
    // done - return the new data structure
    return data;
}

// adler 32 hash from libz
int stringToHash1(const char* str) {
    if (str == NULL) return 0; // NULL string has no hash
    // use fast adler32 hash to create a unique int value for our string
    return (int)adler32(1, (const unsigned char*)str, strlen(str));
}

// the java String.hashCode() implementation (similar to Java's own)
int stringToHash2(const char* str) {
    if (str == NULL) return 0; // NULL string has no hash
    // simple hash based on bit shifts and character values
    int h = 0;
    int l = (int)strlen(str);
    int i = 0;
    if ( l > 0 )
        while (i < l)
            h = (h << 5) - h + str[i++] | 0;
    return h;
}

// help insert a value into our map
int insertHelper(int intHash1Value, int intHash2Value, StringHashSet* data) {
    if (data == NULL) return 0; // null data, no insert
    int firstIndex = abs(intHash1Value % data->allocatedSize);
    int newSize = data->size;

    // simplest case - we don't have an entry yet
    if (data->first[firstIndex] == STRING_HASHMAP_EMPTY_KEY) {
        data->first[firstIndex] = data->size; // first points to the next empty data-slot
        // and the data goes into the slots
        data->intHash1[data->size] = intHash1Value;
        data->intHash2[data->size] = intHash2Value;
        data->next[data->size] = STRING_HASHMAP_EMPTY_KEY;
        newSize += 1; // increase map size

    } else {
        // chain down the colliding items and find the next empty
        int nextIndex = data->first[firstIndex];
        while (data->next[nextIndex] != STRING_HASHMAP_EMPTY_KEY) {
            if (data->intHash2[nextIndex] == intHash2Value) // already exists, not added
                return data->size; // return existing size
            nextIndex = data->next[nextIndex]; // next value in the chain
        }
        // did we land on a value that matches our hashes?
        if (data->intHash1[nextIndex] == intHash1Value && data->intHash2[nextIndex] == intHash2Value) // already exists, not added
            return data->size; // no change, value already exists / inserted
        // INSERT: now prevNext points to the last slot that wasn't empty - chain it in
        data->next[nextIndex] = data->size;
        // and put in our data at the end
        data->intHash1[data->size] = intHash1Value;
        data->intHash2[data->size] = intHash2Value;
        // next pointer is empty
        data->next[data->size] = STRING_HASHMAP_EMPTY_KEY;
        newSize += 1;
    }
    // return new size of the map
    return newSize;
}


// grow the map by 50%
void grow(StringHashSet* data) {
    if (data == NULL) return; // NULL map, can't grow
    int oldSize = data->allocatedSize;
    int growSize = ((oldSize * 3) / 2) + 1; // 50% growth

    StringHashSet* newData = str_hashset_create(growSize);
    if (newData == NULL) return; // can't allocate new data

    // copy existing data into the new array
    for (int i = 0; i < data->allocatedSize; i++) {
        int oldFirst = data->first[i];
        int oldNextIndex = oldFirst;
        while (oldNextIndex != STRING_HASHMAP_EMPTY_KEY) {
            int intHash1Value = data->intHash1[oldNextIndex];
            int intHash2Value = data->intHash2[oldNextIndex];
            newData->size = insertHelper(intHash1Value, intHash2Value, newData);
            oldNextIndex = data->next[oldNextIndex];
        }

    } // for each existing entry

    // transfer the newly mapped data
    str_hashset_free_content_only(data);
    // copy newData into data, as this is what is being replaced / grown
    data->first = newData->first;
    data->next = newData->next;
    data->intHash1 = newData->intHash1;
    data->intHash2 = newData->intHash2;
    data->size = newData->size;
    data->initialSize = newData->initialSize;
    data->allocatedSize = newData->allocatedSize;
    free(newData); // de-allocate temporary helper data
}


/**
 * add a new string into the set
 * @return true if a new item was added, false if the item already existed
 */
int str_hashset_add(StringHashSet* data, const char* str) {
    // can't add empty str or into a NULL data
    if (str == NULL || strlen(str) == 0 || data == NULL)
        return 0;

    // do we need to grow our arrays and remap all existing data?
    if (data->size + 1 >= data->allocatedSize) {
        grow(data);
    }

    // get an index into the first array
    int intHash1Value = stringToHash1(str);
    int intHash2Value = stringToHash2(str);
    int oldSize = data->size;
    data->size = insertHelper(intHash1Value, intHash2Value, data);
    return data->size > oldSize;
}


/**
 * is str inside the map (does it exist)
 */
int str_hashset_contains(StringHashSet* data, const char* str) {
    // we can never insert an empty string - or into a NULL data array
    if (str == NULL || strlen(str) == 0 || data == NULL)
        return 0;
    int intHash1Value = stringToHash1(str); // find first index
    int firstIndex = abs((intHash1Value % data->allocatedSize));
    int nextIndex = data->first[firstIndex];
    if (nextIndex == STRING_HASHMAP_EMPTY_KEY) // not set - not found
        return 0;
    int intHash2Value = stringToHash2(str); // check second hash
    while (data->next[nextIndex] != STRING_HASHMAP_EMPTY_KEY) {
        if (data->intHash1[nextIndex] == intHash1Value && data->intHash2[nextIndex] == intHash2Value)
            return 1; // found it!
        nextIndex = data->next[nextIndex];
    }
    // we've found it if the two hashes match @ nextIndex
    return (data->intHash1[nextIndex] == intHash1Value && data->intHash2[nextIndex] == intHash2Value);
}


/**
 * remove a str from the set
 */
int str_hashset_remove(StringHashSet* data, const char* str) {
    // can't remove something from a NULL data structure, or an empty string
    if (data == NULL || str == NULL || strlen(str) == 0) return 0;

    int intHash1Value = stringToHash1(str); // location hash-value
    int firstIndex = abs((intHash1Value % data->allocatedSize)); // to index
    int nextIndex = data->first[firstIndex]; // does it exist?
    if (nextIndex == STRING_HASHMAP_EMPTY_KEY) // dne
        return 0; // nothing to remove
    int intHash2Value = stringToHash2(str); // second verification hash
    int prevIndex = nextIndex;
    while (data->next[nextIndex] != STRING_HASHMAP_EMPTY_KEY) {
        if (data->intHash1[nextIndex] == intHash1Value && data->intHash2[nextIndex] == intHash2Value)
            break; // found it!
        prevIndex = nextIndex;
        nextIndex = data->next[nextIndex];
    }
    // found?
    if (data->intHash1[nextIndex] == intHash1Value && data->intHash2[nextIndex] == intHash2Value) {
        // first update?
        if (nextIndex == data->first[firstIndex]) {
            data->first[firstIndex] = data->next[nextIndex]; // unchain first item
            // does it remove the item completely?
            if (data->next[nextIndex] == STRING_HASHMAP_EMPTY_KEY) {
                data->intHash1[nextIndex] = STRING_HASHMAP_EMPTY_KEY;
                data->intHash2[nextIndex] = STRING_HASHMAP_EMPTY_KEY;
            }
        } else {
                data->next[prevIndex] = data->next[nextIndex]; // skip one in the chain
        }
        data->size -= 1;
        return 1;
    }
    return 0; // not found
}
