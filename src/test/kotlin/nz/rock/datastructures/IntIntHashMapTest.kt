package nz.rock.datastructures

import org.junit.jupiter.api.Assertions
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


}

