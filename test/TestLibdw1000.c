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


static void verifyGdwGetFirstPathPower(uint8_t* fpAmpl1Bytes, uint8_t* fpAmpl2Bytes,
    uint8_t* fpAmpl3Bytes, uint8_t* rxFrameInfo, uint8_t pulseFrequency, float expected);

void testGdwGetFirstPathPowerAt16MHzLowPower() {
  // Fixture
  uint8_t fpAmpl1Bytes[LEN_FP_AMPL1] = {0x50, 0x60};
  uint8_t fpAmpl2Bytes[LEN_FP_AMPL2] = {0x70, 0x80};
  uint8_t fpAmpl3Bytes[LEN_FP_AMPL3] = {0x90, 0xa0};
  uint8_t rxFrameInfo[LEN_RX_FINFO] = {0x00, 0x00, 0x00, 0xb0};

  // Test
  // Assert
  verifyGdwGetFirstPathPower(fpAmpl1Bytes, fpAmpl2Bytes, fpAmpl3Bytes,
    rxFrameInfo, TX_PULSE_FREQ_16MHZ, -89.425346);
}

void testGdwGetFirstPathPowerAt16MHzHighPower() {
  // Fixture
  uint8_t fpAmpl1Bytes[LEN_FP_AMPL1] = {0x50, 0x90};
  uint8_t fpAmpl2Bytes[LEN_FP_AMPL2] = {0x70, 0xb0};
  uint8_t fpAmpl3Bytes[LEN_FP_AMPL3] = {0x90, 0xd0};
  uint8_t rxFrameInfo[LEN_RX_FINFO] = {0x00, 0x00, 0x00, 0xb0};

  // Test
  // Assert
  verifyGdwGetFirstPathPower(fpAmpl1Bytes, fpAmpl2Bytes, fpAmpl3Bytes,
    rxFrameInfo, TX_PULSE_FREQ_16MHZ, -83.832870);
}

void testGdwGetFirstPathPowerAt64MHzHighPower() {
  // Fixture
  uint8_t fpAmpl1Bytes[LEN_FP_AMPL1] = {0x50, 0x90};
  uint8_t fpAmpl2Bytes[LEN_FP_AMPL2] = {0x70, 0xb0};
  uint8_t fpAmpl3Bytes[LEN_FP_AMPL3] = {0x90, 0xd0};
  uint8_t rxFrameInfo[LEN_RX_FINFO] = {0x00, 0x00, 0x00, 0xb0};

  // Test
  // Assert
  verifyGdwGetFirstPathPower(fpAmpl1Bytes, fpAmpl2Bytes, fpAmpl3Bytes,
    rxFrameInfo, TX_PULSE_FREQ_64MHZ, -92.769882);
}


static void verifyGetReceivePower(uint8_t* cirPwrBytes, uint8_t* rxFrameInfo, uint8_t pulseFrequency, float expected);

void testGetReceivePowerAt16MHzLowPower() {
  // Fixture
  uint8_t cirPwrBytes[LEN_CIR_PWR] = {0x80, 0x80};
  uint8_t rxFrameInfo[LEN_RX_FINFO] = {0x00, 0x00, 0x00, 0xb0};

  // Test
  // Assert
  verifyGetReceivePower(cirPwrBytes, rxFrameInfo, TX_PULSE_FREQ_16MHZ, -88.366127);
}

void testGetReceivePowerAt16MHzHighPower() {
  // Fixture
  uint8_t cirPwrBytes[LEN_CIR_PWR] = {0x80, 0x80};
  uint8_t rxFrameInfo[LEN_RX_FINFO] = {0x00, 0x00, 0x00, 0x40};

  // Test
  // Assert
  verifyGetReceivePower(cirPwrBytes, rxFrameInfo, TX_PULSE_FREQ_16MHZ, -59.931000);
}

void testGetReceivePowerAt64MHzLowPower() {
  // Fixture
  uint8_t cirPwrBytes[LEN_CIR_PWR] = {0x80, 0x80};
  uint8_t rxFrameInfo[LEN_RX_FINFO] = {0x00, 0x00, 0x00, 0x40};

  // Test
  // Assert
  verifyGetReceivePower(cirPwrBytes, rxFrameInfo, TX_PULSE_FREQ_64MHZ, -82.798759);
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

static void verifyGdwGetFirstPathPower(uint8_t* fpAmpl1Bytes, uint8_t* fpAmpl2Bytes,
    uint8_t* fpAmpl3Bytes, uint8_t* rxFrameInfo, uint8_t pulseFrequency, float expected) {
  // Fixture
  dwSpiRead_StubWithCallback(dwSpiRead_executor);

  dwSpiReadExpectation_t readExpectation1 = {&dev, RX_TIME, FP_AMPL1_SUB, fpAmpl1Bytes, LEN_FP_AMPL1, NULL};
  dwSpiRead_addExpectation(&readExpectation1);

  dwSpiReadExpectation_t readExpectation2 = {&dev, RX_FQUAL, FP_AMPL2_SUB, fpAmpl2Bytes, LEN_FP_AMPL2, NULL};
  dwSpiRead_addExpectation(&readExpectation2);

  dwSpiReadExpectation_t readExpectation3 = {&dev, RX_FQUAL, FP_AMPL3_SUB, fpAmpl3Bytes, LEN_FP_AMPL3, NULL};
  dwSpiRead_addExpectation(&readExpectation3);

  dwSpiReadExpectation_t readExpectation4 = {&dev, RX_FINFO, NO_SUB, rxFrameInfo, LEN_RX_FINFO, NULL};
  dwSpiRead_addExpectation(&readExpectation4);

  dev.pulseFrequency = pulseFrequency;

  // Test
  float actual = dwGetFirstPathPower(&dev);

  // Assert
  TEST_ASSERT_FLOAT_WITHIN(0.00001, expected, actual);
}


static void verifyGetReceivePower(uint8_t* cirPwrBytes, uint8_t* rxFrameInfo, uint8_t pulseFrequency, float expected) {
  // Fixture
  dwSpiRead_StubWithCallback(dwSpiRead_executor);

  dwSpiReadExpectation_t readExpectation1 = {&dev, RX_FQUAL, CIR_PWR_SUB, cirPwrBytes, LEN_CIR_PWR, NULL};
  dwSpiRead_addExpectation(&readExpectation1);

  dwSpiReadExpectation_t readExpectation2 = {&dev, RX_FINFO, NO_SUB, rxFrameInfo, LEN_RX_FINFO, NULL};
  dwSpiRead_addExpectation(&readExpectation2);

  dev.pulseFrequency = pulseFrequency;

  // Test
  float actual = dwGetReceivePower(&dev);

  // Assert
  TEST_ASSERT_FLOAT_WITHIN(0.00001, expected, actual);
}
