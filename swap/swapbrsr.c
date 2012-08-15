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
#include "swapbrsr.h"
#include "regtable.h"
#include "params.h"
#include "periodic.h"

// driver
#include "ports.h"
#include "display.h"
//#include "adc12.h"
#include "timer.h"
#include "vti_as.h"

// logic
#include "menu.h"
#include "user.h"
#include "acceleration.h"

/**
 * Macros
 */
#define increaseEndpointVal()   changeEndpointVal(UP)
#define decreaseEndpointVal()   changeEndpointVal(DOWN)
#define currentEndpoint         swExternEndpoints[pageNumber]
#define displayExternalLabel()  display2_swap(LINE2, DISPLAY_LINE_UPDATE_FULL)
#define setControlMode()        swBrowserFlag = SWAP_CONTROL; display_symbol(LCD_ICON_HEART, SEG_ON_BLINK_ON)
#define setBrowserMode()        swBrowserFlag = SWAP_BROWSER; display_symbol(LCD_ICON_HEART, SEG_ON_BLINK_OFF)

// Query current endpoint
#define QUERY_CURRENT_ENDPOINT()        \
      if (currentEndpoint.devAddr != 0) \
      {                                 \
        swStart();                      \
        swQryRegister(currentEndpoint.devAddr, currentEndpoint.regAddr, currentEndpoint.regId); \
        swStop();                       \
      }

// Command current endpoint
#define COMMAND_CURRENT_ENDPOINT(val)   \
      if (currentEndpoint.devAddr != 0) \
      {                                 \
        swStart();                      \
        swCmdRegister(currentEndpoint.devAddr, currentEndpoint.regAddr, currentEndpoint.regId, val); \
        swStop();                       \
      }

/**
 * Global variables
 */
static SWBRSRENDPOINT swExternEndpoints[SWAP_BROWSER_MAX_PAGES];

/**
 * Functions
 */
static void swCloseBrowser(void);
static void initSwapPages(void);
static void displayExternalEndpoint(void);
static void changeEndpointVal(bool direction);

/**
 * Extern functions
 */
extern void ezGoToFirstMenuItem(void);
extern void ezSetDisplayFunct(line_t line, void *dispFunct);

/**
 * Browsing page
 */
byte pageNumber = 0;

/**
 * Amount of "external endpoint" pages
 */
byte nbOfPages = SWAP_BROWSER_MAX_PAGES;

/**
 * swInitBrowser
 *
 * Initialize SWAP browser
 */
void swInitBrowser(void)
{
  // Initialize board
  BSP_Init();

  // Stop acceleration sensor
  as_stop();

  // Init SWAP comms
	swInit();

  // Transmit product code
  swSendInfoPacket(REGI_PRODUCTCODE);
  swStop();

  // Initial SWAP state = SWAP stopped
  setChronosState(SYSTATE_RXOFF);

  swBrowserFlag = SWAP_OFF;

  // Initialize SWAP pages
  initSwapPages();
}

/**
 * swCloseBrowser
 *
 * Close SWAP browser
 */
static void swCloseBrowser(void)
{
  // Start comms
  swStart();
  // Stop SWAP comms and clear icons
  swStopSwapRxTx();

  // Disable SWAP browser
  swBrowserFlag = SWAP_OFF;

  // Turn the rest of icons off
  display_symbol(LCD_ICON_HEART, SEG_OFF);
  display_symbol(LCD_SYMB_ARROW_UP, SEG_OFF);
  display_symbol(LCD_SYMB_ARROW_DOWN, SEG_OFF);
}



/**
 * display_swap
 *
 * Display SWAP data browser
 *
 * 'line'   -> LINE1
 * 'update' -> DISPLAY_LINE_UPDATE_FULL
 */
void display_swap(u8 line, u8 update)
{
	if (update == DISPLAY_LINE_UPDATE_FULL)	
	{
    clear_display_all();

    // Set L2 menu
    ptrMenu_L2 = &menu_L2_SWAP;
    ezSetDisplayFunct(LINE2, &display2_swap);

    swBrowserFlag = SWAP_BROWSER;

    // Send Product Code info
    swInit();
    swSendInfoPacket(REGI_PRODUCTCODE);
    swStop();

    // Query current endpoint
    QUERY_CURRENT_ENDPOINT()

    // Display external value
    displayExternalEndpoint();
	}
}

/**
 * display2_swap
 *
 * Display SWAP data browser, second line
 *
 * 'line'   -> LINE2
 * 'update' -> DISPLAY_LINE_UPDATE_FULL
 */
