/**
 * Copyright (c) 2019 panStamp S.L.U. <contact@panstamp.com>
 * 
 * This file is part of the panStamp project.
 * 
 * panStamp  is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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
 * Creation date: Jan 8 2019
 */

#include "product.h"
#include "panstamp.h"
#include "regtable.h"

/**
 * Declaration of custom functions
 */
const void updtSensor(byte rId);

/**
 * Definition of common registers
 */
DEFINE_COMMON_REGISTERS()

/*
 * Definition of custom registers
 */
// Sensor readings : Vcc (2-byte), millilitres (4-byte) and water pressure (2-byte)
static byte dtSensor[8];
REGISTER regSensor(dtSensor, sizeof(dtSensor), &updtSensor, NULL);

/**
 * Initialize table of registers
 */
DECLARE_REGISTERS_START()
  &regSensor
DECLARE_REGISTERS_END()

/**
 * Definition of common getter/setter callback functions
 */
DEFINE_COMMON_CALLBACKS()

/**
 * Definition of custom getter/setter callback functions
 */

/**
 * updtCurrent
 *
 * Update register with readings
 *
 * @param rId  Register ID
 */
const void updtSensor(byte rId)
{ 
  // Read voltage
  uint16_t voltage = panstamp.getVcc();

  // Update register
  dtSensor[0] = (voltage >> 8) & 0xFF;
  dtSensor[1] = voltage & 0xFF;
  dtSensor[2] = (totalMilliLitres >> 24) & 0xFF;
  dtSensor[3] = (totalMilliLitres >> 16) & 0xFF;
  dtSensor[4] = (totalMilliLitres >> 8) & 0xFF;
  dtSensor[5] = totalMilliLitres & 0xFF;
  dtSensor[6] = (waterPressure >> 8) & 0xFF;
  dtSensor[7] = waterPressure & 0xFF;
}
