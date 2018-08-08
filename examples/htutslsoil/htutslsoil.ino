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
 * Creation date: 12/19/2017
 *
 * Device:
 * Dual Temperature + Humidity sensor
 *
 * Description:
 * This sketch generates a GWAP temperature+humidity sensor device
 * relying on HTU21D sensor and Sparkfun's library
 * https://github.com/sparkfun/HTU21D_Breakout
 * 
 * and Adafruit's TSL2561 sensor library:
 * https://github.com/adafruit/Adafruit_TSL2561
 *
 * This library and the current sketch is also compatible with Silabs'
 * SI7021 temperature+humidity sensor. SI7021 is the sensor that can be
 * hosted by panStamp NRG on the bottom layer.
 *
 * These devices are low-power enabled so they will enter low-power mode
 * just after reading the sensor values and transmitting them over the
 * GWAP network.
 *
 * Associated Device Definition Files, defining registers, endpoints and
 * configuration parameters:
 * 00010001.json (Humidity + Temperature + Light level sensor)
 */

#include "Wire.h"
#include "gwap.h"
#include "regtable.h"
#include <SparkFunHTU21D.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

// Uncomment next line if you are running this application from a
// battery-board
//#define READ_VCC_FROM_A0  1

// Power pin to light sensor
#define TSL_PWR_PIN  22
// Power pin to soil moisture sensor
#define SOIL_PWR_PIN  19
// ADC pin to soil moisture sensor
#define SOIL_ADC_PIN  A3

//Create an instance of the sensor object
HTU21D htu;

// Light sensor object
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

void setup()
{
  // Config TSL sensor power pin
  pinMode(TSL_PWR_PIN, OUTPUT);
  digitalWrite(TSL_PWR_PIN, LOW);

  // Config soil moisture sensor power pin
  pinMode(SOIL_PWR_PIN, OUTPUT);
  digitalWrite(SOIL_PWR_PIN, LOW);
  
  // Init GWAP stack
  gwap.init();

  // AES-128 encryption (for NRG only)
  //byte aesKey[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};  
  //gwap.setAesPassword(aesKey);
  
  // Initialize LED pins
  pinMode(LED, OUTPUT);
 
  // Initialize sensor
  htu.begin();
  

  // Initialize light sensor
  tsl.begin();
  tsl.enableAutoRange(true);  // Auto-gain
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS); // Fast and low res
  
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
//  digitalWrite(LED, HIGH);
  // Transmit sensor data
  gwap.getRegister(REGI_SENSOR)->getData();
//  digitalWrite(LED, LOW);

  // Sleep
  gwap.goToSleep();
}

