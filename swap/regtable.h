/**
 * regtable.h
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

#ifndef _REGTABLE_H
#define _REGTABLE_H

#include "register.h"

/**
 * Macros
 */
#define getChronosState()         regTable[REGI_SYSSTATE]->value->data[0]
#define setChronosState(state)    regTable[REGI_SYSSTATE]->value->data[0] = state

/**
 * Register indexes
 */

enum REGINDEX
{
  REGI_PRODUCTCODE = 0,
  REGI_HWVERSION,
  REGI_FWVERSION,
  REGI_SYSSTATE,
  REGI_FREQCHANNEL,
  REGI_SECUOPTION,
  REGI_SECUPASSWD,
  REGI_SECUNONCE,
  REGI_NETWORKID,
  REGI_DEVADDRESS,
  REGI_CALIBRATION,
  REGI_TXPERIOD,
  REGI_DATETIME,
  REGI_TIMEALARM,
  REGI_TEMPPRESSALTI,
  REGI_ACCELEROMETER,
  REGI_CFGEXTENDPOINT
};

#define SWAP_NBOF_REGISTERS    19

/**
 * Array of registers
 */
extern REGISTER* regTable[];

/**
 * Size of regTable
 */
extern const byte regTableSize;

/**
 * swSendInfoPacket
 *
 * Send SWAP info packet
 *
 * 'rId'    Register id
 *
 * Return:
 *  MRFI_TX_RESULT_SUCCESS - transmit succeeded
 *  MRFI_TX_RESULT_FAILED  - transmit failed because CCA failed
 */
extern byte swSendInfoPacket(byte rId);

#endif

