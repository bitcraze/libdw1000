#include <string.h>
#include "unity.h"
#include "libdw1000.h"

#include "mock_libdw1000Spi.h"
#include "mock_dwTestOps.h"

// Set up opertions with mocks
static dwOps_t ops = {
  .spiRead = spiRead,
  .spiWrite = spiWrite,
  .spiSetSpeed = spiSetSpeed,
  .delayms = delayms,
  .reset = reset
};

static dwDevice_t dev = {
  .ops = &ops,
};

typedef struct dwSpiReadExpectation_s {
  dwDevice_t* dev;
  uint8_t regid;
  uint32_t address;
  uint8_t* data;
  size_t length;
  struct dwSpiReadExpectation_s* next;
} dwSpiReadExpectation_t;

static dwSpiReadExpectation_t* nextReadExpectation = NULL;
static void dwSpiRead_executor(dwDevice_t* dev, uint8_t regid, uint32_t address, void* data, size_t length, int cmock_num_calls);
static void dwSpiRead_addExpectation(dwSpiReadExpectation_t* readExpectation);
#define dwSpiWrite_ExpectAndVerify(dev, regid, address, data) dwSpiWrite_ExpectWithArray(dev, 1, regid, address, data, sizeof(data), sizeof(data))



// TODO krri dwConfigure()
// TODO krri dwManageLDE()

void testThatDeviceIdIsRead() {
  // Fixture
  uint32_t expected = 4711;
  dwSpiRead32_ExpectAndReturn(&dev, DEV_ID, 0, expected);

  // Test
  uint32_t actual = dwGetDeviceId(&dev);

  // Assert
  TEST_ASSERT_EQUAL(expected, actual);
}


void testDwEnableClockAuto() {
  // Fixture
  dwSpiRead_StubWithCallback(dwSpiRead_executor);

  uint8_t initial[LEN_PMSC_CTRL0] = {0xFE, 0xFF, 0xFD, 0xFC};
  dwSpiReadExpectation_t readExpectation = {&dev, PMSC, PMSC_CTRL0_SUB, initial, sizeof(initial), NULL};
  dwSpiRead_addExpectation(&readExpectation);

  spiSetSpeed_Expect(&dev, dwSpiSpeedLow);

  uint8_t write1[] = {0x00};
  dwSpiWrite_ExpectAndVerify(&dev, PMSC, PMSC_CTRL0_SUB, write1);

  uint8_t write2[LEN_PMSC_CTRL0] = {0x00, 0xFE, 0xFD, 0xFC};
  dwSpiWrite_ExpectAndVerify(&dev, PMSC, PMSC_CTRL0_SUB, write2);

  // Test
  dwEnableClock(&dev, dwClockAuto);

  // Assert
}


void testDwEnableClockdwClockXti() {
  // Fixture
  dwSpiRead_StubWithCallback(dwSpiRead_executor);

  uint8_t initial[LEN_PMSC_CTRL0] = {0xFE, 0xFF, 0xFD, 0xFC};
  dwSpiReadExpectation_t readExpectation = {&dev, PMSC, PMSC_CTRL0_SUB, initial, sizeof(initial), NULL};
  dwSpiRead_addExpectation(&readExpectation);

  spiSetSpeed_Expect(&dev, dwSpiSpeedLow);

  uint8_t write1[] = {0xFD};
  dwSpiWrite_ExpectAndVerify(&dev, PMSC, PMSC_CTRL0_SUB, write1);

  uint8_t write2[LEN_PMSC_CTRL0] = {0xFD, 0xFF, 0xFD, 0xFC};
  dwSpiWrite_ExpectAndVerify(&dev, PMSC, PMSC_CTRL0_SUB, write2);

  // Test
  dwEnableClock(&dev, dwClockXti);

  // Assert
}


void testDwEnableClockdwClockPll() {
  // Fixture
  dwSpiRead_StubWithCallback(dwSpiRead_executor);

  uint8_t initial[LEN_PMSC_CTRL0] = {0xFE, 0xFF, 0xFD, 0xFC};
  dwSpiReadExpectation_t readExpectation = {&dev, PMSC, PMSC_CTRL0_SUB, initial, sizeof(initial), NULL};
  dwSpiRead_addExpectation(&readExpectation);

  spiSetSpeed_Expect(&dev, dwSpiSpeedHigh);

  uint8_t write1[] = {0xFE};
  dwSpiWrite_ExpectAndVerify(&dev, PMSC, PMSC_CTRL0_SUB, write1);

  uint8_t write2[LEN_PMSC_CTRL0] = {0xFE, 0xFF, 0xFD, 0xFC};
  dwSpiWrite_ExpectAndVerify(&dev, PMSC, PMSC_CTRL0_SUB, write2);

  // Test
  dwEnableClock(&dev, dwClockPll);

  // Assert
}


