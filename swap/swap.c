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

// *************************************************************************************************
// Include section

// swap
#include "swap.h"
#include "regtable.h"
#include "params.h"
#include "swapbrsr.h"
#include "register.h"

// driver
#include "ports.h"
#include "display.h"
#include "adc12.h"
#include "timer.h"
#include "mrfi.h"

#include "buzzer.h"

// logic
#include "user.h"

/**
 * Internal functions
 */
static byte swTransmit(SWPACKET *swPacket);
static void dispatchSwapPacket(SWPACKET *swPacket);

/**
 * Internal global variables
 */
static byte glblDataArray[SWAP_MAX_REG_DATA_LEN];
static SWDATA glblSwapData;
static byte expectedRegAddr, expectedRegId = 0;
static bool waitForSwapInfo = false;
static bool loadDefaultParams = true;

extern void Mrfi_RxModeOff(void);

/**
 * getRegister
 *
 * Return pointer to register with ID = regId
 *
 * 'regId'  Register ID
 */
REGISTER * getRegister(byte regId)
{
  if (regId >= regTableSize)
    return NULL;

  return regTable[regId]; 
}

/**
 * swInit
 *
 * Initialize SWAP comms
 */
void swInit(void)
{
  // Setup radio
  MRFI_Init();

  // Set default configuration parameters?
  if (loadDefaultParams)
  {
    swSetDefaultConfigParams();
    loadDefaultParams = false;
  }
  // Or the current ones?
  else
    swSetCurrentConfigParams();

  // Initialize global variables
  memset(glblDataArray, 0, sizeof(glblDataArray));
  glblSwapData.data = glblDataArray;
  glblSwapData.length = 0;
  
  // Start comms
  swStart();
}

/**
 * swStart
 *
 * Start (or re-start) SWAP comms
 */
void swStart(void)
{
  MRFI_WakeUp();
  // Enable reception
  MRFI_RxOn();
  
  // New SWAP state = running
  setChronosState(SYSTATE_RUNNING);
}

/**
 * swStop
 *
 * Stop SWAP comms
 */
void swStop(void)
{
  // New SWAP state = SWAP stopped
  setChronosState(SYSTATE_STOPSWAP);

  // Go to sleep mode
  MRFI_RxIdle();
  MRFI_Sleep();
}

/**
 * swTransmit
 *
 * Transmit SWAP packet
 *
 * 'packet'   Pointer to the SWAP packet to be sent
 *
 * Return:
 *  MRFI_TX_RESULT_SUCCESS - transmit succeeded
 *  MRFI_TX_RESULT_FAILED  - transmit failed because CCA failed
 */
static byte swTransmit(SWPACKET *swPacket)
{
  mrfiPacket_t mPacket;
  int i;
  byte ret;

  mPacket.frame[0] = swPacket->value.length + SWAP_DATA_HEAD_LEN;
  mPacket.frame[1] = swPacket->destAddr;
  mPacket.frame[2] = swPacket->srcAddr;
  mPacket.frame[3] = (swPacket->hop << 4) & 0xF0;
  mPacket.frame[3] |= swPacket->security & 0x0F;
  mPacket.frame[4] = swPacket->nonce;
  mPacket.frame[5] = swPacket->function;
  mPacket.frame[6] = swPacket->regAddr;
  mPacket.frame[7] = swPacket->regId;

  for(i=0 ; i<swPacket->value.length ; i++)
    mPacket.frame[8+i] = swPacket->value.data[i];

  i = SWAP_NB_TX_TRIES;
  while ((ret = MRFI_Transmit(&mPacket, SWAP_TX_METHOD)) == MRFI_TX_RESULT_FAILED && i > 0)
  {
    i--;
    DELAY_MS(SWAP_DY_TX_TRIES);
  }

  return ret;
}

/**
 * MRFI_RxCompleteISR
 *
 * Callback function called whenever a new wireless packet is received
 */
void MRFI_RxCompleteISR(void)
{
  SWPACKET swPacket;
  mrfiPacket_t mPacket;

  MRFI_Receive(&mPacket);

  swPacket.value.length = mPacket.frame[0] - SWAP_DATA_HEAD_LEN;
  swPacket.destAddr = mPacket.frame[1];
  swPacket.srcAddr = mPacket.frame[2];
  swPacket.hop = (mPacket.frame[3] >> 4) & 0x0F;
  swPacket.security = mPacket.frame[3] & 0x0F;
  swPacket.nonce = mPacket.frame[4];
  swPacket.function = mPacket.frame[5];
  swPacket.regAddr = mPacket.frame[6];
  swPacket.regId = mPacket.frame[7];
  swPacket.value.data = mPacket.frame + 8;

  dispatchSwapPacket(&swPacket);

  return;
}

/**
 * dispatchSwapPacket
 *
 * Process SWAP packet
 *
 * 'swPacket'  pointer to the SWAP packet
 */
