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

import java.util.zip.Adler32
import kotlin.math.absoluteValue

/**
 * a memory efficient mostly accurate String hash set
 * checking for the presence / existence of a string
 * based on double hash values 0 allocating 20 bytes per string total
 *
 */
class StringHashSet(private val initialSize: Int) {

    // an array of firsts - the first offsets into hash
    private var first = IntArray(initialSize) { EMPTY_KEY }

    // an array of type 1 hashes identifying a string
    private var intHash1 = IntArray(initialSize) { EMPTY_KEY }

    // an array of type 2 hashes identifying a string
    private var intHash2 = IntArray(initialSize) { EMPTY_KEY }

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
            intHash1 = IntArray(initialSize) { EMPTY_KEY }
            intHash2 = IntArray(initialSize) { EMPTY_KEY }
            next = IntArray(initialSize) { EMPTY_KEY }
        } else {
            first.fill(EMPTY_KEY)
            intHash2.fill(EMPTY_KEY)
            next.fill(EMPTY_KEY)
            intHash1.fill(EMPTY_KEY)
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
     * add a new string into the set
     * @return true if a new item was added, false if the item already existed
     */
    fun add(str: String): Boolean {
        if (str.isEmpty()) // we don't allow empty values
            return false
        // do we need to grow our arrays and remap all existing data?
        if (size + 1 >= first.size) {
            grow()
        }

        // get an index into the first array
        val intHash1Value = stringToHash1(str)
        val intHash2Value = stringToHash2(str)
        val oldSize = size
        size = insertHelper(intHash1Value, intHash2Value, size, first, intHash1, intHash2, next)
        return size > oldSize
    }


    /**
     * is str inside the map (does it exist)
     */
    fun contains(str: String): Boolean {
        val intHash1Value = stringToHash1(str)
        val firstIndex = (intHash1Value % first.size).absoluteValue
        var nextIndex = first[firstIndex]
        if (nextIndex == EMPTY_KEY)
            return false
        val intHash2Value = stringToHash2(str)
        while (next[nextIndex] != EMPTY_KEY) {
            if (intHash1[nextIndex] == intHash1Value && intHash2[nextIndex] == intHash2Value)
                return true
            nextIndex = next[nextIndex]
        }
        return (intHash1[nextIndex] == intHash1Value && intHash2[nextIndex] == intHash2Value)
    }


    /**
     * remove a str from the set
     */
    fun remove(str: String): Boolean {
        val intHash1Value = stringToHash1(str)
        val firstIndex = (intHash1Value % first.size).absoluteValue
        var nextIndex = first[firstIndex]
        if (nextIndex == EMPTY_KEY)
            return false // nothing to remove
        val intHash2Value = stringToHash2(str)
        var prevIndex = nextIndex
        while (next[nextIndex] != EMPTY_KEY) {
            if (intHash1[nextIndex] == intHash1Value && intHash2[nextIndex] == intHash2Value)
                break
            prevIndex = nextIndex
            nextIndex = next[nextIndex]
        }
        // found?
        if (intHash1[nextIndex] == intHash1Value && intHash2[nextIndex] == intHash2Value) {
            // first update?
            if (nextIndex == first[firstIndex]) {
                first[firstIndex] = next[nextIndex] // unchain first item
                // does it remove the item completely?
                if (next[nextIndex] == EMPTY_KEY) {
                    intHash1[nextIndex] = EMPTY_KEY
                    intHash2[nextIndex] = EMPTY_KEY
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
        val newHash1Array = IntArray(growSize) { EMPTY_KEY }
        val newHash2Array = IntArray(growSize) { EMPTY_KEY }
        val newNext = IntArray(growSize) { EMPTY_KEY }
        var newSize = 0

        for (oldFirst in first) {

            var oldNextIndex = oldFirst
            while (oldNextIndex != EMPTY_KEY) {
                val intHash1Value = intHash1[oldNextIndex]
                val intHash2Value = intHash2[oldNextIndex]
                newSize = insertHelper(intHash1Value, intHash2Value, newSize, newFirst, newHash1Array, newHash2Array, newNext)
                oldNextIndex = next[oldNextIndex]
            }

        } // for each existing entry

        // transfer the newly mapped data
        first = newFirst
        next = newNext
        intHash1 = newHash1Array
        intHash2 = newHash2Array
        size = newSize
    }


    companion object {
        private val hashCalc = Adler32()
        private const val EMPTY_KEY = -1

        /**
         * return a long hash for a string that is never equal to EMPTY_KEY
         * @param str the string to hash
         * @return a value between MIN_LONG and MAX_LONG that is enver EMPTY_KEY
         */
        fun stringToHash1(str: String): Int {
            hashCalc.reset()
            hashCalc.update(str.toByteArray())
            val value = hashCalc.value.toInt()
            return if (value != EMPTY_KEY) value else 0
        }

        /**
         * return a hash value for a given string as an int that is never EMPTY_KEY
         * @param str the string to return a hash for
         * @return a number between MIN_INT and MAX_INT that is never EMPTY_KEY
         */
        fun stringToHash2(str: String): Int {
            val strHash = str.hashCode()
            return if (strHash == EMPTY_KEY) 0 else strHash
        }

        /**
         * help insert a value into our data structure
         * @param intHash1Value the string's first int hash
         * @param intHash2Value the string's second int hash
         * @param size the number of items in the map presently
         * @param first the map of first indexes pointing into our data structures
         * @param intHash1Data the collection of int hashes for the 1 values
         * @param intHash2Data the collection of int hashes for the 2 values
         * @param next the next pointer array
         * @return the new size if a new value was inserted, or the old size if the value already existed
         */
        private fun insertHelper(
            intHash1Value: Int,
            intHash2Value: Int,
            size: Int,
            first: IntArray,
            intHash1Data: IntArray,
            intHash2Data: IntArray,
            next: IntArray
        ): Int {
            val firstIndex = (intHash1Value % first.size).absoluteValue
            var newSize = size

            // simplest case - we don't have an entry yet
            if (first[firstIndex] == EMPTY_KEY) {
                first[firstIndex] = size // first points to the next empty data-slot
                // and the data goes into the slots
                intHash1Data[size] = intHash1Value
                intHash2Data[size] = intHash2Value
                next[size] = EMPTY_KEY
                newSize += 1

            } else {
                // chain down the colliding items and find the next empty
                var nextIndex = first[firstIndex]
                while (next[nextIndex] != EMPTY_KEY) {
                    if (intHash2Data[nextIndex] == intHash2Value) // already exists, not added
                        return size
                    nextIndex = next[nextIndex]
                }
                if (intHash1Data[nextIndex] == intHash1Value && intHash2Data[nextIndex] == intHash2Value) // already exists, not added
                    return size
                // now prevNext points to the last slot that wasn't empty - chain it in
                next[nextIndex] = size
                // and put in our data at the end
                intHash1Data[size] = intHash1Value
                intHash2Data[size] = intHash2Value
                next[size] = EMPTY_KEY
                newSize += 1
            }
            return newSize
        }


    }

}

