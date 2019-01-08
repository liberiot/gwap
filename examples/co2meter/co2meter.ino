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
 * Creation date: Dec 12 2018
 *
 * Device:
 * Dual Temperature + Humidity sensor + external temperature sensor
 *
 * Description:
 * This sketch generates a GWAP temperature+humidity sensor device
 * relying on HTU21D sensor and Sparkfun's library
 * https://github.com/sparkfun/HTU21D_Breakout
 *
 * Device:
 * CO2 + Temperature + Humidity sensor
 *
 * Description:
 * This application measures:
 *  - Ambient CO2 level from a 0-2000 ppm COZIR sensor
 *    (http://www.co2meter.com/products/cozir-0-2-co2-sensor)
 *  - Temperature and humidity from SI7021 sensor
 *
 * These devices are low-power enabled so they will enter low-power mode
 * just after reading the sensor values and transmitting them over the
 * GWAP network.
 *
 * Associated Device Definition File, defining registers, endpoints and
 * configuration parameters:
 * 0001000A.json (CO2 Humidity + Temperature sensor)
 */
 
#include "Wire.h"
#include "gwap.h"
#include "regtable.h"
#include <SparkFunHTU21D.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <HardwareSerial.h>

// Power pin to light sensor
#define TSL_PWR_PIN  22

// Power pin to CO2 sensor
#define CO2_ENABLE_PIN   19
#define CO2_AUX_PIN      11

uint16_t co2Level = 0; // Holds the actual CO2 value

bool zeroCalibrate = false; // If true, start zero calibration

// Humidity and temperature sensor
HTU21D htu;

// Light sensor object
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

/**
 * readCO2
 *
 * Read CO2 level
 *
 * @return Current CO2 level
 */
uint16_t readCO2(void) 
{
  char co2Buffer[25];
  uint8_t co2Index = 0;
  char  ch;
  bool start = false, stop = false;

  // Enable CO2 sensor
  digitalWrite(CO2_ENABLE_PIN, HIGH);
  digitalWrite(CO2_AUX_PIN, HIGH);

  // Open serial port
  Serial.begin(9600);
  
  delay(500);
  
  while(!stop)
  {
    if(Serial.available())
    { 
      ch = Serial.read();
      switch (ch)
      {
        case 'z':
          start = true;     
          break;
        case '\r':
          co2Buffer[co2Index] = 0;
          start = false;
          stop = true;
          break;
        default:
          if (start)
          {
            if(ch != 0x20)
              co2Buffer[co2Index++] = ch;
          }    
          break;          
      }
    }
  }

  // close serial port
  Serial.end();
  delay(2);
  
  // Disable CO2 sensor
  digitalWrite(CO2_ENABLE_PIN, LOW);
  digitalWrite(CO2_AUX_PIN, LOW);
  
  return (uint16_t) atoi(co2Buffer);
}

/**
 * zeroCalibration
 * 
 * Zero-calibrate CO2 sensor
 */
void zeroCalibration(void)
{
  zeroCalibrate = false;
  digitalWrite(LED, HIGH);
  // Enable CO2 sensor
  digitalWrite(CO2_ENABLE_PIN, HIGH);
  digitalWrite(CO2_AUX_PIN, HIGH);
  
  Serial.begin(9600);
  
  // Start zero calibration
  Serial.println("");
  delay(500);
  Serial.println("K 2");
  delay(1000);
  Serial.println("G");
  delay(1000);
  Serial.println("K 1");
  delay(1000);
   
  // disable CO2 sensor
  digitalWrite(CO2_ENABLE_PIN, LOW);
  digitalWrite(CO2_AUX_PIN, LOW);

  Serial.end();

  digitalWrite(LED, LOW);
}
 
/**
 * setup
 *
 * Arduino setup function
 */
void setup()
{
  pinMode(TSL_PWR_PIN, OUTPUT);
  digitalWrite(TSL_PWR_PIN, LOW);
  
  pinMode(CO2_ENABLE_PIN, OUTPUT);
  digitalWrite(CO2_ENABLE_PIN, LOW);
  pinMode(CO2_AUX_PIN, OUTPUT);
  digitalWrite(CO2_AUX_PIN, LOW);
  
  // Init GWAP stack
  gwap.init();
  
  // Initialize Hhumidity+temperature sensor
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

  // Calibrate sensor?
  if (zeroCalibrate)
    zeroCalibration();

  // Transmit periodic Tx interval
  gwap.getRegister(REGI_TXINTERVAL)->getData();
  delay(GWAP_TX_SILENCE);
   // Switch to Rx OFF state
  gwap.enterSystemState(SYSTATE_RXOFF);
  delay(GWAP_TX_SILENCE);
}

/**
 * loop
 *
 * Arduino main loop
 */
void loop()
{
  // Read CO2 sensor
  co2Level = readCO2();
  // Transmit sensor data
  gwap.getRegister(REGI_SENSOR)->getData();
 
  // Sleep
  gwap.goToSleep();
 }

