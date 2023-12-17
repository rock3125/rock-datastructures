package nz.rock.datastructures

import org.junit.jupiter.api.Assertions.assertEquals
import org.junit.jupiter.api.Assertions.assertTrue
import org.junit.jupiter.api.Test

/**
 * tests
 */
class StringHashSetTest {

    @Test
    fun testSet1() {
        val map = StringHashSet(10)
        assertTrue(map.add("test"))
        assertEquals(1, map.size())
        assertTrue(map.contains("test"))
    }

    @Test
    fun testSet2() {
        val map = StringHashSet(10)
        assertTrue(map.add("test"))
        assertTrue(!map.add("test")) // already exists
        assertTrue(!map.add("test"))
        assertEquals(1, map.size())
        assertTrue(map.contains("test"))

        map.clear()
        assertEquals(0, map.size())
        assertTrue(map.isEmpty())
        assertTrue(!map.contains("test"))
    }

    @Test
    fun testSet3() {
        val map = StringHashSet(10)
        assertTrue(map.add("test"))
        assertTrue(map.add("test1"))
        assertTrue(!map.add("test1")) // already exists
        assertEquals(2, map.size())
        assertTrue(map.contains("test"))
        assertTrue(map.contains("test1"))
    }

    @Test
    fun testSet4() {
        val map = StringHashSet(100)
        for (i in 0 until 99) {
            val str = generateTestString(i)
            assertTrue(map.add(str))
        }
        assertEquals(99, map.size())
        for (i in 0 until 99) {
            val str = generateTestString(i)
            assertTrue(map.contains(str), "does not contain \"$str\"")
        }
    }


    @Test
    fun testSet5() {
        val map = StringHashSet(10)
        for (i in 0 until 99) {
            val str = generateTestString(i)
            assertTrue(map.add(str))
        }
        assertEquals(99, map.size())
        for (i in 0 until 99) {
            val str = generateTestString(i)
            assertTrue(map.contains(str), "does not contain \"$str\"")
        }
    }


    @Test
    fun testSet6() {
        val insertSize = 1_000_000
        val map = StringHashSet(500_000)
        val extensions = ArrayList(listOf("txt", "eml", "mp3", "doc", "docx", "avi", "xls", "xlsx", "ppt"))
        for (i in 1..insertSize) {
            val folder = i / 1000
            val ext = extensions[i % extensions.size]
            val str = "https://dataset.rock.co.nz/gov-docs-$folder/file-$i.$ext"
            val added = map.add(str)
            if (!added) {
                map.add(str)
            }
            assertTrue(added)
        }
        assertEquals(insertSize, map.size())

        for (i in 1..insertSize) {
            val folder = i / 1000
            val ext = extensions[i % extensions.size]
            val str = "https://dataset.rock.co.nz/gov-docs-$folder/file-$i.$ext"
            assertTrue(map.contains(str))
        }

        map.clear()
        assertEquals(0, map.size())
        assertTrue(map.isEmpty())
        assertTrue(!map.contains("test"))
    }


    @Test
    fun testSet7() {
        val map = StringHashSet(100)
        for (i in 0 until 99) {
            val str = generateTestString(i)
            assertTrue(map.add(str))
        }
        assertEquals(99, map.size())
        for (i in 0 until 99) {
            val str = generateTestString(i)
            assertTrue(map.remove(str), "could not remove \"$str\"")
        }
        assertEquals(0, map.size())
        assertTrue(map.isEmpty())
    }


    @Test
    fun testSet8() {
        val map = StringHashSet(100)
        for (i in 0 until 99) {
            val str = generateTestString(i)
            assertTrue(map.add(str))
        }
        assertEquals(99, map.size())
        for (i in 0 until 50) {
            val str = generateTestString(i)
            assertTrue(map.remove(str), "could not remove \"$str\"")
        }
        assertEquals(49, map.size())
        assertTrue(!map.isEmpty())
        for (i in 50 until 99) {
            val str = generateTestString(i)
            assertTrue(map.contains(str), "does not have \"$str\"")
        }
    }


    @Test
    fun testSet9() {
        val map = StringHashSet(10)
        for (i in 0 until 99) {
            val str = generateTestString(i)
            assertTrue(map.add(str))
        }
        assertEquals(99, map.size())
        for (i in 0 until 99) {
            val str = generateTestString(i)
            assertTrue(map.remove(str), "could not remove \"$str\"")
        }
        assertEquals(0, map.size())
        assertTrue(map.isEmpty())
        for (i in 0 until 99) {
            val str = generateTestString(i)
            assertTrue(map.add(str))
        }
        for (i in 0 until 99) {
            val str = generateTestString(i)
            assertTrue(map.contains(str), "does not have \"$str\"")
        }
    }

    /////////////////////////////////////////////////////////////////////////

    // helper generate test string used throughout these tests
    private fun generateTestString(index: Int): String {
        return "https://some.com/test_$index.html"
    }

}

