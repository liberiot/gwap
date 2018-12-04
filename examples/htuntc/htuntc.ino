/*
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
 *
 * Device:
 * Dual Temperature + Humidity sensor + external temperature sensor
 *
 * Description:
 * This sketch generates a GWAP temperature+humidity sensor device
 * relying on HTU21D sensor and Sparkfun's library
 * https://github.com/sparkfun/HTU21D_Breakout
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
 * 00010009.json (Humidity + 2 x Temperature sensor)
 */

#include "Wire.h"
#include "gwap.h"
#include "regtable.h"
#include <SparkFunHTU21D.h>
#include <thermistor.h>
                     

//Create an instance of the sensor object
HTU21D htu;

// NTC power pin
#define NTC_POWER_PIN   10
// NTC ADC pin
#define NTC_PIN    A1

// Macros
#define powerThermistorOn()   digitalWrite(NTC_POWER_PIN, HIGH)
#define powerThermistorOff()  digitalWrite(NTC_POWER_PIN, LOW)

// Thermistor object
THERMISTOR thermistor(NTC_PIN,        // Analog pin
                      10000,          // Nominal resistance at 25 ºC
                      3950,           // thermistor's beta coefficient
                      10000);         // Value of the series resistor
                      

void setup()
{
  pinMode(NTC_POWER_PIN, OUTPUT);    // Configure power pin. This pin will be used to power the thermistor
  powerThermistorOff();              // Unpower sensor by default
  
  // Init GWAP stack
  gwap.init();

  // AES-128 encryption (for NRG only)
  //byte aesKey[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};  
  //gwap.setAesPassword(aesKey);
  
  // Initialize LED pins
  pinMode(LED, OUTPUT);
 
  // Initialize sensor
  htu.begin();
  
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

