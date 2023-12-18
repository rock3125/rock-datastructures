package nz.rock.datastructures

import nz.rock.nz.rock.datastructures.IntObjectHashMap
import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.Test

class IntObjectHashMapTest {

    @Test
    fun testSet1() {
        val map = IntObjectHashMap<String>(10)
        assertTrue(map.add(1, "test"))
        assertEquals(1, map.size())
        assertTrue(map.contains(1))
        assertEquals("test", map.getValue(1))
    }


    @Test
    fun testSet2() {
        val map = IntObjectHashMap<String>(10)
        assertTrue(map.add(1, "test"))
        assertTrue(!map.add(1, "test 1")) // already exists
        assertTrue(!map.add(1, "test 2"))
        assertEquals(1, map.size())
        assertTrue(map.contains(1))
        assertTrue(map.getValue(1) == "test 2")

        map.clear()
        assertEquals(0, map.size())
        assertTrue(map.isEmpty())
        assertTrue(!map.contains(1))
    }

    @Test
    fun testSet3() {
        val map = IntObjectHashMap<String>(10)
        assertTrue(map.add(1, "test"))
        assertTrue(map.add(2, "test 1"))
        assertTrue(!map.add(2, "test 2")) // already exists
        assertEquals(2, map.size())
        assertTrue(map.contains(1))
        assertTrue(map.contains(2))
        assertTrue(map.getValue(1) =="test")
        assertTrue(map.getValue(2) == "test 2")
    }

    @Test
    fun testSet4() {
        val map = IntObjectHashMap<String>(100)
        for (i in 0 until 99) {
            val str = generateTestString(i)
            assertTrue(map.add(i, str))
        }
        assertEquals(99, map.size())
        for (i in 0 until 99) {
            val str = generateTestString(i)
            assertTrue(map.contains(i), "does not contain $i")
            assertTrue(map.getValue(i) == str, "does not have the right value for $i ($str)")
        }
    }


    @Test
    fun testSet5() {
        val map = IntObjectHashMap<String>(10)
        for (i in 0 until 99) {
            val str = generateTestString(i)
            assertTrue(map.add(i, str))
        }
        assertEquals(99, map.size())
        for (i in 0 until 99) {
            assertTrue(map.contains(i), "does not contain $i")
            val str = generateTestString(i)
            assertTrue(map.getValue(i) == str, "does not have the right value for $i ($str)")
        }
    }


    @Test
    fun testSet6() {
        val size = 1_000_000
        val map = IntObjectHashMap<String>(500_000)
        for (i in 0 until size) {
            val str = generateTestString(i)
            assertTrue(map.add(i, str))
        }
        assertEquals(size, map.size())
        for (i in 0 until size) {
            assertTrue(map.contains(i), "does not contain $i")
            val str = generateTestString(i)
            assertTrue(map.getValue(i) == str, "does not have the right value for $i ($str)")
        }
    }


    @Test
    fun testSet7() {
        val map = IntObjectHashMap<String>(100)
        for (i in 0 until 99) {
            val str = generateTestString(i)
            assertTrue(map.add(i, str))
        }
        assertEquals(99, map.size())
        for (i in 0 until 99) {
            assertTrue(map.remove(i), "could not remove $i")
        }
        assertEquals(0, map.size())
        assertTrue(map.isEmpty())
    }


    @Test
    fun testSet8() {
        val map = IntObjectHashMap<String>(100)
        for (i in 0 until 99) {
            val str = generateTestString(i)
            assertTrue(map.add(i, str))
        }
        assertEquals(99, map.size())
        for (i in 0 until 50) {
            assertTrue(map.remove(i), "could not remove $i")
        }
        assertEquals(49, map.size())
        assertTrue(!map.isEmpty())
        for (i in 50 until 99) {
            assertTrue(map.contains(i), "does not have $i")
            val str = generateTestString(i)
            assertTrue(map.getValue(i) == str, "does not have $i value ($str)")
        }
    }


    @Test
    fun testSet9() {
        val map = IntObjectHashMap<String>(10)
        for (i in 0 until 99) {
            val str = generateTestString(i)
            assertTrue(map.add(i, str))
        }
        assertEquals(99, map.size())
        for (i in 0 until 99) {
            assertTrue(map.remove(i), "could not remove $i")
        }
        assertEquals(0, map.size())
        assertTrue(map.isEmpty())
        for (i in 0 until 99) {
            val str = generateTestString(i)
            assertTrue(map.add(i, str))
        }
        for (i in 0 until 99) {
            assertTrue(map.contains(i), "does not have $i")
            val str = generateTestString(i)
            assertTrue(map.getValue(i) == str, "does not have $i value")
        }
    }

    /////////////////////////////////////////////////////////////////////////

    // helper generate test string used throughout these tests
    private fun generateTestString(index: Int): String {
        return "https://some.com/test_$index.html"
    }

}

