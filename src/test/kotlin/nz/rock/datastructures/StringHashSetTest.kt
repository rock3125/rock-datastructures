package nz.rock.datastructures

import org.junit.jupiter.api.Assertions
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
        Assertions.assertEquals(1, map.size())
        assertTrue(map.contains("test"))
    }

    @Test
    fun testSet2() {
        val map = StringHashSet(10)
        assertTrue(map.add("test"))
        assertTrue(!map.add("test")) // already exists
        assertTrue(!map.add("test"))
        Assertions.assertEquals(1, map.size())
        assertTrue(map.contains("test"))

        map.clear()
        Assertions.assertEquals(0, map.size())
        assertTrue(map.isEmpty())
        assertTrue(!map.contains("test"))
    }

    @Test
    fun testSet3() {
        val map = StringHashSet(10)
        assertTrue(map.add("test"))
        assertTrue(map.add("test1"))
        assertTrue(!map.add("test1")) // already exists
        Assertions.assertEquals(2, map.size())
        assertTrue(map.contains("test"))
        assertTrue(map.contains("test1"))
    }

    @Test
    fun testSet4() {
        val map = StringHashSet(100)
        for (i in 0 until 99) {
            val str = "some test $i"
            assertTrue(map.add(str))
        }
        Assertions.assertEquals(99, map.size())
        for (i in 0 until 99) {
            val str = "some test $i"
            assertTrue(map.contains(str), "does not contain \"$str\"")
        }
    }


    @Test
    fun testSet5() {
        val map = StringHashSet(10)
        for (i in 0 until 99) {
            val str = "some test $i"
            assertTrue(map.add(str))
        }
        Assertions.assertEquals(99, map.size())
        for (i in 0 until 99) {
            val str = "some test $i"
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
        Assertions.assertEquals(insertSize, map.size())

        for (i in 1..insertSize) {
            val folder = i / 1000
            val ext = extensions[i % extensions.size]
            val str = "https://dataset.rock.co.nz/gov-docs-$folder/file-$i.$ext"
            assertTrue(map.contains(str))
        }

        map.clear()
        Assertions.assertEquals(0, map.size())
        assertTrue(map.isEmpty())
        assertTrue(!map.contains("test"))
    }


}

