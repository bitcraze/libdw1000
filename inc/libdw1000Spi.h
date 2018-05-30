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

#ifndef __LIBDW1000_SPI_H__
#define __LIBDW1000_SPI_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "dw1000.h"
#include "libdw1000Types.h"

/**
 * Read from the dw1000 SPI interface
 */
void dwSpiRead(dwDevice_t *dev, uint8_t regid, uint32_t address, void* data, size_t length);
uint16_t dwSpiRead16(dwDevice_t *dev, uint8_t regid, uint32_t address);
uint32_t dwSpiRead32(dwDevice_t *dev, uint8_t regid, uint32_t address);

/**
 * Write to the dw1000 SPI interface
 */
void dwSpiWrite(dwDevice_t *dev, uint8_t regid, uint32_t address,
                                 const void* data, size_t length);

void dwSpiWrite8(dwDevice_t *dev, uint8_t regid, uint32_t address,
                                   uint8_t data);

void dwSpiWrite32(dwDevice_t *dev, uint8_t regid, uint32_t address,
                                  uint32_t data);

#endif //__LIBDW1000_SPI_H__
