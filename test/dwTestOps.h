#ifndef __DW_TEST_OPS_H__
#define __DW_TEST_OPS_H__

  #include <libdw1000Types.h>

  // Definitions of functions that can be used to generate mocks for the
  // ops functions,

  void spiRead(dwDevice_t* dev, const void *header, size_t headerLength,
                                   void* data, size_t dataLength);

  void spiWrite(dwDevice_t* dev, const void *header, size_t headerLength,
                                    const void* data, size_t dataLength);

  void spiSetSpeed(dwDevice_t* dev, dwSpiSpeed_t speed);
  void delayms(dwDevice_t* dev, unsigned int delay);
  void reset(dwDevice_t *dev);

#endif // __DW_TEST_OPS_H__