static void dispatchSwapPacket(SWPACKET *swPacket)
{
  REGISTER *reg;

  // Function
  switch(swPacket->function)
  {
    case SWAPFUNCT_CMD:
      // Valid register?
      if ((reg = getRegister(swPacket->regId)) == NULL)
        break;
      if (reg->setValue != NULL)
      {
        // Filter incorrect data lengths
        if (swPacket->value.length == reg->value->length)
          reg->setValue(swPacket->regId, swPacket->value.data);
      }
      else
      {
        // Send SWAP information message
        swTransmitInfo(swPacket->regId, reg->value);
      }
      break;
    case SWAPFUNCT_QRY:
      // Valid register?
      if ((reg = getRegister(swPacket->regId)) == NULL)
        break;
      if (reg->updateValue != NULL)
        reg->updateValue(swPacket->regId);
      // Send SWAP information message
      swTransmitInfo(swPacket->regId, reg->value);
      break;
    case SWAPFUNCT_INF:
      // Update table of external endpoints
      swUpdateExternalEndpointVal(swPacket->regAddr, swPacket->regId, &swPacket->value);
      // Waiting for SWAP info?
      if (waitForSwapInfo)
      {
        if (expectedRegAddr == swPacket->regAddr && expectedRegId == swPacket->regId)    
        {
          // Save SWAP data into the global variable
          memcpy(glblSwapData.data, swPacket->value.data, swPacket->value.length);
          glblSwapData.length = swPacket->value.length;
          // Notify the reception of the expected response
          waitForSwapInfo = FALSE;
        }
      }
      break;
    default:
      break;
  }
}

/**
 * swTransmitInfo
 *
 * Send SWAP info packet
 *
 * 'rId'    Register id
 * 'value'  Pointer to the register value
 *
 * Return:
 *  MRFI_TX_RESULT_SUCCESS - transmit succeeded
 *  MRFI_TX_RESULT_FAILED  - transmit failed because CCA failed
 */
byte swTransmitInfo(byte rId, SWDATA *value)
{
  SWPACKET swPacket;

  swPacket.value.length = value->length;
  swPacket.value.data = value->data;

  swPacket.destAddr = SWAP_BCAST_ADDR;
  swPacket.srcAddr = swDeviceAddress;
  swPacket.hop = 0;
  swPacket.security = 0;
  swPacket.nonce = swSecuNonce++;
  swPacket.function = SWAPFUNCT_INF;
  swPacket.regAddr = swDeviceAddress;
  swPacket.regId = rId;

  return swTransmit(&swPacket);
}

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
byte swTransmitCmd(byte dAddr, byte rAddr, byte rId, SWDATA *value)
{
  SWPACKET swPacket;

  swPacket.value.length = value->length;
  swPacket.value.data = value->data;

  swPacket.destAddr = dAddr;
  swPacket.srcAddr = swDeviceAddress;
  swPacket.hop = 0;
  swPacket.security = 0;
  swPacket.nonce = 0;
  swPacket.function = SWAPFUNCT_CMD;
  swPacket.regAddr = rAddr;
  swPacket.regId = rId;

  return swTransmit(&swPacket);
}

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
byte swTransmitQry(byte dAddr, byte rAddr, byte rId)
{
  SWPACKET swPacket;

  swPacket.destAddr = dAddr;
  swPacket.srcAddr = swDeviceAddress;
  swPacket.hop = 0;
  swPacket.security = 0;
  swPacket.nonce = 0;
  swPacket.function = SWAPFUNCT_QRY;
  swPacket.regAddr = rAddr;
  swPacket.regId = rId;

  swPacket.value.length = 0;

  return swTransmit(&swPacket);
}

/**
 * swQryRegister
 *
 * Query SWAP register and wait for response
 *
 * 'dAddr' Device address
 * 'rAddr' Register address
 * 'rId'   Register id
 *
 * Return:
 *  pointer to the SWAP data received from the remote device
 *  NULL in case of lack of response
 */
SWDATA* swQryRegister(byte dAddr, byte rAddr, byte rId)
{
  unsigned long i = SWAP_DELAY_QRY_INF / 10;

  // Expected response
  expectedRegAddr = rAddr;
  expectedRegId = rId;

  if (swTransmitQry(dAddr, rAddr, rId) == MRFI_TX_RESULT_FAILED)
    return NULL;

  waitForSwapInfo = TRUE;

  //Wait up to 500 msec for the response
  while(waitForSwapInfo)
  {
    DELAY_MS(10);

    if (i-- == 0)
    {
      waitForSwapInfo = FALSE;
      return NULL;
    }
  }

  return &glblSwapData;
}

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
SWDATA* swCmdRegister(byte dAddr, byte rAddr, byte rId, SWDATA *value)
{
  unsigned long i = SWAP_DELAY_QRY_INF / 10;

  // Expected response
  expectedRegAddr = rAddr;
  expectedRegId = rId;

  if (swTransmitCmd(dAddr, rAddr, rId, value) == MRFI_TX_RESULT_FAILED)
    return NULL;

  waitForSwapInfo = TRUE;

  //Wait up to 500 msec for the response
  while(waitForSwapInfo)
  {
    DELAY_MS(10);

    if (i-- == 0)
    {
      waitForSwapInfo = FALSE;
      return NULL;
    }
  }

  return &glblSwapData;
}

