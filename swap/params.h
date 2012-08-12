/**
 * params.h
 *
 * Copyright (c) 2011 Daniel Berenguer <dberenguer@usapiens.com>
 * 
 * This file is part of the panStamp project.
 * 
 * panStamp  is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
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
 * Creation date: 03/31/2011
 */

#ifndef _PARAMS_H
#define _PARAMS_H

#include "swapdefs.h"

/**
 * System states
 * This is the old protocol
enum SYSTATE
{
  SYSTATE_RESTART = 0,
  SYSTATE_RUNNING,
  SYSTATE_SYNC,
  SYSTATE_STOPSWAP,
  SYSTATE_SENDACC
};
 */

//TODO this is the new protocol SYNC and STOSWAP reverted>!!>
enum SYSTATE
{
  SYSTATE_RESTART = 0,
  SYSTATE_RUNNING,
  SYSTATE_STOPSWAP,
  SYSTATE_SYNC,
  SYSTATE_SENDACC
};

/**
 * System state
 */
byte swSystemState;

/**
 * Security password (not implemented yet)
 */
byte dtPasswd[1];

/**
 * Frequency channel
 */
byte swFreqChannel;

/**
 * Network ID
 */
byte swNetworkId[2];

/**
 * Device address
 */
byte swDeviceAddress;

/**
 * Security option
 */
byte swSecuOption;

/**
 * Security nonce
 */
byte swSecuNonce;

/**
 * Tx period for temperature, altitude and pressure (in minutes)
 */
byte swTxPeriod;

/**
 * swSetDefaultConfigParams
 * 
 * Set default configuration parameters
 */
extern void swSetDefaultConfigParams(void);

/**
 * swSetCurrentConfigParams
 * 
 * Set current configuration parameters
 */
extern void swSetCurrentConfigParams(void);

/**
 * swSetFreqChannel
 * 
 * Set frequency channel
 *
 * 'channel'  New frequency channel
 */
extern void swSetFreqChannel(byte channel);

/**
 * swSetNetworkId
 * 
 * Set network ID
 *
 * 'netId'  New network id
 */
extern void swSetNetworkId(byte *netId);

/**
 * swSetDeviceAddress
 * 
 * Set device address
 *
 * 'address'  New mote address
 */
extern void swSetDeviceAddress(byte address);

#endif

