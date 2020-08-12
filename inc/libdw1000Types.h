/*
 * Driver for decaWave DW1000 802.15.4 UWB radio chip.
 *
 * Copyright (c) 2016 Bitcraze AB
 * Converted to C from  the Decawave DW1000 library for arduino.
 * which is Copyright (c) 2015 by Thomas Trojer <thomas@trojer.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __LIBDW1000_TYPES_H__
#define __LIBDW1000_TYPES_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "dw1000.h"

struct dwOps_s;
struct dwDevice_s;

typedef union dwTime_u {
	uint8_t raw[5];
	uint64_t full;
	struct {
		uint32_t low32;
		uint8_t high8;
	} __attribute__((packed));
	struct {
		uint8_t low8;
		uint32_t high32;
	} __attribute__((packed));
} dwTime_t;

typedef void (*dwHandler_t)(struct dwDevice_s *dev);

/**
 * DW device type. Contains the context of a dw1000 device and should be passed
 * as first argument of most of the driver functions.
 */
typedef struct dwDevice_s {
	struct dwOps_s *ops;
	void *userdata;

	/* State */
	uint8_t sysctrl[LEN_SYS_CTRL];
	uint8_t deviceMode;
	uint8_t networkAndAddress[LEN_PANADR];
	uint8_t syscfg[LEN_SYS_CFG];
	uint8_t sysmask[LEN_SYS_MASK];
	uint8_t chanctrl[LEN_CHAN_CTRL];
	uint8_t sysstatus[LEN_SYS_STATUS];
	uint8_t txfctrl[LEN_TX_FCTRL];

	uint8_t extendedFrameLength;
	uint8_t pacSize;
	uint8_t pulseFrequency;
	uint8_t dataRate;
	uint8_t preambleLength;
	uint8_t preambleCode;
	uint8_t channel;
	bool smartPower;
	bool frameCheck;
	bool permanentReceive;
	bool wait4resp;

	dwTime_t antennaDelay;

	// Callback handles
	dwHandler_t handleSent;
	dwHandler_t handleError;
	dwHandler_t handleReceived;
	dwHandler_t handleReceiveTimeout;
	dwHandler_t handleReceiveFailed;
	dwHandler_t handleReceiveTimestampAvailable;

	// settings
	uint32_t txPower;
	bool forceTxPower;
} dwDevice_t;

typedef enum {dwSpiSpeedLow, dwSpiSpeedHigh} dwSpiSpeed_t;

typedef enum {dwClockAuto = 0x00, dwClockXti = 0x01, dwClockPll = 0x02} dwClock_t;

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
	 *	 - dwSpiSpeedLow: <= 4MHz
	 *	 - dwSpiSpeedHigh: <= 20MHz
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

#endif //__LIBDW1000_TYPES_H__
