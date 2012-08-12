/**
 * regtable.c
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

// SWAP
#include "regtable.h"
#include "product.h"
#include "params.h"
#include "swapbrsr.h"

// Logic
#include "temperature.h"
#include "altitude.h"
#include "acceleration.h"
#include "clock.h"
#include "date.h"
#include "alarm.h"

// Drivers
#include "display.h"
#include "infomem.h"

/**
 * Setter functions
 */
static const void setSysState(byte id, byte *state);
static const void setFreqChannel(byte id, byte *channel);
static const void setSecuOption(byte id, byte *secu);
static const void setNetworkId(byte id, byte *netId);
static const void setDevAddress(byte id, byte *addr);
static const void setCalibration(byte id, byte *calib);
static const void setPeriod(byte id, byte *period);
static const void setDateTime(byte id, byte *dt);
static const void setTimeAlarm(byte id, byte *alarm);
static const void setCfgExternEndpoint(byte id, byte *settings);

/**
 * Updater functions
 */
static const void uptTempPressAlti(byte id);

/**
 * Register setup
 */
// Product code
static byte dtProductCode[8] = {(byte)((long)SWAP_MANUFACT_ID >> 24), (byte)((long)SWAP_MANUFACT_ID >> 16), (byte)((long)SWAP_MANUFACT_ID >> 8), (byte)SWAP_MANUFACT_ID,
                       (byte)((long)SWAP_PRODUCT_ID >> 24), (byte)((long)SWAP_PRODUCT_ID >> 16), (byte)((long)SWAP_PRODUCT_ID >> 8), (byte)SWAP_PRODUCT_ID};