void testDwSoftReset() {
  // Fixture
  dwSpiRead_StubWithCallback(dwSpiRead_executor);

  uint8_t initial[LEN_PMSC_CTRL0] = {0xFF, 0xFF, 0xFF, 0xFF};
  dwSpiReadExpectation_t readExpectation = {&dev, PMSC, PMSC_CTRL0_SUB, initial, sizeof(initial), NULL};
  dwSpiRead_addExpectation(&readExpectation);

  uint8_t write1[LEN_PMSC_CTRL0] = {0x01, 0xFF, 0xFF, 0xFF};
  dwSpiWrite_ExpectAndVerify(&dev, PMSC, PMSC_CTRL0_SUB, write1);

  uint8_t write2[LEN_PMSC_CTRL0] = {0x01, 0xFF, 0xFF, 0x00};
  dwSpiWrite_ExpectAndVerify(&dev, PMSC, PMSC_CTRL0_SUB, write2);

  delayms_Expect(&dev, 10);

  uint8_t write3[LEN_PMSC_CTRL0] = {0x00, 0xFF, 0xFF, 0xF0};
  dwSpiWrite_ExpectAndVerify(&dev, PMSC, PMSC_CTRL0_SUB, write3);

  // dwIdle()
  uint8_t idle[LEN_SYS_CTRL] = {0x40, 0x00, 0x00, 0x00};
  dwSpiWrite_ExpectAndVerify(&dev, SYS_CTRL, NO_SUB, idle);

  // Test
  dwSoftReset(&dev);

  // Assert
}


static void verifyGdwGetFirstPathPower(uint16_t fpAmpl1, uint16_t fpAmpl2,
  uint16_t fpAmpl3, uint8_t* rxFrameInfo, uint8_t pulseFrequency, float expected);

void testGdwGetFirstPathPowerAt16MHzLowPower() {
  // Fixture
  uint16_t fpAmpl1 = 0x6251;
  uint16_t fpAmpl2 = 0x8473;
  uint16_t fpAmpl3 = 0xa695;
  uint8_t rxFrameInfo[LEN_RX_FINFO] = {0x00, 0x00, 0x01, 0xb0};

  // Test
  // Assert
  verifyGdwGetFirstPathPower(fpAmpl1, fpAmpl2, fpAmpl3,
    rxFrameInfo, TX_PULSE_FREQ_16MHZ, -85.324951);
}

void testGdwGetFirstPathPowerAt16MHzHighPower() {
  // Fixture
  uint16_t fpAmpl1 = 0xe859;
  uint16_t fpAmpl2 = 0xb677;
  uint16_t fpAmpl3 = 0xd495;
  uint8_t rxFrameInfo[LEN_RX_FINFO] = {0x00, 0x00, 0x03, 0x40};

  // Test
  // Assert
  verifyGdwGetFirstPathPower(fpAmpl1, fpAmpl2, fpAmpl3,
    rxFrameInfo, TX_PULSE_FREQ_16MHZ, -43.299194);
}

void testGdwGetFirstPathPowerAt64MHzHighPower() {
  // Fixture
  uint16_t fpAmpl1 = 0xe859;
  uint16_t fpAmpl2 = 0xb677;
  uint16_t fpAmpl3 = 0xd495;
  uint8_t rxFrameInfo[LEN_RX_FINFO] = {0x00, 0x00, 0x03, 0x40};

  // Test
  // Assert
  verifyGdwGetFirstPathPower(fpAmpl1, fpAmpl2, fpAmpl3,
    rxFrameInfo, TX_PULSE_FREQ_64MHZ, -76.213211);
}


static void verifyGetReceivePower(uint16_t cirPwr, uint8_t* rxFrameInfo, uint8_t pulseFrequency, float expected);

void testGetReceivePowerAt16MHzLowPower() {
  // Fixture
  uint16_t cirPwr = 0x7e81;
  uint8_t rxFrameInfo[LEN_RX_FINFO] = {0x00, 0x00, 0x02, 0xb0};

  // Test
  // Assert
  verifyGetReceivePower(cirPwr, rxFrameInfo, TX_PULSE_FREQ_16MHZ, -82.946953);
}

void testGetReceivePowerAt16MHzHighPower() {
  // Fixture
  uint16_t cirPwr = 0x9173;
  uint8_t rxFrameInfo[LEN_RX_FINFO] = {0x00, 0x00, 0x02, 0x40};

  // Test
  // Assert
  verifyGetReceivePower(cirPwr, rxFrameInfo, TX_PULSE_FREQ_16MHZ, -51.637222);
}

void testGetReceivePowerAt64MHzLowPower() {
  // Fixture
  uint16_t cirPwr = 0x9173;
  uint8_t rxFrameInfo[LEN_RX_FINFO] = {0x00, 0x00, 0x02, 0x40};

  // Test
  // Assert
  verifyGetReceivePower(cirPwr, rxFrameInfo, TX_PULSE_FREQ_64MHZ, -81.632904);
}