void display2_swap(u8 line, u8 update)
{
  byte label[7];
  // Display endpoint label
  label[0] = ' ';
  memcpy(label+1, currentEndpoint.label, sizeof(currentEndpoint.label));
  display_chars(LCD_SEG_L2_5_0, label, SEG_ON);
}

/**
 * initSwapPages
 *
 * Initialize SWAP browsing pages
 */
static void initSwapPages(void)
{
  int i;

  for(i=0 ; i<nbOfPages ; i++)
  {
    // Text label
    memcpy(swExternEndpoints[i].label, "PAGE", 4);
    swExternEndpoints[i].label[4] = 0x30 + i;
    swExternEndpoints[i].label[5] = 0;

    // Addresses
    swExternEndpoints[i].devAddr = 0;
    swExternEndpoints[i].regAddr = 0;

    // Register id
    swExternEndpoints[i].regId = 0;

    // Endpoint format
    swExternEndpoints[i].format = 0;

    // Delta, position and size
    swExternEndpoints[i].delta = 0;
    swExternEndpoints[i].posize = 0;

    // Values
    swExternEndpoints[i].value = 0;
  }
}

/**
 * displayExternalEndpoint
 *
 * Display external endpoint data
 */
static void displayExternalEndpoint(void)
{
  byte *str = NULL;

  clear_display_all();
  display_symbol(LCD_SYMB_ARROW_UP, SEG_OFF);
  display_symbol(LCD_SYMB_ARROW_DOWN, SEG_OFF);
 	display_symbol(LCD_SEG_L1_DP1, SEG_OFF);

  // Filter errors
  byte b = currentEndpoint.getSize();
  if (b > sizeof(u16))
    str = (byte*) SWBRSR_ERR_SIZE;
  else if (!(currentEndpoint.isCombined()))
  {
    b = currentEndpoint.getPosition();
    if (b > 0)
      str = (byte*) SWBRSR_ERR_NOCOMB;
  }

  // Everything seems t be correct so far
  if (str == NULL)
  {
    // Analog endpoint
    if (currentEndpoint.isAnalog())
    {
      // Add decimal point?
      if (currentEndpoint.isDecimal())
      	display_symbol(LCD_SEG_L1_DP1, SEG_ON);
      // Signed value?
      if (currentEndpoint.isSigned())
      {
        // Add UP or DOWN arrows
        if (currentEndpoint.isPositive())
          display_symbol(LCD_SYMB_ARROW_DOWN, SEG_ON);
        else
          display_symbol(LCD_SYMB_ARROW_UP, SEG_ON);
      }
      //TODO abs should use gcc function?
      if(currentEndpoint.value>0)
    	  str = _itoa(currentEndpoint.value, 4, 4);
      else
    	  str = _itoa(-currentEndpoint.value, 4, 4);
    }
    // Binary endpoint?
    else
    {
      if (currentEndpoint.value == OFF)
        str = (byte*) " OFF";
      else
        str = (byte*) "  ON";
    }

    // Output endpoint?
    if (currentEndpoint.isOutput())
      display_symbol(LCD_ICON_HEART, SEG_ON);
    else
      display_symbol(LCD_ICON_HEART, SEG_OFF);
  }
  
  display_chars(LCD_SEG_L1_3_0, str, SEG_ON);

  // Display external label
  displayExternalLabel();
}

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
byte swSetExternalEndpointCfg(byte page, byte *settings)
{
  if (page >= nbOfPages)
    return -1;

  // Text label
  memcpy(swExternEndpoints[page].label, settings, 5);
  swExternEndpoints[page].label[5] = 0;

  // Device address
  swExternEndpoints[page].devAddr = settings[5];
  // Register address
  swExternEndpoints[page].regAddr = settings[6];
  // Register Id
  swExternEndpoints[page].regId = settings[7];
  // Format byte
  swExternEndpoints[page].format = settings[8];
  // Delta
  swExternEndpoints[page].delta = settings[9];
  // Position and size byte
  swExternEndpoints[page].posize = settings[10];

  return 0;
}

/**
 * swUpdateExternalEndpointVal
 *
 * Update value of external endpoint
 *
 * 'regAddr' Register address
 * 'regId'   Register Id
 * 'regVal'  Register value
 */
