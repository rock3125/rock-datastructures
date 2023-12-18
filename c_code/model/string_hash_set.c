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
#include "string_hash_set.h"


/**
 * clear the hash set - remove all data
 */
void str_hashset_clear(StringHashSet* data) {
    if (data == NULL)
        return;
    data->size = 0;
    // shrink the arrays?
    if (data->allocatedSize > data->initialSize) {
        if (data->first != NULL) free(data->first);
        if (data->intHash1 != NULL) free(data->intHash1);
        if (data->intHash2 != NULL) free(data->intHash2);
        if (data->next != NULL) free(data->next);

        data->first = calloc(data->initialSize, sizeof(int));
        data->intHash1 = calloc(data->initialSize, sizeof(int));
        data->intHash2 = calloc(data->initialSize, sizeof(int));
        data->next = calloc(data->initialSize, sizeof(int));
    }

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
void str_hashset_free(StringHashSet* data) {
    if (data->first != NULL) free(data->first);
    if (data->intHash1 != NULL) free(data->intHash1);
    if (data->intHash2 != NULL) free(data->intHash2);
    if (data->next != NULL) free(data->next);
    data->first = NULL;
    data->intHash1 = NULL;
    data->intHash2 = NULL;
    data->next = NULL;
    data->size = 0;
    data->allocatedSize = 0;
    free(data);
}


/**
 * create a new hash set
 */
StringHashSet* str_hashset_create(int initialSize) {
    StringHashSet* data = (StringHashSet*) calloc(1, sizeof(StringHashSet));
    data->first = calloc(data->initialSize, sizeof(int));
    data->intHash1 = calloc(data->initialSize, sizeof(int));
    data->intHash2 = calloc(data->initialSize, sizeof(int));
    data->next = calloc(data->initialSize, sizeof(int));
    data->size = 0;
    data->initialSize = initialSize;
    data->allocatedSize = initialSize;
    return data;
}


int stringToHash1(const char* str) {
    return 0;
}

int stringToHash2(const char* str) {
    return 0;
}

int insertHelper(int intHash1Value, int intHash2Value, StringHashSet* data) {
    return 0;
}


void grow(StringHashSet* data) {
    int oldSize = data->allocatedSize;
    int growSize = ((oldSize * 3) / 2) + 1; // 50% growth

    StringHashSet* newData = str_hashset_create(growSize);

    for (int i = 0; i < data->allocatedSize; i++) {
        int oldFirst = data->first[i];

        int oldNextIndex = oldFirst;
        while (oldNextIndex != STRING_HASHMAP_EMPTY_KEY) {
            int intHash1Value = data->intHash1[oldNextIndex];
            int intHash2Value = data->intHash2[oldNextIndex];
            insertHelper(intHash1Value, intHash2Value, newData);
            oldNextIndex = data->next[oldNextIndex];
        }

    } // for each existing entry

    // transfer the newly mapped data
    data->first = newData->first;
    data->next = newData->next;
    data->intHash1 = newData->intHash1;
    data->intHash2 = newData->intHash2;
    data->size = newData->size;
    free(newData);
}


/**
 * add a new string into the set
 * @return true if a new item was added, false if the item already existed
 */
int str_hashset_add(StringHashSet* data, const char* str) {
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
    int size = insertHelper(intHash1Value, intHash2Value, data);
    return size > oldSize;
}


/**
 * is str inside the map (does it exist)
 */
int str_hashset_contains(StringHashSet* data, const char* str) {
    if (str == NULL || strlen(str) == 0)
        return 0;
    int intHash1Value = stringToHash1(str);
    int firstIndex = abs((intHash1Value % data->allocatedSize));
    int nextIndex = data->first[firstIndex];
    if (nextIndex == STRING_HASHMAP_EMPTY_KEY)
        return 0;
    int intHash2Value = stringToHash2(str);
    while (data->next[nextIndex] != STRING_HASHMAP_EMPTY_KEY) {
        if (data->intHash1[nextIndex] == intHash1Value && data->intHash2[nextIndex] == intHash2Value)
            return 1;
        nextIndex = data->next[nextIndex];
    }
    return (data->intHash1[nextIndex] == intHash1Value && data->intHash2[nextIndex] == intHash2Value);
}


/**
 * remove a str from the set
 */
int str_hashset_remove(StringHashSet* data, const char* str) {
    int intHash1Value = stringToHash1(str);
    int firstIndex = abs((intHash1Value % data->allocatedSize));
    int nextIndex = data->first[firstIndex];
    if (nextIndex == STRING_HASHMAP_EMPTY_KEY)
        return 0; // nothing to remove
    int intHash2Value = stringToHash2(str);
    int prevIndex = nextIndex;
    while (data->next[nextIndex] != STRING_HASHMAP_EMPTY_KEY) {
        if (data->intHash1[nextIndex] == intHash1Value && data->intHash2[nextIndex] == intHash2Value)
            break;
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
