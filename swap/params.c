/**
 * params.c
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
 * Creation date: 04/24/2011
 */

#include "params.h"
#include "mrfi.h"

#include <string.h>

/**
 * swSetDefaultConfigParams
 * 
 * Set default configuration parameters
 */
void swSetDefaultConfigParams(void)
{
  // Set default channel number
  swSetFreqChannel(SWAP_DEF_CHANNEL);
  
  // Set default Network ID
  byte netId[] = SWAP_DEF_NETID; 
  swSetNetworkId(netId);

  // Set default device address
  swSetDeviceAddress(SWAP_DEF_DEVADDR);

  // Initial system state
  swSystemState = SYSTATE_STOPSWAP;

  // Reset security nonce
  swSecuNonce = 0;

  // Default Tx period = 0 -> no periodic transmission
  swTxPeriod = 0;
}

/**
 * swSetCurrentConfigParams
 * 
 * Set current configuration parameters
 */
void swSetCurrentConfigParams(void)
{
  // Set current channel number
  mrfiRadioInterfaceWriteReg(CHANNR, swFreqChannel);
  
  // Set current Network ID
  mrfiRadioInterfaceWriteReg(SYNC1, swNetworkId[0]);
  mrfiRadioInterfaceWriteReg(SYNC0, swNetworkId[1]);

  // Set current device address
  mrfiRadioInterfaceWriteReg(ADDR, swDeviceAddress);
}

/**
 * swSetFreqChannel
 * 
 * Set frequency channel
 *
 * 'channel'  New frequency channel
 */
void swSetFreqChannel(byte channel)
{
  swFreqChannel = channel;
  mrfiRadioInterfaceWriteReg(CHANNR, swFreqChannel);
}

/**
 * swSetNetworkId
 * 
 * Set network ID
 *
 * 'netId'  New network id
 */
void swSetNetworkId(byte *netId)
{
  memcpy(swNetworkId, netId, sizeof(swNetworkId));
  mrfiRadioInterfaceWriteReg(SYNC1, swNetworkId[0]);
  mrfiRadioInterfaceWriteReg(SYNC0, swNetworkId[1]);
}

/**
 * swSetDeviceAddress
 * 
 * Set device address
 *
 * 'address'  New mote address
 */
void swSetDeviceAddress(byte address)
{
  swDeviceAddress = address;
  mrfiRadioInterfaceWriteReg(ADDR, swDeviceAddress);
}

