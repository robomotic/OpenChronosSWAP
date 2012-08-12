/**
 * swapdefs.h
 *
 * Copyright (c) 2011 Daniel Berenguer <dberenguer@usapiens.com>
 * 
 * This file is part of the panStamp project.
 * 
 * panStamp  is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * panLoader is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with panLoader; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 
 * USA
 * 
 * Author: Daniel Berenguer
 * Creation date: 04/28/2011
 */

#ifndef _SWAPDEFS_H_
#define _SWAPDEFS_H_

#include "project.h"
#include <string.h>

#define byte  uint8_t
#define bool  uint8_t
#define true  1
#define false 0
#define ON    1
#define OFF   0
#define UP    1
#define DOWN  0

/**
 * CC11XX definitions
 */
#define CC1101_DATA_LEN         61

/**
 * SWAP default parameters
 */
#define SWAP_TX_METHOD          MRFI_TX_TYPE_FORCED   // Force transmission regardless of the current traffic
//#define SWAP_TX_METHOD          MRFI_TX_TYPE_CCA      // Clear channel assessment (collision avoidance) method
#define SWAP_DEF_CHANNEL        0                     // Default frequency channel
#define SWAP_DEF_NETID          {0xB5, 0x47}          // Default Network ID
#define SWAP_DEF_DEVADDR        0x07                  // Default device address
#define SWAP_MAX_REG_DATA_LEN   55                    // Max length of the data register field
#define SWAP_ACC_INTERVAL_MS    500                   // Interval between stream transmissions of Accelerometer data
#define SWAP_DATA_HEAD_LEN      7
#define SWAP_REG_VAL_LEN        CC1101_DATA_LEN - SWAP_DATA_HEAD_LEN   // SWAP data payload - max length
#define SWAP_BCAST_ADDR         0x00                                   // SWAP broadcast address
#define SWAP_NB_TX_TRIES        5                                      // Number of transmission retries
#define SWAP_DY_TX_TRIES        100                                    // Delay between Tx retries in ms
#define SWAP_TXSENSOR_PERIOD    300                                    // Transmit Sensor data every 5 min
#define SWAP_DELAY_QRY_INF      200                                    // Maximum time (msec) to wait for the response after sending a query or a command

#endif
