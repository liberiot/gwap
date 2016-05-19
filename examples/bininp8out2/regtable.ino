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
 * Definition of common registers
 */
DEFINE_COMMON_REGISTERS()

/*
 * Definition of custom registers
 */
// Binary input register
byte dtBinInput[3];    // Voltage supply and binary input state
REGISTER regBinInput(dtBinInput, sizeof(dtBinInput), &updtBinInput, NULL);

// Binary output registers
byte dtBinOutput0[1];    // 1st binary output
REGISTER regBinOutput0(dtBinOutput0, sizeof(dtBinOutput0), NULL, &setBinOutput);
byte dtBinOutput1[1];    // 2nd binary output
REGISTER regBinOutput1(dtBinOutput1, sizeof(dtBinOutput1), NULL, &setBinOutput);

// Main supply alarm
REGISTER regAlarm(&alarm, sizeof(alarm), NULL, NULL);

/**
 * Initialize table of registers
 */
DECLARE_REGISTERS_START()
  &regBinInput,
  &regBinOutput0,
  &regBinOutput1,
  &regAlarm
DECLARE_REGISTERS_END()

/**
 * Definition of common getter/setter callback functions
 */
DEFINE_COMMON_CALLBACKS()

/**
 * Definition of custom getter/setter callback functions
 */
 
/**
 * updtBinInput
 *
 * Update binary input state and Vcc
 *
 * 'rId'  Register ID
 */
const void updtBinInput(uint8_t rId)
{
  // Read input pin
  uint8_t state = 0, tmp;
  
  for(uint8_t i=0 ; i<NBOF_INPUTS ; i++)
  {
    tmp = digitalRead(inputPin[i]);
    
    if (!tmp) // invert state
      state |= (1 << i);
    else
      state &= ~(1 << i);
  }
  
  // Read Vcc
  uint32_t voltage = panstamp.getVcc();
  
  // Update register
  dtBinInput[0] = (voltage >> 8) & 0xFF;
  dtBinInput[1] = voltage & 0xFF;
  dtBinInput[2] = state;
}

/**
 * setBinOutput
 *
 * Set binary output
 *
 * 'rId'     Register ID
 * 'state'   New output state
 */
const void setBinOutput(uint8_t rId, uint8_t *state)
{
  uint8_t output = rId - REGI_BINOUTPUT0;

  // Update register
  regTable[rId]->value[0] = state[0];

  // Control pin
  digitalWrite(outputPin[output], state[0]);
}

