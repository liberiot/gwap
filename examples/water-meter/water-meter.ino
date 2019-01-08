/**
 * Copyright (c) 2019 panStamp S.L.U. <contact@panstamp.com>
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
 * Creation date: Jan 8 2019
 *
 * Device:
 * Water meter (consumption + pressure)
 *
 * Description:
 * This application gets readings from a flow meter and a water pressure sensor
 * to calculate water consumption and pressure.
 *
 * This device is low-power enabled so it will enter low-power mode
 * just after reading the sensor value and transmitting it over the
 * GWAP network.
 *
 * Associated Device Definition Files, defining registers, endpoints and
 * configuration parameters:
 * 00010040.json (Water meter (consumption + pressure))
 */

#include "gwap.h"
#include "regtable.h"

//#define SERIAL_DEBUG  1
#ifdef SERIAL_DEBUG
#include <HardwareSerial.h>
#endif

// On-board MCP1640B PSU
const uint8_t EXT_PSU_EN_PIN = 22;

// Flow meter
const uint8_t FLOWMETER_PIN = 7;
volatile uint16_t pulseCount = 0;
const float calibrationFactor = 6.6; // 6.6 pulses/sec per litre/min
uint32_t totalMilliLitres = 0;

// Water pressure sensor
const uint8_t WATER_PRESS_PIN = A2;
uint16_t waterPressure = 0;

// Sampling interval in seconds
const uint8_t samplingInterval = 15;

// Enable wireless transmission
bool transmit = false;

/**
 * pulseCounter
 * 
 * Count pulse comming from flow meter
 */
void pulseCounter(void)
{
  pulseCount++;
}

/**
 * readFlow
 * 
 * Read water flow and pressure sensors
 */
void readFlow(void)
{ 
  // Power water pressure sensor
  digitalWrite(EXT_PSU_EN_PIN, HIGH);
  delay(100);

  // Read water pressure pin
  analogReference(INTERNAL2V5);
  uint16_t adc = analogRead(WATER_PRESS_PIN);
  float voltage = (adc * 2.5 / 0xFFF) * 2;
  // P = 0.3 * V - 0.15 MPa/cm2
  waterPressure = voltage * 300 - 150; // In KPa/cm2
  
  // Unpower water pressure sensor  
  digitalWrite(EXT_PSU_EN_PIN, LOW);
  
  // Calculate flow rate l/min
  float flowRate = pulseCount / calibrationFactor;
  // Calculate millilitres consummed in txInterval
  totalMilliLitres = flowRate * gwap.txInterval;
  totalMilliLitres /= 60;

  #ifdef SERIAL_DEBUG
  Serial.print("Pulses: ");
  Serial.print(pulseCount);
  Serial.print(" litres/min: ");
  Serial.print(flowRate);
  Serial.print(" millilitres: ");
  Serial.println(totalMilliLitres);
  
  Serial.print("ADC: ");
  Serial.print(adc);
  Serial.print(" Voltage: ");
  Serial.print(voltage);
  Serial.print(" Pressure: ");
  Serial.println(waterPressure);
  #endif

  pulseCount = 0;
}

/**
 * setup
 *
 * Arduino setup function
 */
void setup()
{
  int i;
 
  // Init GWAP stack
  gwap.init();

  // Initialize LED pins
  pinMode(LED, OUTPUT);

  // Initialize external PSU (MCP1640B) enable pin
  pinMode(EXT_PSU_EN_PIN, OUTPUT);
  digitalWrite(EXT_PSU_EN_PIN, LOW);
  
  #ifdef SERIAL_DEBUG
  Serial.begin(38400);
  Serial.println("Starting...");
  #endif

  // Flow meter
  pinMode(FLOWMETER_PIN, INPUT_PULLUP);
  
  // Enter SYNC state
  gwap.enterSystemState(SYSTATE_SYNC);

  // During 3 seconds, listen the network for possible commands whilst the LED blinks
  for(i=0 ; i<6 ; i++)
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

  // Keep counting pulses from flow sensor
  attachInterrupt(FLOWMETER_PIN, pulseCounter, RISING);
}

/**
 * loop
 *
 * Arduino main loop
 */
void loop()
{
  // Sleep
  panstamp.sleepSec(samplingInterval);

  // Calculate and transmit values
  gwap.getRegister(REGI_SENSOR)->getData();
}

