#include "unity.h"

#include "mock_libdw1000Spi.h"

// Include c file to test static functions
#include "libdw1000.c"

void testThatGetBitReturnsFirstBitWhenSet() {
  // Fixture
  uint8_t data[] = {0x01};

  // Test
  bool actual = getBit(data, 1, 0);

  // Assert
  TEST_ASSERT_EQUAL(true, actual);
}


void testThatGetBitReturnsFirstBitWhenNotSet() {
  // Fixture
  uint8_t data[] = {0};

  // Test
  bool actual = getBit(data, 1, 0);

  // Assert
  TEST_ASSERT_EQUAL(false, actual);
}


void testThatGetBitReturnsBit7() {
  // Fixture
  uint8_t data[] = {0x80};

  // Test
  bool actual = getBit(data, 1, 7);

  // Assert
  TEST_ASSERT_EQUAL(true, actual);
}


void testThatGetBitReturnsBit8() {
  // Fixture
  uint8_t data[] = {0x00, 0x01};

  // Test
  bool actual = getBit(data, 2, 8);

  // Assert
  TEST_ASSERT_EQUAL(true, actual);
}


void testThatGetBitReturnsBit9() {
  // Fixture
  uint8_t data[] = {0x00, 0x02};

  // Test
  bool actual = getBit(data, 2, 9);

  // Assert
  TEST_ASSERT_EQUAL(true, actual);
}


void testThatSetBitSetsFirstBit() {
  // Fixture
  uint8_t data[] = {0x00};

  // Test
  setBit(data, 1, 0, true);

  // Assert
  TEST_ASSERT_EQUAL(0x01, data[0]);
}


void testThatSetBitResetsFirstBit() {
  // Fixture
  uint8_t data[] = {0xff};

  // Test
  setBit(data, 1, 0, false);

  // Assert
  TEST_ASSERT_EQUAL(0xfe, data[0]);
}


void testThatSetBitSetsBit7() {
  // Fixture
  uint8_t data[] = {0x00};

  // Test
  setBit(data, 1, 7, true);

  // Assert
  TEST_ASSERT_EQUAL(0x80, data[0]);
}


void testThatSetBitSetsBit8() {
  // Fixture
  uint8_t data[] = {0x00, 0x00};

  // Test
  setBit(data, 2, 8, true);

  // Assert
  TEST_ASSERT_EQUAL(0x01, data[1]);
}


void testThatSetBitSetsBit9() {
  // Fixture
  uint8_t data[] = {0x00, 0x00};

  // Test
  setBit(data, 2, 9, true);

  // Assert
  TEST_ASSERT_EQUAL(0x02, data[1]);
}


void testThatwriteValueToBytesWritesOneByte() {
  // Fixture
  uint8_t data[] = {0x00};

  // Test
  writeValueToBytes(data, 11, 1);

  // Assert
  TEST_ASSERT_EQUAL(11, data[0]);
}


void testThatwriteValueToBytesWritesOneByteOnly() {
  // Fixture
  uint8_t data[] = {0x00, 0x00};

  // Test
  writeValueToBytes(data, 0xff, 1);

  // Assert
  TEST_ASSERT_EQUAL(0, data[1]);
}


void testThatwriteValueToBytesWritesFiveBytes() {
  // Fixture
  uint8_t data[] = {0x00, 0x00, 0x00, 0x00, 0x00};

  // Test
  writeValueToBytes(data, 0x01020304, 5);

  // Assert
  uint8_t expected[] = {0x04, 0x03, 0x02, 0x01, 0x00};
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, data, 5);
}


void testThatwriteValueToBytesWritesNegativeNumber() {
  // Fixture
  uint8_t data[] = {0x00, 0x00, 0x00, 0x00, 0x00};

  // Test
  writeValueToBytes(data, -1, 5);

  // Assert
  uint8_t expected[] = {0xff, 0xff, 0xff, 0xff, 0xff};
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, data, 5);
}
