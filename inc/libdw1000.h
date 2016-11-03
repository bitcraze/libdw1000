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

#ifndef __LIBDW1000_H__
#define __LIBDW1000_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "libdw1000Spi.h"


// Default Mode of operation
extern const uint8_t MODE_LONGDATA_RANGE_LOWPOWER[];
extern const uint8_t MODE_SHORTDATA_FAST_LOWPOWER[];
extern const uint8_t MODE_LONGDATA_FAST_LOWPOWER[];
extern const uint8_t MODE_SHORTDATA_FAST_ACCURACY[];
extern const uint8_t MODE_LONGDATA_FAST_ACCURACY[];
extern const uint8_t MODE_LONGDATA_RANGE_ACCURACY[];

/**
 * Initialize the device data structure.
 */
void dwInit(dwDevice_t* dev, dwOps_t* ops);

/**
 * Set a userData pointer to the device.
 */
void dwSetUserdata(dwDevice_t* dev, void* userdata);

/**
 * Get the userData pointer from the device.
 */
void* dwGetUserdata(dwDevice_t* dev);

/**
 * Setup the DW1000
 */
int dwConfigure(dwDevice_t* dev);

/**
 * Read and return the device ID, only chip with ID 0xdeca0130 is supported.
 */
uint32_t dwGetDeviceId(dwDevice_t* dev);

/**
 * Manualy blinks LEDs.
 * @param leds Bit-field of the LEDs to blink
 */
void dwEnableAllLeds(dwDevice_t* dev);

/**
 * Sets clock Mode
 */
void dwEnableClock(dwDevice_t* dev, dwClock_t clock);

/**
 * Resets the chip via SPI
 */
void dwSoftReset(dwDevice_t* dev);

void dwManageLDE(dwDevice_t* dev);

/* ###########################################################################
 * #### DW1000 register read/write ###########################################
 * ######################################################################### */

void dwReadSystemConfigurationRegister(dwDevice_t* dev);
void dwWriteSystemConfigurationRegister(dwDevice_t* dev);
void dwReadSystemEventStatusRegister(dwDevice_t* dev);
void dwReadNetworkIdAndDeviceAddress(dwDevice_t* dev);
void dwWriteNetworkIdAndDeviceAddress(dwDevice_t* dev);
void dwReadSystemEventMaskRegister(dwDevice_t* dev);
void dwWriteSystemEventMaskRegister(dwDevice_t* dev);
void dwReadChannelControlRegister(dwDevice_t* dev);
void dwWriteChannelControlRegister(dwDevice_t* dev);
void dwReadTransmitFrameControlRegister(dwDevice_t* dev);
void dwWriteTransmitFrameControlRegister(dwDevice_t* dev);

/****************************************************************/

/**
 * Set Receive Wait Timeout.
 * @param timeout Timeout in step of 1.026us (512 count of the dw1000
 *                 fundamental 499.2MHz clock) or 0 to disable the timeout.
 *
 * @note dwCommitConfiguration() should be called if this function
 * enables or disables the timeout. If the timeout is just updated and not
 * enabled this function will update to the new timeout and nothing more has to
 * be done.
 */
void dwSetReceiveWaitTimeout(dwDevice_t *dev, uint16_t timeout);

void dwSetFrameFilter(dwDevice_t* dev, bool val);
void dwSetFrameFilterBehaveCoordinator(dwDevice_t* dev, bool val);
void dwSetFrameFilterAllowBeacon(dwDevice_t* dev, bool val);
void dwSetFrameFilterAllowData(dwDevice_t* dev, bool val);
void dwSetFrameFilterAllowAcknowledgement(dwDevice_t* dev, bool val);
void dwSetFrameFilterAllowMAC(dwDevice_t* dev, bool val);
void dwSetFrameFilterAllowReserved(dwDevice_t* dev, bool val);
void dwSetDoubleBuffering(dwDevice_t* dev, bool val);
void dwSetInterruptPolarity(dwDevice_t* dev, bool val);
void dwSetReceiverAutoReenable(dwDevice_t* dev, bool val);
void dwInterruptOnSent(dwDevice_t* dev, bool val);
void dwInterruptOnReceived(dwDevice_t* dev, bool val);
void dwInterruptOnReceiveFailed(dwDevice_t* dev, bool val);
void dwInterruptOnReceiveTimeout(dwDevice_t* dev, bool val);
void dwInterruptOnReceiveTimestampAvailable(dwDevice_t* dev, bool val);
void dwInterruptOnAutomaticAcknowledgeTrigger(dwDevice_t* dev, bool val);
void dwClearInterrupts(dwDevice_t* dev);



