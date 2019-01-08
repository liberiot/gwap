/**
 * regtable
 *
 * Copyright (c) 2016 Daniel Berenguer <contact@panstamp.com>
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
 * Creation date: 03/18/2016
 */

#include "product.h"
#include "regtable.h"


/**
 * Declaration of custom functions
 */
const void updtSensor(byte rId);

/**
 * Declaration of custom functions
 */
 
/**
 * Definition of common registers
 */
DEFINE_COMMON_REGISTERS()

/*
 * Definition of custom registers
 */
// Sensor value register
static byte dtSensor[14];
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
  uint16_t voltage = panstamp.getVcc();

  if (bmeSensor.performReading())
  { 
    uint32_t voc = bmeSensor.gas_resistance;
    uint16_t temperature = (bmeSensor.temperature + 50) * 10;
    uint16_t humidity = bmeSensor.humidity * 10;
    uint32_t pressure = bmeSensor.pressure;
   
    // Update register value
    dtSensor[0] = (voltage >> 8) & 0xFF;
    dtSensor[1] = voltage & 0xFF;
    dtSensor[2] = (temperature >> 8) & 0xFF;
    dtSensor[3] = temperature & 0xFF;
    dtSensor[4] = (humidity >> 8) & 0xFF;
    dtSensor[5] = humidity & 0xFF;
    dtSensor[6] = (pressure >> 24) & 0xFF;
    dtSensor[7] = (pressure >> 16) & 0xFF;
    dtSensor[8] = (pressure >> 8) & 0xFF;
    dtSensor[9] = pressure & 0xFF;
    dtSensor[10] = (voc >> 24) & 0xFF;
    dtSensor[11] = (voc >> 16) & 0xFF;
    dtSensor[12] = (voc >> 8) & 0xFF;
    dtSensor[13] = voc & 0xFF;
  }
}

