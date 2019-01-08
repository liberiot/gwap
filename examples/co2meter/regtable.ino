/**
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
 * Creation date: Dec 12 2018
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
static byte dtSensor[10];
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

  // Power light sensor
  digitalWrite(TSL_PWR_PIN, HIGH);
  delay(10);
  // Initialize light sensor
  tsl.begin();
  tsl.enableAutoRange(true);  // Auto-gain
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS); // Fast and low res
  // Read light level
  sensors_event_t event;
  tsl.getEvent(&event);
  int light = (int)event.light;
  dtSensor[6] = (light >> 8) & 0xFF;
  dtSensor[7] = light & 0xFF;
  // Unpower light sensor
  digitalWrite(TSL_PWR_PIN, LOW);

  dtSensor[8] = (co2Level >> 8) & 0xFF;
  dtSensor[9] = co2Level & 0xFF;
}

