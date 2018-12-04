/**
 * regtable
 *
 * Copyright (c) 2018 panStamp S.L.U. <contact@panstamp.com>
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
 * Creation date: Nov 30 2018
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
 * updtSensor
 *
 * Measure sensor data and update register
 *
 * 'rId'  Register ID
 */
const void updtSensor(byte rId)
{
  uint32_t voltage;
  
  #ifdef READ_VCC_FROM_A0
  voltage = analogRead(A0);
  voltage *= 3300;
  voltage /= 0xFFF;
  #else
  voltage = panstamp.getVcc();
  #endif

  /**
   * Read on-board temperature + humidity sensor
   */
  // Read humidity
  float h = htu.readHumidity();
  // Read temperature
  float t = htu.readTemperature();

  uint16_t humidity = h * 10;
  uint16_t temperature = (t + 50) * 10;

  dtSensor[0] = (voltage >> 8) & 0xFF;
  dtSensor[1] = voltage & 0xFF;
  dtSensor[2] = (temperature >> 8) & 0xFF;
  dtSensor[3] = temperature & 0xFF;
  dtSensor[4] = (humidity >> 8) & 0xFF;
  dtSensor[5] = humidity & 0xFF;

  /*
   * Read external NTC
   */
  powerThermistorOn();                    // Power thermistor
  uint16_t extTemp = thermistor.read();   // Read temperature from NTC
  powerThermistorOff();                   // Unpower thermistor

  extTemp += 500;
  dtSensor[6] = (extTemp >> 8) & 0xFF;
  dtSensor[7] = extTemp & 0xFF;

}

