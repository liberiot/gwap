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
 * Author: Daniel Berenguer
 * Creation date: 03/31/2016
 */

#include "product.h"
#include "panstamp.h"
#include "regtable.h"

/**
 * Definition of common registers
 */
DEFINE_COMMON_REGISTERS()

/*
 * Definition of custom registers
 */
// Sensor readings : Vcc (2-byte), temperature (2-byte) and 3 x RMS current (4-byte)
static byte dtSensor[16];
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
  uint32_t batt = analogRead(A0);
  batt *= panstamp.getVcc() * 2;
  batt /= 4095;

  // Update register
  dtSensor[0] = (batt >> 8) & 0xFF;
  dtSensor[1] = batt & 0xFF;

  // Read temperature
  uint16_t temp;
  powerThermistorOn();        // Power thermistor
  temp = thermistor.read();   // Read temperature
  powerThermistorOff();       // Unpower thermistor
  temp += 500;

  // Update register
  dtSensor[2] = (temp >> 8) & 0xFF;
  dtSensor[3] = temp & 0xFF;
  
  // Channel 0 RMS current (mA)
  dtSensor[4] = (channel0.rmsCurrent >> 24) & 0xFF;
  dtSensor[5] = (channel0.rmsCurrent >> 16) & 0xFF;
  dtSensor[6] = (channel0.rmsCurrent >> 8) & 0xFF;
  dtSensor[7] = channel0.rmsCurrent & 0xFF;
  
  // Channel 1 RMS current (mA)
  dtSensor[8] = (channel1.rmsCurrent >> 24) & 0xFF;
  dtSensor[9] = (channel1.rmsCurrent >> 16) & 0xFF;
  dtSensor[10] = (channel1.rmsCurrent >> 8) & 0xFF;
  dtSensor[11] = channel1.rmsCurrent & 0xFF;
  
  // Channel 2 RMS current (mA)
  dtSensor[12] = (channel2.rmsCurrent >> 24) & 0xFF;
  dtSensor[13] = (channel2.rmsCurrent >> 16) & 0xFF;
  dtSensor[14] = (channel2.rmsCurrent >> 8) & 0xFF;
  dtSensor[15] = channel2.rmsCurrent & 0xFF;
}

