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
 * Creation date: 05/11/2016
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
// Sensor register
byte dtSensor[6];    // Voltage supply, level and temperature
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
 * updtSensor
 *
 * Update sensor data and transmit status packet
 *
 * 'rId'  Register ID
 */
const void updtSensor(byte rId)
{
  // Read Vcc
  uint32_t voltage = panstamp.getVcc();

  // Adapt temperature value before transmitting
  temperature += 500;
 
  // Update register
  dtSensor[0] = (voltage >> 8) & 0xFF;
  dtSensor[1] = voltage & 0xFF;
  dtSensor[2] = (temperature >> 8) & 0xFF;
  dtSensor[3] = temperature & 0xFF;
  dtSensor[4] = (level >> 8) & 0xFF;
  dtSensor[5] = level & 0xFF;
}