void dwIdle(dwDevice_t* dev);
void dwNewReceive(dwDevice_t* dev);
void dwStartReceive(dwDevice_t* dev);
void dwNewTransmit(dwDevice_t* dev);
void dwStartTransmit(dwDevice_t* dev);
void dwNewConfiguration(dwDevice_t* dev);
void dwCommitConfiguration(dwDevice_t* dev);
void dwWaitForResponse(dwDevice_t* dev, bool val);
void dwSuppressFrameCheck(dwDevice_t* dev, bool val);
void dwUseSmartPower(dwDevice_t* dev, bool smartPower);
dwTime_t dwSetDelay(dwDevice_t* dev, const dwTime_t* delay);
void dwSetTxRxTime(dwDevice_t* dev, const dwTime_t futureTime);
void dwSetDataRate(dwDevice_t* dev, uint8_t rate);
void dwSetPulseFrequency(dwDevice_t* dev, uint8_t freq);
uint8_t dwGetPulseFrequency(dwDevice_t* dev);
void dwSetPreambleLength(dwDevice_t* dev, uint8_t prealen);
void dwUseExtendedFrameLength(dwDevice_t* dev, bool val);
void dwReceivePermanently(dwDevice_t* dev, bool val);
void dwSetChannel(dwDevice_t* dev, uint8_t channel);
void dwSetPreambleCode(dwDevice_t* dev, uint8_t preacode);
void dwSetDefaults(dwDevice_t* dev);
void dwSetData(dwDevice_t* dev, uint8_t data[], unsigned int n);
unsigned int dwGetDataLength(dwDevice_t* dev);
void dwGetData(dwDevice_t* dev, uint8_t data[], unsigned int n);
void dwGetTransmitTimestamp(dwDevice_t* dev, dwTime_t* time);
void dwGetReceiveTimestamp(dwDevice_t* dev, dwTime_t* time);
void dwGetRawReceiveTimestamp(dwDevice_t* dev, dwTime_t* time);
void dwCorrectTimestamp(dwDevice_t* dev, dwTime_t* timestamp);
void dwGetSystemTimestamp(dwDevice_t* dev, dwTime_t* time);
bool dwIsTransmitDone(dwDevice_t* dev);
bool dwIsReceiveTimestampAvailable(dwDevice_t* dev);
bool dwIsReceiveDone(dwDevice_t* dev);
bool dwIsReceiveFailed(dwDevice_t *dev);
bool dwIsReceiveTimeout(dwDevice_t* dev);
bool dwIsClockProblem(dwDevice_t* dev);
void dwClearAllStatus(dwDevice_t* dev);
void dwClearReceiveTimestampAvailableStatus(dwDevice_t* dev);
void dwClearReceiveStatus(dwDevice_t* dev);
void dwClearTransmitStatus(dwDevice_t* dev);
float dwGetReceiveQuality(dwDevice_t* dev);
float dwGetFirstPathPower(dwDevice_t* dev);
float dwGetReceivePower(dwDevice_t* dev);
void dwEnableMode(dwDevice_t *dev, const uint8_t mode[]);
void dwTune(dwDevice_t *dev);
void dwHandleInterrupt(dwDevice_t *dev);

void dwAttachSentHandler(dwDevice_t *dev, dwHandler_t handler);
void dwAttachReceivedHandler(dwDevice_t *dev, dwHandler_t handler);
void dwAttachReceiveTimeoutHandler(dwDevice_t *dev, dwHandler_t handler);
void dwAttachReceiveFailedHandler(dwDevice_t *dev, dwHandler_t handler);

void dwSetAntenaDelay(dwDevice_t *dev, dwTime_t delay);

/* Tune the DWM radio parameters */
void dwTune(dwDevice_t *dev);

/**
 * Put the dwm1000 in idle mode
 */
void dwIdle(dwDevice_t* dev);

/**
 * Returns a human-readable error string
 */
char* dwStrError(int error);

/* Error codes */
#define DW_ERROR_OK 0
#define DW_ERROR_WRONG_ID 1


#endif //__LIBDW1000_H__
