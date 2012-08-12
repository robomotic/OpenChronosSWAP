/**
 * periodic.c
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
 * Creation date: 06/13/2011
 */

#include "swapdefs.h"
#include "regtable.h"
#include "params.h"
#include "swap.h"

/**
 * Tick counter for 1-sec tasks
 */
static int countTransmitSensorData = 0;

/**
 * Transmit ACC data
 */
bool transmitACCdata = false;

/**
 * swProcessPeriodicTasks
 * 
 * Process periodic tasks. Function called once per second
 */
void swProcessPeriodicTasks(void) 
{
  if ((getChronosState() == SYSTATE_STOPSWAP) && (swTxPeriod > 0))
  {
    // Transmit sensor data?
    if (countTransmitSensorData == 0)
    {
      countTransmitSensorData = swTxPeriod;
    	swInit();
      regTable[REGI_TEMPPRESSALTI]->updateValue(REGI_TEMPPRESSALTI);
      swStop();
    }

    countTransmitSensorData--;
  }
}

/**
 * sendPeriodicAcc
 *
 * Transmit periodic acceleration data via SWAP
 */
void sendPeriodicAcc(void)
{
  if (transmitACCdata)
  {
    // Send acceleration data via SWAP
  	swInit();
    swSendInfoPacket(REGI_ACCELEROMETER);
    swStop();
    setChronosState(SYSTATE_SENDACC);
    request.flag.acceleration_measurement = 1;
  }
}