void swUpdateExternalEndpointVal(byte regAddr, byte regId, SWDATA *regVal)
{
  int i, j, j0, size;

  for(i=0 ; i<nbOfPages ; i++)
  {
    if ((swExternEndpoints[i].regAddr == regAddr) && (swExternEndpoints[i].regId == regId))
    {
      size = swExternEndpoints[i].getSize();
      if (size > sizeof(u16))
        size = sizeof(u16);

      j0 = swExternEndpoints[i].getPosition();

      // Exit function if the actual register size is smaller than expected
      if (regVal->length < (j0 + size))
        return;

      // Update endpoint value
      swExternEndpoints[i].value = 0;
      for(j=0 ; j<size ; j++)
        swExternEndpoints[i].value |= regVal->data[j+j0] << (8*(size-1) - 8*j);

      // Endpoint currently being viewed?
      if (i == pageNumber)
        displayExternalEndpoint();
    }
  }
}

/**
 * sx_rf
 *
 * Start SWAP connection
 *
 * 'line' u8 line		LINE2
 */
void sx_rf(u8 line)
{
	#ifdef CONFIG_ACCEL
  if (!isAccTxMode())
  {
    // Exit if battery voltage is too low for radio operation
    if (sys.flag.low_battery)
      return;

    // Transmit product code and system state
	  swStart();
    swSendInfoPacket(REGI_PRODUCTCODE);
    setChronosState(SYSTATE_SENDACC);
    DELAY_MS(50);
    swSendInfoPacket(REGI_SYSSTATE);
    swStop();

    // Start blinking the RF icon
    display_symbol(LCD_ICON_BEEPER1, SEG_ON_BLINK_ON);
    display_symbol(LCD_ICON_BEEPER2, SEG_ON_BLINK_ON);
    display_symbol(LCD_ICON_BEEPER3, SEG_ON_BLINK_ON);

    // Start transmission of ACC data
    startAccTx();
  }
  else
  {
    // Stop transmission of ACC data
    stopAccTx();
    // Stop acceleration sensor
    as_stop();

    // Stop blinking the RF icon
    display_symbol(LCD_ICON_BEEPER1, SEG_OFF);
    display_symbol(LCD_ICON_BEEPER2, SEG_OFF);
    display_symbol(LCD_ICON_BEEPER3, SEG_OFF);

    // Transmit system state
	  swStart();
    setChronosState(SYSTATE_RXOFF);
    swSendInfoPacket(REGI_SYSSTATE);
    swStop();
  }
	#endif
}

/**
 * sx_synchro
 *
 * Start remote synchronization
 *
 * 'line' u8 line		LINE2
 */
void sx_synchro(u8 line)
{
  if (getChronosState() != SYSTATE_SYNC)
  {
    swStart();
    swSendInfoPacket(REGI_PRODUCTCODE);
    setChronosState(SYSTATE_SYNC);
    DELAY_MS(50);
    swSendInfoPacket(REGI_SYSSTATE);

    // Start blinking the RF icon
    display_symbol(LCD_ICON_BEEPER1, SEG_ON_BLINK_ON);
    display_symbol(LCD_ICON_BEEPER2, SEG_ON_BLINK_ON);
    display_symbol(LCD_ICON_BEEPER3, SEG_ON_BLINK_ON);
  }
  else
    swStopSwapRxTx();
}

/**
 * swStopSwapRxTx
 *
 * Stop SWAP comms and clear icons
 */
void swStopSwapRxTx(void)
{
  // Set new system state
  setChronosState(SYSTATE_RXOFF);
  // Send info message before stopping SWAP comms
  swSendInfoPacket(REGI_SYSSTATE);
  // Stop SWAP comms
  swStop();

  // Stop blinking the RF icon
  display_symbol(LCD_ICON_BEEPER1, SEG_OFF);
  display_symbol(LCD_ICON_BEEPER2, SEG_OFF);
  display_symbol(LCD_ICON_BEEPER3, SEG_OFF);

  // Go back to the first menu
  ezGoToFirstMenuItem();
}

/**
 * display_rf
 *
 * Display ACC mode screen
 *
 * 'line'     LINE2
 * 'update'		DISPLAY_LINE_UPDATE_FULL
 */
void display_rf(u8 line, u8 update)
{
	if (update == DISPLAY_LINE_UPDATE_FULL)	
	{
		display_chars(LCD_SEG_L2_5_0, (u8 *)"   ACC", SEG_ON);
	}
}

/**
 * display_synchro
 *
 * Display SYNC mode screen
 *
 * 'line'     LINE2
 * 'update'		DISPLAY_LINE_UPDATE_FULL
 */
void display_synchro(u8 line, u8 update)
{
	if (update == DISPLAY_LINE_UPDATE_FULL)	
	{
		display_chars(LCD_SEG_L2_5_0, (u8 *)"  SYNC", SEG_ON);
	}
}

