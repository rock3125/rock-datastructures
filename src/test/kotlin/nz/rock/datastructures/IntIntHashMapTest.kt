package nz.rock.datastructures

import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.Test

class IntIntHashMapTest {

    @Test
    fun testSet1() {
        val map = IntIntHashMap(10)
        assertTrue(map.add(1, 2))
        assertEquals(1, map.size())
        assertTrue(map.contains(1))
        assertEquals(2, map.getValue(1))
    }


    @Test
    fun testSet2() {
        val map = IntIntHashMap(10)
        assertTrue(map.add(1, 2))
        assertTrue(!map.add(1, 3)) // already exists
        assertTrue(!map.add(1, 4))
        assertEquals(1, map.size())
        assertTrue(map.contains(1))
        assertTrue(map.getValue(1) == 4)

        map.clear()
        assertEquals(0, map.size())
        assertTrue(map.isEmpty())
        assertTrue(!map.contains(1))
    }

    @Test
    fun testSet3() {
        val map = IntIntHashMap(10)
        assertTrue(map.add(1, 2))
        assertTrue(map.add(2, 4))
        assertTrue(!map.add(2, 5)) // already exists
        assertEquals(2, map.size())
        assertTrue(map.contains(1))
        assertTrue(map.contains(2))
        assertTrue(map.getValue(1) == 2)
        assertTrue(map.getValue(2) == 5)
    }

    @Test
    fun testSet4() {
        val map = IntIntHashMap(100)
        for (i in 0 until 99) {
            assertTrue(map.add(i, i * 100))
        }
        assertEquals(99, map.size())
        for (i in 0 until 99) {
            assertTrue(map.contains(i), "does not contain $i")
            assertTrue(map.getValue(i) == i * 100, "does not have the right value for $i")
        }
    }


    @Test
    fun testSet5() {
        val map = IntIntHashMap(10)
        for (i in 0 until 99) {
            assertTrue(map.add(i, i * 100))
        }
        assertEquals(99, map.size())
        for (i in 0 until 99) {
            assertTrue(map.contains(i), "does not contain $i")
            assertTrue(map.getValue(i) == i * 100, "does not have the right value for $i")
        }
    }


    @Test
    fun testSet6() {
        val size = 1_000_000
        val map = IntIntHashMap(500_000)
        for (i in 0 until size) {
            assertTrue(map.add(i, i * 100))
        }
        assertEquals(size, map.size())
        for (i in 0 until size) {
            assertTrue(map.contains(i), "does not contain $i")
            assertTrue(map.getValue(i) == i * 100, "does not have the right value for $i")
        }
    }


    @Test
    fun testSet7() {
        val map = IntIntHashMap(100)
        for (i in 0 until 99) {
            assertTrue(map.add(i, i * 100))
        }
        assertEquals(99, map.size())
        for (i in 0 until 99) {
            assertTrue(map.remove(i), "could not remove $i")
        }
        assertEquals(0, map.size())
        assertTrue(map.isEmpty())
    }


}

