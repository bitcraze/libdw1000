# Libdw1000  [![CI](https://github.com/bitcraze/libdw1000/workflows/CI/badge.svg)](https://github.com/bitcraze/libdw1000/actions?query=workflow%3ACI)

Open source driver implementation for the Decawave DW1000 UWB radio chip

This driver is mainly a port of the [Arduino DW1000 driver][1]. As such it is
licensed under the same license, Apache2.

## Usage

### Initialize

All functions of the lib takes a ```dwDevice_t *``` as first argument. The struct
contains the state of the driver, and the purpose is to enable the driver could be used
to control more than one dw1000 radio in the same system.

Example of libdw initialization:

``` c
dwDevice_t dwm_device;
dwDevice_t *dwm = &dwm_device;

// (...)

printf("TEST\t: Initialize DWM1000 ... ");
dwInit(dwm, &dwOps);       // Init libdw
dwOpsInit(dwm);
result = dwConfigure(dwm); // Configure the dw1000 chip
if (result == 0) {
  printf("[OK]\r\n");
  dwEnableAllLeds(dwm);
} else {
  printf("[ERROR]: %s\r\n", dwStrError(result));
  selftestPasses = false;
}

// (...)

dwTime_t delay = {.full = ANTENNA_DELAY/2};
dwSetAntenaDelay(dwm, delay);

dwAttachSentHandler(dwm, txcallback);
dwAttachReceivedHandler(dwm, rxcallback);

dwNewConfiguration(dwm);
dwSetDefaults(dwm);
dwEnableMode(dwm, MODE_SHORTDATA_FAST_ACCURACY);
dwSetChannel(dwm, CHANNEL_2);
dwSetPreambleCode(dwm, PREAMBLE_CODE_64MHZ_9);

dwCommitConfiguration(dwm);
```

The ```txcallback``` and ```rxcallback``` function are defined that way:
``` c
void txcallback(dwDevice_t *dev);
void rxcallback(dwDevice_t *dev);
```

They are called when a packet has been received or sent.

### Ops

The driver is platform independent and so you need to provide platform-specific
function. This is done by instanciating a dwOps_t structure:

``` c
/**
 * DW operation type. Constains function pointer to all hardware-dependent
 * operation required to access the DW1000 device.
 */
typedef struct dwOps_s {
  /**
   * Function that activates the chip-select, sends header, read data and
   * disable the chip-select.
   */
  void (*spiRead)(dwDevice_t* dev, const void *header, size_t headerLength,
                                   void* data, size_t dataLength);

  /**
   * Function that activates the chip-select, sends header, sends data and
   * disable the chip-select.
   */
  void (*spiWrite)(dwDevice_t* dev, const void *header, size_t headerLength,
                                    const void* data, size_t dataLength);

  /**
   * Sets the SPI bus speed. Take as argument:
   *   - dwSpiSpeedLow: <= 4MHz
   *   - dwSpiSpeedHigh: <= 20MHz
   */
  void (*spiSetSpeed)(dwDevice_t* dev, dwSpiSpeed_t speed);

  /**
   * Waits at least 'delay' miliseconds.
   */
  void (*delayms)(dwDevice_t* dev, unsigned int delay);

  /**
   * Resets the DW1000 by pulling the reset pin low and then releasing it.
   * This function is optional, if not set softreset via SPI will be used.
   */
   void (*reset)(dwDevice_t *dev);
} dwOps_t;
```

### Send and receive

To send a packet:
``` c
dwNewTransmit(dev);
dwSetDefaults(dev);
dwSetData(dev, (uint8_t*)&txPacket, MAC802154_HEADER_LENGTH+2);

dwStartTransmit(dev);
```

To receive a packet:
``` c
dwNewReceive(dev);
dwSetDefaults(dev);
dwStartReceive(dev);
```

To put the radio in IDLE mode (cancel current send/receive)
``` c
dwIdle(dev);
```

## Testing

### Dependencies

Frameworks for unit testing are pulled in as git submodules. To get them when cloning

```bash
git clone --recursive https://github.com/bitcraze/lps-node-firmware.git
```
        
or if you already have a cloned repo and want the submodules
 
```bash
git submodule init        
git submodule update        
```

The testing framework uses ruby and rake to generate and run code. 

To minimize the need for installations and configuration, use the docker builder
image (bitcraze/builder) that contains all tools needed. All scripts in the 
tools/build directory are intended to be run in the image. You may use the 
utility script tools/do to start the docker container. For instance
 
        tools/do build

### Running unit tests
    
With the environment set up locally

        rake

with the docker builder image

        tools/do test



[1]: https://github.com/thotro/arduino-dw1000

## Contribute
Go to the [contribute page](https://www.bitcraze.io/contribute/) on our website to learn more.

### Test code for contribution
Run the automated build locally to test your code

	./tools/build/build
