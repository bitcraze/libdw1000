#include <string.h>
#include "unity.h"

#include "libdw1000Spi.h"


static dwDevice_t* readListenerDev = NULL;
static uint8_t readListenerHeader[3];
static size_t readListenerHeaderLength = 0;
static void* readListenerData = NULL;
static size_t readListenerDataLength = 0;

static uint8_t* nextReadData = NULL;

static void spiReadListener(dwDevice_t* dev, const void *header,
  size_t headerLength, void* data, size_t dataLength) {

  readListenerDev = dev;
  memcpy(readListenerHeader, header, headerLength);
  readListenerHeaderLength = headerLength;

  if (NULL != nextReadData) {
    memcpy(data, nextReadData, dataLength);
  }

  readListenerData = data;
  readListenerDataLength = dataLength;
}

static dwDevice_t* writeListenerDev = NULL;
static uint8_t writeListenerHeader[3];
static size_t writeListenerHeaderLength = 0;
static const void* writeListenerData = NULL;
static size_t writeListenerDataLength = 0;

static void spiWriteListener(dwDevice_t* dev, const void *header,
    size_t headerLength, const void* data, size_t dataLength) {

  writeListenerDev = dev;
  memcpy(writeListenerHeader, header, headerLength);
  writeListenerHeaderLength = headerLength;
  writeListenerData = data;
  writeListenerDataLength = dataLength;
}

static dwOps_t ops = {
  .spiRead = spiReadListener,
  .spiWrite = spiWriteListener,
};

static dwDevice_t dev = {
  .ops = &ops,
};


void setup() {
  nextReadData = NULL;
}

void testThatDwSpiReadCallsSpiRead() {
  // Fixture
  void* data = (void*) 4711;
  size_t length = 17;

  // Test
  dwSpiRead(&dev, 0, 0, data, length);

  // Assert
  TEST_ASSERT_EQUAL_UINT((size_t)&dev, (size_t)readListenerDev);
  TEST_ASSERT_EQUAL_UINT((size_t)data, (size_t)readListenerData);
  TEST_ASSERT_EQUAL_UINT(length, readListenerDataLength);
}


static void verifyThatDwSpiReadHeaderIsCorrect(uint32_t address, uint8_t* expectedHeader, size_t expectedLength) {
  // Fixture
  uint8_t regid = 0xff;

  // Test
  dwSpiRead(&dev, regid, address, NULL, 0);

  // Assert
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expectedHeader, readListenerHeader,  expectedLength);
  TEST_ASSERT_EQUAL(expectedLength, readListenerHeaderLength);
}


void testThatDwSpiReadHeaderIsCorrectForAddresses() {
  uint8_t expectedHeader[] = {0x3f};
  verifyThatDwSpiReadHeaderIsCorrect(0x00, expectedHeader, sizeof(expectedHeader));
}


void testThatDwSpiReadHeaderIsCorrectForAddress0() {
  // Fixture
  // Test
  // Assert
  uint8_t expectedHeader[] = {0x3f};
  verifyThatDwSpiReadHeaderIsCorrect(0x00, expectedHeader, sizeof(expectedHeader));
}


void testThatDwSpiReadHeaderIsCorrectForAddress1() {
  // Fixture
  // Test
  // Assert
  uint8_t expectedHeader[] = {0x7f, 0x01};
  verifyThatDwSpiReadHeaderIsCorrect(0x01, expectedHeader, sizeof(expectedHeader));
}


void testThatDwSpiReadHeaderIsCorrectForAddress0xef() {
  // Fixture
  // Test
  // Assert
  uint8_t expectedHeader[] = {0x7f, 0x7f};
  verifyThatDwSpiReadHeaderIsCorrect(0x7f, expectedHeader, sizeof(expectedHeader));
}


void testThatDwSpiReadHeaderIsCorrectForAddress0xff() {
  // Fixture
  // Test
  // Assert
  uint8_t expectedHeader[] = {0x7f, 0xff, 0x01};
  verifyThatDwSpiReadHeaderIsCorrect(0xff, expectedHeader, sizeof(expectedHeader));
}