static SWDATA swProductCode = {dtProductCode, sizeof(dtProductCode)};
REGISTER regProductCode = {REGI_PRODUCTCODE, &swProductCode, NULL, NULL};
// Hardware version
static byte dtHwVersion[4] = {(byte)((long)HARDWARE_VERSION >> 24), (byte)((long)HARDWARE_VERSION >> 16), (byte)((long)HARDWARE_VERSION >> 8), (byte)HARDWARE_VERSION};
static SWDATA swHwVersion = {dtHwVersion, sizeof(dtHwVersion)};
REGISTER regHwVersion = {REGI_HWVERSION, &swHwVersion, NULL, NULL};
// Firmware version
static byte dtFwVersion[4] = {(byte)((long)FIRMWARE_VERSION >> 24), (byte)((long)FIRMWARE_VERSION >> 16), (byte)((long)FIRMWARE_VERSION >> 8), (byte)FIRMWARE_VERSION};
static SWDATA swFwVersion = {dtFwVersion, sizeof(dtFwVersion)};
REGISTER regFwVersion = {REGI_FWVERSION, &swFwVersion, NULL, NULL};
// System state
static SWDATA swSysState = {&swSystemState, sizeof(swSystemState)};
REGISTER regSysState = {REGI_SYSSTATE, &swSysState, NULL, &setSysState};
// Frequency channel
static SWDATA swFreqChann = {&swFreqChannel, sizeof(swFreqChannel)};
REGISTER regFreqChannel = {REGI_FREQCHANNEL, &swFreqChann, NULL, &setFreqChannel};
// Security option
static SWDATA swSecurity = {&swSecuOption, sizeof(swSecuOption)};
REGISTER regSecuOption = {REGI_SECUOPTION, &swSecurity, NULL, &setSecuOption};
// Security password (not implemented yet)
static SWDATA swPasswd = {dtPasswd, sizeof(dtPasswd)};
REGISTER regSecuPasswd = {REGI_SECUPASSWD, &swPasswd, NULL, NULL};
// Security nonce
static SWDATA swNonce = {&swSecuNonce, sizeof(swSecuNonce)};
REGISTER regSecuNonce = {REGI_SECUNONCE, &swNonce, NULL, NULL};
// Network Id
static SWDATA swNetId = {swNetworkId, sizeof(swNetworkId)};
REGISTER regNetworkId = {REGI_NETWORKID, &swNetId, NULL, &setNetworkId};
// Device address
static SWDATA swAddr = {&swDeviceAddress, sizeof(swDeviceAddress)};
REGISTER regDevAddress = {REGI_DEVADDRESS, &swAddr, NULL, &setDevAddress};
// Calibration values
static byte dtCalibration[5];
static SWDATA swCalibration = {dtCalibration, sizeof(dtCalibration)};
REGISTER regCalibration = {REGI_CALIBRATION, &swCalibration, NULL, &setCalibration};
// Calibration values
static SWDATA swPeriod = {&swTxPeriod, sizeof(swTxPeriod)};
REGISTER regPeriod = {REGI_TXPERIOD, &swPeriod, NULL, &setPeriod};
// Date/Time data
static byte dtDateTime[7];
static SWDATA swDateTime = {dtDateTime, sizeof(dtDateTime)};
REGISTER regDateTime = {REGI_DATETIME, &swDateTime, NULL, &setDateTime};
// Time alarm
static byte dtTimeAlarm[2];
static SWDATA swTimeAlarm = {dtTimeAlarm, sizeof(dtTimeAlarm)};
REGISTER regTimeAlarm = {REGI_TIMEALARM, &swTimeAlarm, NULL, &setTimeAlarm};
// Temperature & Altimeter sensors
static byte dtTempPressAlti[8];
static SWDATA swTempPressAlti = {dtTempPressAlti, sizeof(dtTempPressAlti)};
REGISTER regTempPressAlti = {REGI_TEMPPRESSALTI, &swTempPressAlti, &uptTempPressAlti, NULL};
// Accelerometer data
static SWDATA swAccelerometer = {sAccel.xyz, sizeof(sAccel.xyz)};
REGISTER regAccelerometer = {REGI_ACCELEROMETER, &swAccelerometer, NULL, NULL};
// SWAP browser - External Endpoint 0
static byte dtCfgExternEndpoint0[11] = {0x50, 0x41, 0x47, 0x45, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static SWDATA swCfgExternEndpoint0 = {dtCfgExternEndpoint0, sizeof(dtCfgExternEndpoint0)};
REGISTER regCfgExternEndpoint0 = {REGI_CFGEXTENDPOINT, &swCfgExternEndpoint0, NULL, &setCfgExternEndpoint};
// SWAP browser - External Endpoint 1
static byte dtCfgExternEndpoint1[11] = {0x50, 0x41, 0x47, 0x45, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static SWDATA swCfgExternEndpoint1 = {dtCfgExternEndpoint1, sizeof(dtCfgExternEndpoint1)};
REGISTER regCfgExternEndpoint1 = {REGI_CFGEXTENDPOINT+1, &swCfgExternEndpoint1, NULL, &setCfgExternEndpoint};
// SWAP browser - External Endpoint 2
static byte dtCfgExternEndpoint2[11] = {0x50, 0x41, 0x47, 0x45, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static SWDATA swCfgExternEndpoint2 = {dtCfgExternEndpoint2, sizeof(dtCfgExternEndpoint2)};
REGISTER regCfgExternEndpoint2 = {REGI_CFGEXTENDPOINT+2, &swCfgExternEndpoint2, NULL, &setCfgExternEndpoint};
// SWAP browser - External Endpoint 3
static byte dtCfgExternEndpoint3[11] = {0x50, 0x41, 0x47, 0x45, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static SWDATA swCfgExternEndpoint3 = {dtCfgExternEndpoint3, sizeof(dtCfgExternEndpoint3)};
REGISTER regCfgExternEndpoint3 = {REGI_CFGEXTENDPOINT+3, &swCfgExternEndpoint3, NULL, &setCfgExternEndpoint};
// SWAP browser - External Endpoint 4
static byte dtCfgExternEndpoint4[11] = {0x50, 0x41, 0x47, 0x45, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static SWDATA swCfgExternEndpoint4 = {dtCfgExternEndpoint4, sizeof(dtCfgExternEndpoint4)};
REGISTER regCfgExternEndpoint4 = {REGI_CFGEXTENDPOINT+4, &swCfgExternEndpoint4, NULL, &setCfgExternEndpoint};

/*
 * Add here your custom registers
 */

/**
 * Initialize table of registers
 */
REGISTER *regTable[] = {
        &regProductCode,
        &regHwVersion,
	      &regFwVersion,
        &regSysState,
        &regFreqChannel,
        &regSecuOption,
        &regSecuPasswd,
        &regSecuNonce,
        &regNetworkId,
        &regDevAddress,
        &regCalibration,
        &regPeriod,
        &regDateTime,
        &regTimeAlarm,
        &regTempPressAlti,
        &regAccelerometer,
        &regCfgExternEndpoint0,
        &regCfgExternEndpoint1,
        &regCfgExternEndpoint2,
        &regCfgExternEndpoint3,
        &regCfgExternEndpoint4
}; 

/**
 * Size of regTable
 */
 const byte regTableSize = sizeof(regTable);

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
byte swSendInfoPacket(byte rId)
{
  return swTransmitInfo(rId, regTable[rId]->value);
}

/**
 * "Update/Set" handling functions
 */

/**
 * setSysState
 *
 * Set system state
 *
 * 'id'     Register id
 * 'state'  New system state
 */
static const void setSysState(byte id, byte *state)
{ 
  switch(state[0])
  {
    case SYSTATE_RESTART:
      setChronosState(state[0]);
      // Send info message before restarting the Chronos
      swSendInfoPacket(id);
      // Restart SWAP comms only
      swStop();
      swInit();
      break;
    case SYSTATE_STOPSWAP:
      // Stop SWAP comms and clear icons
      swStopSwapRxTx();
      break;
    default:
      // Unable to modify state. Send info packet with the curernt state
      swSendInfoPacket(id);
      break;
  }
}

/**
 * setFreqChannel
 *
 * Set frequency channel
 *
 * 'id'       Register id
 * 'channel'  New channel
 */
static const void setFreqChannel(byte id, byte *channel)
{
  SWDATA swVal = {channel, regTable[id]->value->length};
  // Send info message before entering the new frequency channel
  swTransmitInfo(id, &swVal);
  // Update register value
  swSetFreqChannel(channel[0]);
  // Send product code over the new frequency channel
  swSendInfoPacket(REGI_PRODUCTCODE);
}

/**
 * setSecuOption
 *
 * Set security option
 *
 * 'id'    Register id
 * 'secu'  New security option
 */
static const void setSecuOption(byte id, byte *secu)
{
  SWDATA swVal = {secu, regTable[id]->value->length};
  // Send info message before entering the new security option
  swTransmitInfo(id, &swVal);
  // Update register value
  swSecuOption = secu[0] & 0x0F;
}

/**
 * setNetworkId
 *
 * Set network id
 *
 * 'id'    Register id
 * 'netId' New network id
 */
static const void setNetworkId(byte id, byte *netId)
{
  SWDATA swVal = {netId, regTable[id]->value->length};
  // Send info message before entering the new network ID
  swTransmitInfo(id, &swVal);
  // Update register value
  swSetNetworkId(netId);
  // Send product code with the new network id
  swSendInfoPacket(REGI_PRODUCTCODE);
}

/**
 * setDevAddress
 *
 * Set device address
 *
 * 'id'    Register id
 * 'addr'  New device address
 */
static const void setDevAddress(byte id, byte *addr)
{
  if (addr[0] > 0)
  {
    SWDATA swVal = {addr, regTable[id]->value->length};
    // Send info message before taking the new device address
    swTransmitInfo(id, &swVal);
    // Update device address
    swSetDeviceAddress(addr[0]);
  }
}

/**
 * setCalibration
 *
 * Set calibration settings
 *
 * 'id'     Register id
 * 'calib'  New calibration values
 */
static const void setCalibration(byte id, byte *calib)
{
  s16 t1, offset;

  swSetRegValue(&regCalibration, calib);

  // Set metrics flag
  sys.flag.use_metric_units = dtCalibration[0] & 0x01;

	// Set temperature and temperature offset
	t1 = (s16)((dtCalibration[1] << 8) | dtCalibration[2]);
	offset = t1 - (sTemp.degrees - sTemp.offset);
	sTemp.offset  = offset;	
	sTemp.degrees = t1;									

  // Set altitude
	sAlt.altitude = (s16)((dtCalibration[3] << 8) | dtCalibration[4]);
	update_pressure_table(sAlt.altitude, sAlt.pressure, sAlt.temperature);

  // Send info packet
  swSendInfoPacket(id);
}

/**
 * setPeriod
 *
 * Set transmission period
 *
 * 'id'       Register id
 * 'period'   New interval
 */
static const void setPeriod(byte id, byte *period)
{
  // Update register value
  swSetRegValue(&regPeriod, period);

  // Send info packet
  swSendInfoPacket(id);
}

/**
 * setDateTime
 *
 * Set date and time settings
 *
 * 'id'     Register id
 * 'dt'     New date/time data
 */
static const void setDateTime(byte id, byte *dt)
{
  swSetRegValue(&regDateTime, dt);

  // New date/time settings
	sTime.hour 			= dt[0];
	sTime.minute 		= dt[1];
	sTime.second 		= dt[2];
	sDate.year 			= (dt[3]<<8) + dt[4];
	sDate.month 		= dt[5];
	sDate.day 			= dt[6];

  // Send info packet
  swSendInfoPacket(id);
}

/**
 * setTimeAlarm
 *
 * Set time alarm
 *
 * 'id'     Register id
 * 'alarm'  New time alarm
 */
static const void setTimeAlarm(byte id, byte *alarm)
{
  swSetRegValue(&regTimeAlarm, alarm);

  // New time alarm
	sAlarm.hour = alarm[0];
	sAlarm.minute = alarm[1];

  // Send info packet
  swSendInfoPacket(id);
}

/**
 * uptTempPressAlti
 *
 * Update temperature, pressure and altitude values
 *
 * 'id' Register id
 */
static const void uptTempPressAlti(byte id)
{
  // Measure temperature
  temperature_measurement(FILTER_OFF);
  // Measure pressure and altitude
  do_altitude_measurement(FILTER_OFF);
  // Update register value
  dtTempPressAlti[0] = (sTemp.degrees >> 8) & 0xFF;
  dtTempPressAlti[1] = sTemp.degrees & 0xFF;
  dtTempPressAlti[2] = (sAlt.pressure >> 24) & 0xFF;
  dtTempPressAlti[3] = (sAlt.pressure >> 16) & 0xFF;
  dtTempPressAlti[4] = (sAlt.pressure >> 8) & 0xFF;
  dtTempPressAlti[5] = sAlt.pressure & 0xFF;
  dtTempPressAlti[6] = (sAlt.altitude >> 8) & 0xFF;
  dtTempPressAlti[7] = sAlt.altitude & 0xFF;

  // Send info packet
  swSendInfoPacket(id);
}

/**
 * setCfgExternEndpoint
 *
 * Set external endpoint settings
 *
 * 'id'        Register id
 * 'settings'  Endpoint settings
 */
static const void setCfgExternEndpoint(byte id, byte *settings)
{
  // Update browser
  if (!swSetExternalEndpointCfg(id - REGI_CFGEXTENDPOINT, settings))
  {
    // Update endpoint value
    swSetRegValue(regTable[id], settings);
  }

  // Send info packet
  swSendInfoPacket(id);
}