// TODO krri dwEnableAllLeds()
// TODO krri dwIdle()
// TODO krri dwNewReceive()
// TODO krri dwStartReceive()
// TODO krri dwNewTransmit()
// TODO krri dwStartTransmit()
// TODO krri dwNewConfiguration()
// TODO krri dwCommitConfiguration()
// TODO krri dwWaitForResponse()
// TODO krri dwUseSmartPower()
// TODO krri dwSetDelay()
// TODO krri dwSetDataRate()
// TODO krri dwSetPulseFrequency()
// TODO krri dwSetPreambleLength()
// TODO krri dwUseExtendedFrameLength()
// TODO krri dwReceivePermanently()
// TODO krri dwSetChannel()
// TODO krri dwSetPreambleCode()
// TODO krri dwSetDefaults()
// TODO krri dwSetData()
// TODO krri dwGetDataLength()
// TODO krri dwGetData()
// TODO krri dwGetReceiveTimestamp()
// TODO krri dwCorrectTimestamp()
// TODO krri dwIsReceiveDone()
// TODO krri dwIsReceiveFailed()
// TODO krri dwIsClockProblem()
// TODO krri dwGetReceiveQuality()
// TODO krri dwGetFirstPathPower()
// TODO krri dwEnableMode()
// TODO krri dwTune()
// TODO krri dwHandleInterrupt()
// TODO krri dwStrError()


/*****************************/

// A mock implementation of dwSpiRead. Supports setting up multiple
// expectations. All arguments are validated against the expectation except
// data that is written to the data pointer.

static void dwSpiRead_addExpectation(dwSpiReadExpectation_t* readExpectation) {
  if (NULL == nextReadExpectation) {
    nextReadExpectation = readExpectation;
  } else {
    dwSpiReadExpectation_t* p = nextReadExpectation;
    while (NULL != p->next) {
      p = p->next;
    };
    p->next = readExpectation;
  }

  readExpectation->next = NULL;
}

static void dwSpiRead_executor(dwDevice_t* dev, uint8_t regid, uint32_t address, void* data, size_t length, int cmock_num_calls) {
  cmock_num_calls++; // Dummy line to keep compiler happy

  TEST_ASSERT_NOT_NULL_MESSAGE(nextReadExpectation, "Out of read expectations!");
  TEST_ASSERT_EQUAL_MESSAGE((size_t)dev, (size_t)nextReadExpectation->dev, "dev is different");
  TEST_ASSERT_EQUAL_UINT8_MESSAGE(regid, nextReadExpectation->regid, "regid is different");
  TEST_ASSERT_EQUAL_UINT32_MESSAGE(address, nextReadExpectation->address, "address is different");
  TEST_ASSERT_EQUAL_UINT32_MESSAGE(length, nextReadExpectation->length, "length is different");
  memcpy(data, nextReadExpectation->data, length);

  nextReadExpectation = nextReadExpectation->next;
}

/*****************************/

static void verifyGdwGetFirstPathPower(uint16_t fpAmpl1, uint16_t fpAmpl2,
    uint16_t fpAmpl3, uint8_t* rxFrameInfo, uint8_t pulseFrequency, float expected) {

  // Fixture
  dwSpiRead_StubWithCallback(dwSpiRead_executor);

  dwSpiRead16_ExpectAndReturn(&dev, RX_TIME, FP_AMPL1_SUB, fpAmpl1);
  dwSpiRead16_ExpectAndReturn(&dev, RX_FQUAL, FP_AMPL2_SUB, fpAmpl2);
  dwSpiRead16_ExpectAndReturn(&dev, RX_FQUAL, FP_AMPL3_SUB, fpAmpl3);

  dwSpiReadExpectation_t readExpectation4 = {&dev, RX_FINFO, NO_SUB, rxFrameInfo, LEN_RX_FINFO, NULL};
  dwSpiRead_addExpectation(&readExpectation4);

  dev.pulseFrequency = pulseFrequency;

  // Test
  float actual = dwGetFirstPathPower(&dev);

  // Assert
  TEST_ASSERT_FLOAT_WITHIN(0.00002, expected, actual);
}


static void verifyGetReceivePower(uint16_t cirPwr, uint8_t* rxFrameInfo, uint8_t pulseFrequency, float expected) {
  // Fixture
  dwSpiRead_StubWithCallback(dwSpiRead_executor);

  dwSpiRead16_ExpectAndReturn(&dev, RX_FQUAL, CIR_PWR_SUB, cirPwr);

  dwSpiReadExpectation_t readExpectation2 = {&dev, RX_FINFO, NO_SUB, rxFrameInfo, LEN_RX_FINFO, NULL};
  dwSpiRead_addExpectation(&readExpectation2);

  dev.pulseFrequency = pulseFrequency;

  // Test
  float actual = dwGetReceivePower(&dev);

  // Assert
  TEST_ASSERT_FLOAT_WITHIN(0.00003, expected, actual);
}
