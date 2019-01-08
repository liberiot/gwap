/*
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
 * Creation date: 09/10/2018
 *
 * Device:
 * Dual Temperature + Humidity sensor
 *
 * Description:
 * This sketch generates a GWAP temperature+humidity+pressure+VOC sensor device
 * relying on the BME680 sensor made by Bosch and Adafruit's library
 * https://github.com/adafruit/Adafruit_BME680
 *
 * These devices are low-power enabled so they will enter low-power mode
 * just after reading the sensor values and transmitting them over the
 * GWAP network.
 *
 * Associated Device Definition Files, defining registers, endpoints and
 * configuration parameters:
 * 00010038.json ("Indoor Air Quality meter (T+H+P+VOC)")
 */

#include "Wire.h"
#include "gwap.h"
#include "regtable.h"

#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>

// Create an instance of the sensor object
Adafruit_BME680 bmeSensor;

void setup()
{
  // Init GWAP stack
  gwap.init();

  // AES-128 encryption (for NRG only)
  //byte aesKey[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};  
  //gwap.setAesPassword(aesKey);
  
  // Initialize LED pins
  pinMode(LED, OUTPUT);
 
  // Initialize sensor
  if (bmeSensor.begin())
  {    
    // Set up oversampling and filter initialization
    bmeSensor.setTemperatureOversampling(BME680_OS_8X);
    bmeSensor.setHumidityOversampling(BME680_OS_2X);
    bmeSensor.setPressureOversampling(BME680_OS_4X);
    bmeSensor.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bmeSensor.setGasHeater(320, 150); // 320*C for 150 ms
  }

  // Enter SYNC state
  gwap.enterSystemState(SYSTATE_SYNC);

  // During 3 seconds, listen the network for possible commands whilst the LED blinks
  for(uint8_t i=0 ; i<6 ; i++)
  {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(400);
  }

  // Transmit periodic Tx interval
  gwap.getRegister(REGI_TXINTERVAL)->getData();
  delay(GWAP_TX_SILENCE);
   // Switch to Rx OFF state
  gwap.enterSystemState(SYSTATE_RXOFF);
  delay(GWAP_TX_SILENCE);
}

void loop()
{
  digitalWrite(LED, HIGH);
  // Transmit sensor data
  gwap.getRegister(REGI_SENSOR)->getData();
  digitalWrite(LED, LOW);

  // Sleep
  gwap.goToSleep();
}

