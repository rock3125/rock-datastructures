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
 * based on double hash values
 *
 */
class StringHashSet(private val initialSize: Int) {

    // an array of firsts - the first offsets into hash
    private var first = IntArray(initialSize) { EMPTY_KEY }

    // an array of the long hash values for size-remapping
    private var longHash = LongArray(initialSize) { EMPTY_KEY_LONG }

    // an array of hashes identifying a string
    private var intHash = IntArray(initialSize) { EMPTY_KEY }

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
            longHash = LongArray(initialSize) { EMPTY_KEY_LONG }
            intHash = IntArray(initialSize) { EMPTY_KEY }
            next = IntArray(initialSize) { EMPTY_KEY }
        } else {
            first.fill(EMPTY_KEY)
            longHash.fill(EMPTY_KEY_LONG)
            next.fill(EMPTY_KEY)
            intHash.fill(EMPTY_KEY)
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
        val longHashValue = stringToHashLong(str)
        val intHashValue = stringToHashInt(str)
        val oldSize = size
        size = insertHelper(longHashValue, intHashValue, size, first, longHash, intHash, next)
        return size > oldSize
    }


    /**
     * is str inside the map (does it exist)
     */
    fun contains(str: String): Boolean {
        val longHashValue = stringToHashLong(str)
        val firstIndex = (longHashValue % first.size).absoluteValue.toInt()
        var nextIndex = first[firstIndex]
        if (nextIndex == EMPTY_KEY)
            return false
        val intHashValue = stringToHashInt(str)
        while (next[nextIndex] != EMPTY_KEY) {
            if (longHash[nextIndex] == longHashValue && intHash[nextIndex] == intHashValue)
                return true
            nextIndex = next[nextIndex]
        }
        return (longHash[nextIndex] == longHashValue && intHash[nextIndex] == intHashValue)
    }


    /**
     * grow all the maps 50% and remap all existing data
     */
    private fun grow() {
        val oldSize = first.size
        val growSize = ((oldSize * 3) / 2) + 1 // 50% growth

        val newFirst = IntArray(growSize) { EMPTY_KEY }
        val newLongHash = LongArray(growSize) { EMPTY_KEY_LONG }
        val newIntHash = IntArray(growSize) { EMPTY_KEY }
        val newNext = IntArray(growSize) { EMPTY_KEY }
        var newSize = 0

        for (oldFirst in first) {

            var oldNextIndex = oldFirst
            while (oldNextIndex != EMPTY_KEY) {
                val longHashValue = longHash[oldNextIndex]
                val intHashValue = intHash[oldNextIndex]
                newSize = insertHelper(longHashValue, intHashValue, newSize, newFirst, newLongHash, newIntHash, newNext)
                oldNextIndex = next[oldNextIndex]
            }

        } // for each existing entry

        // transfer the newly mapped data
        longHash = newLongHash
        first = newFirst
        next = newNext
        intHash = newIntHash
        size = newSize
    }


    /**
     * read all the hashes stored under a bucket (all colliding items)
     */
    private fun getHashesFromBucket(firstIndex: Int): List<Int> {
        val hashList = mutableListOf<Int>()
        var nextIndex = first[firstIndex]
        while (nextIndex != EMPTY_KEY) {
            hashList.add(intHash[nextIndex])
            nextIndex = next[nextIndex]
        }
        return hashList
    }


    companion object {
        private val hashCalc = Adler32()
        private const val EMPTY_KEY = -1
        private const val EMPTY_KEY_LONG = -1L

        /**
         * return a long hash for a string that is never equal to EMPTY_KEY_LONG
         * @param str the string to hash
         * @return a value between MIN_LONG and MAX_LONG that is enver EMPTY_KEY_LONG
         */
        fun stringToHashLong(str: String): Long {
            hashCalc.reset()
            hashCalc.update(str.toByteArray())
            val value = hashCalc.value
            return if (value != EMPTY_KEY_LONG) value else 0
        }

        /**
         * return a hash value for a given string as an int that is never EMPTY_KEY
         * @param str the string to return a hash for
         * @return a number between MIN_INT and MAX_INT that is never EMPTY_KEY
         */
        fun stringToHashInt(str: String): Int {
            val strHash = str.hashCode()
            return if (strHash == EMPTY_KEY) 0 else strHash
        }

        /**
         * help insert a value into our data structure
         * @param longHashValue the string's long hash
         * @param intHashValue the string's int hash
         * @param size the number of items in the map presently
         * @param first the map of first indexes pointing into our data structures
         * @param longHash the collection of long hashes
         * @param intHash the collection of int hashes
         * @param next the next pointer array
         * @return the new size if a new value was inserted, or the old size if the value already existed
         */
        private fun insertHelper(
            longHashValue: Long,
            intHashValue: Int,
            size: Int,
            first: IntArray,
            longHash: LongArray,
            intHash: IntArray,
            next: IntArray
        ): Int {
            val firstIndex = (longHashValue % first.size).absoluteValue.toInt()
            var newSize = size

            // simplest case - we don't have an entry yet
            if (first[firstIndex] == EMPTY_KEY) {
                first[firstIndex] = size // first points to the next empty data-slot
                // and the data goes into the slots
                longHash[size] = longHashValue
                intHash[size] = intHashValue
                next[size] = EMPTY_KEY
                newSize += 1

            } else {
                // chain down the colliding items and find the next empty
                var nextIndex = first[firstIndex]
                while (next[nextIndex] != EMPTY_KEY) {
                    if (intHash[nextIndex] == intHashValue) // already exists, not added
                        return size
                    nextIndex = next[nextIndex]
                }
                if (longHash[nextIndex] == longHashValue && intHash[nextIndex] == intHashValue) // already exists, not added
                    return size
                // now prevNext points to the last slot that wasn't empty - chain it in
                next[nextIndex] = size
                // and put in our data at the end
                longHash[size] = longHashValue
                intHash[size] = intHashValue
                next[size] = EMPTY_KEY
                newSize += 1
            }
            return newSize
        }


    }

}

