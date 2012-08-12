/**
 * register.h
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

#ifndef _REGISTER_H
#define _REGISTER_H

#include "swap.h"

/**
 * Struct: REGISTER
 * 
 * Description:
 * ez430-Chronos internal register structure
 */
typedef struct
{
  /**
   * Register id
   */
  const byte id;
  
  /**
   * Register value
   */
  SWDATA *value;

  /**
   * Pointer to the register "updater" function
   *
   * 'id' Register id
   */
  const void (*updateValue)(byte id);

  /**
   * Pointer to the register "setter" function
   *
   * 'id' Register id
   * 'val'  New register value
   */
  const void (*setValue)(byte id, byte *val);

} REGISTER;

/**
 * Extern global functions
 */
extern void swSetRegValue(REGISTER *reg, byte *val);

#endif

