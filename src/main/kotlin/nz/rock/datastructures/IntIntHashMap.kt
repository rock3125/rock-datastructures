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

package nz.rock.datastructures

import kotlin.math.absoluteValue

class IntIntHashMap(private val initialSize: Int) {

    // an array of firsts - the first offsets into the data map
    private var first = IntArray(initialSize) { EMPTY_KEY }
    // an array of next offsets for collisions
    private var keySet = IntArray(initialSize) { EMPTY_KEY }
    // an array of next offsets for collisions
    private var valueSet = IntArray(initialSize) { EMPTY_KEY }
    // an array of next offsets for collisions
    private var next = IntArray(initialSize) { EMPTY_KEY }

    // how much data we have and where the offset is for the next entry
    private var size = 0

    /**
     * clear the hash set - remove all data
     */
    fun clear() {
        size = 0
        // shrink the arrays?
        if (first.size > initialSize) {
            first = IntArray(initialSize) { EMPTY_KEY }
            keySet = IntArray(initialSize) { EMPTY_KEY }
            valueSet = IntArray(initialSize) { EMPTY_KEY }
            next = IntArray(initialSize) { EMPTY_KEY }
        } else {
            first.fill(EMPTY_KEY)
            keySet.fill(EMPTY_KEY)
            valueSet.fill(EMPTY_KEY)
            next.fill(EMPTY_KEY)
        }
    }


    /**
     * return how many items are in the set
     */
    fun size(): Int {
        return size
    }


    /**
     * return true if the set is empty
     */
    fun isEmpty(): Boolean {
        return size == 0
    }


    /**
     * add an int into the map
     * @return true if a new item was added, false if the item already existed
     */
    fun add(key: Int, value: Int): Boolean {
        if (key == EMPTY_KEY || value == EMPTY_KEY) // we don't allow empty values
            return false
        // do we need to grow our arrays and remap all existing data?
        if (size + 1 >= first.size) {
            grow()
        }
        val oldSize = size
        size = insertHelper(key, value, size, first, next, keySet, valueSet)
        return size > oldSize
    }


    /**
     * is the key int inside the map (does it exist)
     */
    fun contains(key: Int): Boolean {
        val firstIndex = (key % first.size).absoluteValue
        var nextIndex = first[firstIndex]
        if (nextIndex == EMPTY_KEY)
            return false
        while (next[nextIndex] != EMPTY_KEY) {
            if (keySet[nextIndex] == key)
                return true
            nextIndex = next[nextIndex]
        }
        return (keySet[nextIndex] == key)
    }


    /**
     * is the key int inside the map (does it exist)
     */
    fun getValue(key: Int): Int {
        val firstIndex = (key % first.size).absoluteValue
        var nextIndex = first[firstIndex]
        if (nextIndex == EMPTY_KEY)
            return EMPTY_KEY
        while (next[nextIndex] != EMPTY_KEY) {
            if (keySet[nextIndex] == key)
                return valueSet[nextIndex]
            nextIndex = next[nextIndex]
        }
        return if (keySet[nextIndex] == key) valueSet[nextIndex] else EMPTY_KEY
    }


    /**
     * remove a str from the set
     */
    fun remove(key: Int): Boolean {
        val firstIndex = (key % first.size).absoluteValue
        var nextIndex = first[firstIndex]
        if (nextIndex == EMPTY_KEY)
            return false // nothing to remove
        var prevIndex = nextIndex
        while (next[nextIndex] != EMPTY_KEY) {
            if (keySet[nextIndex] == key)
                break
            prevIndex = nextIndex
            nextIndex = next[nextIndex]
        }
        // found?
        if (keySet[nextIndex] == key) {
            // first update?
            if (nextIndex == first[firstIndex]) {
                first[firstIndex] = next[nextIndex] // unchain first item
                // does it remove the item completely?
                if (next[nextIndex] == EMPTY_KEY) {
                    keySet[nextIndex] = EMPTY_KEY
                    valueSet[nextIndex] = EMPTY_KEY
                }
            } else {
                next[prevIndex] = next[nextIndex] // skip one in the chain
            }
            size -= 1
            return true
        }
        return false // not found
    }


    /**
     * grow all the maps 50% and remap all existing data
     */
    private fun grow() {
        val oldSize = first.size
        val growSize = ((oldSize * 3) / 2) + 1 // 50% growth

        val newFirst = IntArray(growSize) { EMPTY_KEY }
        val newKeySet = IntArray(growSize) { EMPTY_KEY }
        val newValueSet = IntArray(growSize) { EMPTY_KEY }
        val newNext = IntArray(growSize) { EMPTY_KEY }
        var newSize = 0

        for (oldFirst in first) {

            var oldNextIndex = oldFirst
            while (oldNextIndex != EMPTY_KEY) {
                val key = keySet[oldNextIndex]
                val value = valueSet[oldNextIndex]
                newSize = insertHelper(
                    key,
                    value,
                    newSize,
                    newFirst,
                    newKeySet,
                    newValueSet,
                    newNext
                )
                oldNextIndex = next[oldNextIndex]
            }

        } // for each existing entry

        // transfer the newly mapped data
        first = newFirst
        next = newNext
        keySet = newKeySet
        valueSet = newValueSet
        size = newSize
    }


    companion object {
        private const val EMPTY_KEY = -1


        /**
         * help insert a value into our data structure
         * @param key the string's first int hash
         * @param value the string's second int hash
         * @param size the number of items in the map presently
         * @param first the map of first indexes pointing into our data structures
         * @param keyData the collection of int hashes for the 1 values
         * @param valueData the collection of int hashes for the 2 values
         * @param next the next pointer array
         * @return the new size if a new value was inserted, or the old size if the value already existed
         */
        private fun insertHelper(
            key: Int,
            value: Int,
            size: Int,
            first: IntArray,
            keyData: IntArray,
            valueData: IntArray,
            next: IntArray
        ): Int {
            val firstIndex = (key % first.size).absoluteValue
            var newSize = size

            // simplest case - we don't have an entry yet
            if (first[firstIndex] == EMPTY_KEY) {
                first[firstIndex] = size // first points to the next empty data-slot
                // and the data goes into the slots
                keyData[size] = key
                valueData[size] = value
                next[size] = EMPTY_KEY
                newSize += 1

            } else {
                // chain down the colliding items and find the next empty
                var nextIndex = first[firstIndex]
                while (next[nextIndex] != EMPTY_KEY) {
                    if (valueData[nextIndex] == value) // already exists, not added
                        return size
                    nextIndex = next[nextIndex]
                }
                if (keyData[nextIndex] == key && valueData[nextIndex] == value) // already exists, not added
                    return size
                // now prevNext points to the last slot that wasn't empty - chain it in
                next[nextIndex] = size
                // and put in our data at the end
                keyData[size] = key
                valueData[size] = value
                next[size] = EMPTY_KEY
                newSize += 1
            }
            return newSize
        }

    }

}

