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

#ifndef SWAPBRSR_H_
#define SWAPBRSR_H_

#include "swapdefs.h"
#include "swap.h"
#include "bsp.h"

#define SWAP_BROWSER_MAX_PAGES    5

/**
 * General SWAP browser flag
 */
typedef enum
{
  SWAP_OFF = 0,
  SWAP_BROWSER,
  SWAP_CONTROL
} SWBRSRFLAG;

extern SWBRSRFLAG swBrowserFlag;

/**
 * Endpoint format - definitions
 */
#define ENDP_ANALOG          0b00000001
#define ENDP_OUTPUT          0b00000010
#define ENDP_HAS_DEC         0b00000100
#define ENDP_IS_SIGNED       0b00001000
#define ENDP_IS_COMBINED     0b00010000

/**
 * Infomem constants
 */
#define SWAP_INFOMEM_ID      0x00

/**
 * Macros
 */
#define isOutput()           format & ENDP_OUTPUT
#define isAnalog()           format & ENDP_ANALOG
#define isDecimal()          format & ENDP_HAS_DEC
#define isSigned()           format & ENDP_IS_SIGNED
#define isCombined()         format & ENDP_IS_COMBINED
#define isPositive()         value < 0x8000
#define getSize()            posize >> 6
#define getPosition()        posize & 0b00111111
#define isSwapMode()         swBrowserFlag != SWAP_OFF

/**
 * Errors
 */
#define SWBRSR_ERR_SIZE      "Err0"   // Actual size of the endpoint exceeds the size of the member struct
#define SWBRSR_ERR_NOCOMB    "Err1"   // Uncombined endpoints can not take position > 0 in register
#define SWBRSR_ERR_RFLASH    "Err2"   // Unable to read endpoint data stored in flash

/**
 * SWAP endpoint browser structure
 */
typedef struct
{
  byte label[6];        // Display label - User configurable
  byte devAddr;         // Device address - User configurable
  byte regAddr;         // Register address - User configurable
  byte regId;           // Register Id - User configurable
  byte format;          // Format byte - User configurable
  byte delta;           // Delta to be applied for each value change (up/down button presses) - User configurable
  byte posize;          // Position and size of the endpoint within the register - User configurable
  u16 value;            // Endpoint value
} SWBRSRENDPOINT;

/**
 *  State of SWAP browser
 */
SWBRSRFLAG swBrowserFlag;

/**
 * swInitBrowser
 *
 * Initialize SWAP browser
 */
extern void swInitBrowser(void);

/**
 * display_swap
 *
 * Display SWAP data browser
 *
 * 'line'   -> LINE2
 * 'update' -> DISPLAY_LINE_UPDATE_FULL
 */
extern void display_swap(u8 line, u8 update);

/**
 * display2_swap
 *
 * Display SWAP data browser, second line
 *
 * 'line'   -> LINE2
 * 'update' -> DISPLAY_LINE_UPDATE_FULL
 */
void display2_swap(u8 line, u8 update);

/**
 * swSetExternalEndpointCfg
 *
 * Set external endpoint settings
 *
 * 'page'     endpoint page
 * 'settings' endpoint settings
 *
 * Return 0 in case of success. -1 otherwise
 */
extern byte swSetExternalEndpointCfg(byte page, byte *settings);

/**
 * swUpdateExternalEndpointVal
 *
 * Update value of external endpoint
 *
 * 'regAddr' Register address
 * 'regId'   Register Id
 * 'regVal'  Register value
 */
extern void swUpdateExternalEndpointVal(byte regAddr, byte regId, SWDATA *regVal);

/**
 * sx_rf
 *
 * Start SWAP connection
 *
 * 'line' u8 line		LINE2
 */
extern void sx_rf(u8 line);

/**
 * sx_synchro
 *
 * Start remote synchronization
 *
 * 'line' u8 line		LINE2
 */
extern void sx_synchro(u8 line);

/**
 * display_synchro
 *
 * Display SYNC mode screen
 *
 * 'line'     LINE2
 * 'update'		DISPLAY_LINE_UPDATE_FULL
 */
extern void display_synchro(u8 line, u8 update);

extern void update_sync(u8 line, u8 update);

extern void update_swapbrws(u8 line, u8 update);

extern void update_swap(u8 line, u8 update);

/**
 * swStopSwapRxTx
 *
 * Stop SWAP comms and clear icons
 */
extern void swStopSwapRxTx(void);

/**
 * swButtonEvent
 *
 * Button event
 */
extern void swButtonEvent(void);
#endif