/**
 * swButtonEvent
 *
 * Button event
 */
void swButtonEvent(void)
{
	// Process long button press event (while button is held)
	if (button.flag.star_long)
	{
		// Clear button event
		button.flag.star_long = 0;
	}
	else if (button.flag.num_long)
	{
		// Clear button event
		button.flag.num_long = 0;
	}
	// Process single button press event (after button was released)
	else if (button.all_flags)
	{
		// M1 button event ---------------------------------------------------------------------
		// (Short) Exit SWAP mode
		if(button.flag.star) 
		{
			// Clear button flag
			button.flag.star = 0;
      // Close SWAP browser
      swCloseBrowser();
		}
		// NUM button event ---------------------------------------------------------------------
		// (Short) Enable control over the current endpoint
		else if(button.flag.num) 
		{
			// Clear button flag
			button.flag.num = 0;

      if (swBrowserFlag == SWAP_BROWSER)
      {
        if (currentEndpoint.isOutput())
        {
          setControlMode();
        }
      }
      else if (swBrowserFlag == SWAP_CONTROL)
      {
        setBrowserMode();
      }
		}	
		// UP button event ---------------------------------------------------------------------
		// Activate user function for Line1 menu item
		else if(button.flag.up) 	
		{
			// Clear button flag	
			button.flag.up = 0;

      if (swBrowserFlag == SWAP_BROWSER)
      {
        if (pageNumber == nbOfPages - 1)
          pageNumber = 0;
        else
          pageNumber++;

        // Query actual register value
        QUERY_CURRENT_ENDPOINT();

        // Display endpoint data
        displayExternalEndpoint();
      }
      else if (swBrowserFlag == SWAP_CONTROL)
        increaseEndpointVal();
		}			
		// DOWN button event ---------------------------------------------------------------------
		// Activate user function for Line2 menu item
		else if(button.flag.down) 	
		{
			// Clear button flag	
			button.flag.down = 0;

      if (swBrowserFlag == SWAP_BROWSER)
      {
        if (pageNumber == 0)
          pageNumber = nbOfPages - 1;
        else
          pageNumber--;

        // Query actual register value
        QUERY_CURRENT_ENDPOINT();

        // Display endpoint data
        displayExternalEndpoint();
      }
      else if (swBrowserFlag == SWAP_CONTROL)
        decreaseEndpointVal();
		}			
	}
}

/**
 * changeEndpointVal
 *
 * Change endpoint value depending on its settings
 *
 * 'direction'  UP or DOWN
 */
static void changeEndpointVal(bool direction)
{
  byte i;
  u16 newEndpVal;
  byte bData[SWAP_MAX_REG_DATA_LEN];
  SWDATA newRegVal, *currRegVal;

  newRegVal.data = bData;

  // Get new endpoint value
  if (direction == UP)
  {
    // In case of analog endpoint
    if (currentEndpoint.isAnalog())
      newEndpVal = currentEndpoint.value + currentEndpoint.delta;
    else
      newEndpVal = ON;
  }
  else  // DOWN
  {
    // In case of analog endpoint
    if (currentEndpoint.isAnalog())
      newEndpVal = currentEndpoint.value - currentEndpoint.delta;
    else
      newEndpVal = OFF;
  }
  // If the endpoint value is combined with other values within the same register
  if (currentEndpoint.isCombined())
  {
    // Query current register value
    swStart();
    currRegVal = swQryRegister(currentEndpoint.devAddr, currentEndpoint.regAddr, currentEndpoint.regId);
    swStop();

    if (currRegVal == NULL)
      return;   // Device not available. Abort operation

    byte pos = currentEndpoint.getPosition();
    byte size = currentEndpoint.getSize();

    newRegVal.length = currRegVal->length;

    for(i=0 ; i<newRegVal.length ; i++)
    {
      if (i >= pos && i < (pos+size))
        newRegVal.data[i] = (newEndpVal >> (8*(size-1) - 8*(i-pos))) & 0xFF;
      else
        newRegVal.data[i] = currRegVal->data[i];
    }
  }
  // If the register only contains our endpoint value
  else
  {
    newRegVal.length = currentEndpoint.getSize();
    for(i=0 ; i<newRegVal.length ; i++)
      newRegVal.data[i] = (newEndpVal >> (8*(newRegVal.length-1) - 8*i)) & 0xFF;
  }

  // Send command packet
  COMMAND_CURRENT_ENDPOINT(&newRegVal);
}