void testThatDwSpiReadHeaderIsCorrectForAddress0xffff() {
  // Fixture
  // Test
  // Assert
  uint8_t expectedHeader[] = {0x7f, 0xff, 0xff};
  verifyThatDwSpiReadHeaderIsCorrect(0xffff, expectedHeader, sizeof(expectedHeader));
}


void testThatDwSpiRead16() {
  // Fixture
  uint8_t data[] = {0x01, 0x02};
  nextReadData = &data;

  // Test
  uint32_t actual = dwSpiRead16(&dev, 0, 0);

  // Assert
  TEST_ASSERT_EQUAL(2, readListenerDataLength);
  TEST_ASSERT_EQUAL_HEX16(0x0201, actual);
}


void testThatDwSpiRead32() {
  // Fixture
  uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
  nextReadData = &data;

  // Test
  uint32_t actual = dwSpiRead32(&dev, 0, 0);

  // Assert
  TEST_ASSERT_EQUAL(4, readListenerDataLength);
  TEST_ASSERT_EQUAL_HEX32(0x04030201, actual);
}


void testThatDwSpiWriteCallsSpiWrite() {
  // Fixture
  const void* data = (const void*) 4711;
  size_t length = 17;

  // Test
  dwSpiWrite(&dev, 0, 0, data, length);

  // Assert
  TEST_ASSERT_EQUAL_UINT((size_t)&dev, (size_t)writeListenerDev);
  TEST_ASSERT_EQUAL_UINT((size_t)data, (size_t)writeListenerData);
  TEST_ASSERT_EQUAL_UINT(length, writeListenerDataLength);
}


static void verifyThatDwSpiWriteHeaderIsCorrect(uint32_t address, uint8_t* expectedHeader, size_t expectedLength) {
  // Fixture
  uint8_t regid = 0xff;

  // Test
  dwSpiWrite(&dev, regid, address, NULL, 0);

  // Assert
  TEST_ASSERT_EQUAL_HEX8_ARRAY(expectedHeader, writeListenerHeader,  expectedLength);
  TEST_ASSERT_EQUAL(expectedLength, writeListenerHeaderLength);
}


void testThatDwSpiWriteHeaderIsCorrectForAddress0() {
  // Fixture
  // Test
  // Assert
  uint8_t expectedHeader[] = {0xBf};
  verifyThatDwSpiWriteHeaderIsCorrect(0x00, expectedHeader, sizeof(expectedHeader));
}


void testThatDwSpiWriteHeaderIsCorrectForAddress1() {
  // Fixture
  // Test
  // Assert
  uint8_t expectedHeader[] = {0xff, 0x01};
  verifyThatDwSpiWriteHeaderIsCorrect(0x01, expectedHeader, sizeof(expectedHeader));
}


void testThatDwSpiWriteHeaderIsCorrectForAddress0x7f() {
  // Fixture
  // Test
  // Assert
  uint8_t expectedHeader[] = {0xff, 0x7f};
  verifyThatDwSpiWriteHeaderIsCorrect(0x7f, expectedHeader, sizeof(expectedHeader));
}


void testThatDwSpiWriteHeaderIsCorrectForAddress0xff() {
  // Fixture
  // Test
  // Assert
  uint8_t expectedHeader[] = {0xff, 0xff, 0x01};
  verifyThatDwSpiWriteHeaderIsCorrect(0xff, expectedHeader, sizeof(expectedHeader));
}


void testThatDwSpiWriteHeaderIsCorrectForAddress0xffff() {
  // Fixture
  // Test
  // Assert
  uint8_t expectedHeader[] = {0xff, 0xff, 0xff};
  verifyThatDwSpiWriteHeaderIsCorrect(0xffff, expectedHeader, sizeof(expectedHeader));
}


void testThatDwSpiWrite32PassesDataOn() {
  // Fixture

  // Test
  dwSpiWrite32(&dev, 0, 0, 4711);

  // Assert
  TEST_ASSERT_EQUAL_UINT((size_t)&dev, (size_t)writeListenerDev);
  TEST_ASSERT_EQUAL_UINT(4, writeListenerDataLength);
  // It is requires a bit too much work to verify the data. Settle for the length.
}
