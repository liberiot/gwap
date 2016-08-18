/**
 * regtable
 *
 * Copyright (c) 2016 Daniel Berenguer <dberenguer@usapiens.com>
 * 
 * This file is part of the panStamp project.
 * 
 * panStamp  is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 * 
 * panStamp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with panStamp; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 
 * USA
 * 
 * Author: Daniel Berenguer
 * Creation date: 04/06/2016
 */

#include "product.h"
#include "regtable.h"

/**
 * Declaration of custom functions
 */
const void updtVoltSupply(byte rId);
const void updtBinInput(byte rId);

/**
 * Definition of common registers
 */
DEFINE_COMMON_REGISTERS()

/*
 * Definition of custom registers
 */
// Binary input register
byte dtBinInput[3];    // Voltage supply and binary input state
REGISTER regBinInput(dtBinInput, sizeof(dtBinInput), &updtBinInput, NULL);

/**
 * Initialize table of registers
 */
DECLARE_REGISTERS_START()
  &regBinInput
DECLARE_REGISTERS_END()

/**
 * Definition of common getter/setter callback functions
 */
DEFINE_COMMON_CALLBACKS()

/**
 * Definition of custom getter/setter callback functions
 */
 
/**
 * updtVoltSupply
 *
 * Measure voltage supply and update register
 *
 * 'rId'  Register ID
 */
const void updtVoltSupply(byte rId)
{
  uint16_t result = panstamp.getVcc();

  /**
   * register[eId]->member can be replaced by regVoltSupply in this case since
   * no other register is going to use "updtVoltSupply" as "updater" function
   */

  // Update register value
  regTable[rId]->value[0] = (result >> 8) & 0xFF;
  regTable[rId]->value[1] = result & 0xFF;
}

/**
 * updtBinInput
 *
 * Update binary input state and Vcc
 *
 * 'rId'  Register ID
 */
const void updtBinInput(byte rId)
{
  // Read input pin
  uint8_t state = digitalRead(INPUT_PIN);
  
  // Read Vcc
  uint32_t voltage = panstamp.getVcc();
  
  // Update register
  dtBinInput[0] = (voltage >> 8) & 0xFF;
  dtBinInput[1] = voltage & 0xFF;
  dtBinInput[2] = state;
}

