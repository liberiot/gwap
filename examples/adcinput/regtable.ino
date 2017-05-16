/**
 * Copyright (c) 2016 panStamp S.L.U. <contact@panstamp.com>
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
 * Author: Francisco Barrena
 * Creation date: 05/16/2017
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
// Sensor value register
static byte dtSensor[4];
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
 * Measure sensor data and update register
 *
 * 'rId'  Register ID
 */
const void updtSensor(byte rId)
{
  uint32_t voltage = panstamp.getVcc();
  
  digitalWrite(POWER_PIN, HIGH);         // Power external circuit
  analogReference(DEFAULT);              // Vcc analog reference
  uint16_t inp = analogRead(ADC_PIN);    // Read ADC pin
  digitalWrite(POWER_PIN, LOW);          // Unpower external circuit

  // Update register value
  dtSensor[0] = (voltage >> 8) & 0xFF;
  dtSensor[1] = voltage & 0xFF;
  dtSensor[2] = (inp >> 8) & 0xFF;
  dtSensor[3] = inp & 0xFF;
}

