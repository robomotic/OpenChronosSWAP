/**
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

#ifndef _SWAP_H_
#define _SWAP_H_
#undef CONFIG_TEST
#include "swapdefs.h"

/**
 * Macros
 */
#define DELAY_MS(millis)    Timer0_A4_Delay(CONV_MS_TO_TICKS(millis))

/**
 * SWAP message functions
 */
enum SWAPFUNCT
{
  SWAPFUNCT_INF = 0x00,
  SWAPFUNCT_QRY,
  SWAPFUNCT_CMD
};

/**
 * Structure: SWDATA
 * 
 * Description:
 * SWAP data structure
 */
typedef struct
{
  /**
   * Data buffer
   */
  byte *data;

  /**
   * Data length
   */
  byte length;
} SWDATA;

typedef struct
{
  /**
   * Destination address
   */
  byte destAddr;

  /**
   * Source address
   */
  byte srcAddr;

  /**
   * Hop counter. Incremented each time the message is repeated
   */
  byte hop;

  /**
   * Security option byte
   */
  byte security;

  /**
   * Security cyclic nonce
   */
  byte nonce;

  /**
   * Function byte
   */
  byte function;

  /**
   * Register address
   */
  byte regAddr;

  /**
   * Register id
   */
  byte regId;

  /**
   * Register value
   */
  SWDATA value;
} SWPACKET;

/**
 * swInit
 *
 * Initialize SWAP comms
 */
extern void swInit(void);

/**
 * swStart
 *
 * Start (or re-start) SWAP comms
 */
extern void swStart(void);

/**
 * swStop
 *
 * Stop SWAP comms
 */
extern void swStop(void);

/**
 * swTransmitInfo
 *
 * Send SWAP info packet
 *
 * 'rId'   Register id
 * 'value' Pointer to the register value
 *
 * Return:
 *  MRFI_TX_RESULT_SUCCESS - transmit succeeded
 *  MRFI_TX_RESULT_FAILED  - transmit failed because CCA failed
 */
extern byte swTransmitInfo(byte rId, SWDATA *value);

/**
 * swTransmitCmd
 *
 * Send SWAP command packet
 *
 * 'dAddr' Device address
 * 'rAddr' Register address
 * 'rId'   Register id
 * 'value' Pointer to the register value
 *
 * Return:
 *  MRFI_TX_RESULT_SUCCESS - transmit succeeded
 *  MRFI_TX_RESULT_FAILED  - transmit failed because CCA failed
 */
extern byte swTransmitCmd(byte dAddr, byte rAddr, byte rId, SWDATA *value);

/**
 * swTransmitQry
 *
 * Send SWAP query packet
 *
 * 'dAddr' Device address
 * 'rAddr' Register address
 * 'rId'   Register id
 *
 * Return:
 *  MRFI_TX_RESULT_SUCCESS - transmit succeeded
 *  MRFI_TX_RESULT_FAILED  - transmit failed because CCA failed
 */
extern byte swTransmitQry(byte dAddr, byte rAddr, byte rId);

/**
 * swQryRegister
 *
 * Query SWAP register and return value
 *
 * 'dAddr' Device address
 * 'rAddr' Register address
 * 'rId'   Register id
 *
 * Return:
 *  pointer to the SWAP data received from the remote device
 *  NULL in case of lack of response
 */
extern SWDATA* swQryRegister(byte dAddr, byte rAddr, byte rId);

/**
 * swCmdRegister
 *
 * Command SWAP register and wait for response
 *
 * 'dAddr' Device address
 * 'rAddr' Register address
 * 'rId'   Register id
 * 'value' New SWDATA value
 *
 * Return:
 *  pointer to the SWAP data received from the remote device
 *  NULL in case of lack of response
 */
extern SWDATA* swCmdRegister(byte dAddr, byte rAddr, byte rId, SWDATA *value);

#endif
