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

#define SERIAL_DEBUG  1
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

// Sampling interval in msec
uint16_t samplingInterval = 1000;
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
 * Read water flow meter
 */
void readFlow(void)
{   
  // Calculate flow rate l/min
  float flowRate = pulseCount / calibrationFactor;
  // Calculate millilitres consummed in samplingInterval msec
  uint32_t partial = flowRate * samplingInterval;
  partial /= 60;
  totalMilliLitres += partial;

  #ifdef SERIAL_DEBUG
  Serial.print("Pulses: ");
  Serial.print(pulseCount);
  Serial.print(" litres/min: ");
  Serial.print(flowRate);
  Serial.print(" millilitres: ");
  Serial.println(totalMilliLitres);
  #endif

  pulseCount = 0;
}


/**
 * readPressure
 * 
 * Read water pressure sensor
 */
void readPressure(void)
{ 
  // Read water pressure pin
  analogReference(INTERNAL2V5);
  uint16_t adc = analogRead(WATER_PRESS_PIN);
  float voltage = (adc * 2.5 / 0xFFF) * 2;
  // P = 0.3 * V - 0.15 MPa/cm2
  waterPressure = voltage * 300 - 150; // In KPa/cm2

  #ifdef SERIAL_DEBUG
  Serial.print("ADC: ");
  Serial.print(adc);
  Serial.print(" Voltage: ");
  Serial.print(voltage);
  Serial.print(" Pressure: ");
  Serial.println(waterPressure);
  #endif
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
digitalWrite(EXT_PSU_EN_PIN, HIGH);
gwap.txInterval = 60;
  
  #ifdef SERIAL_DEBUG
  Serial.begin(38400);
  Serial.println("Starting...");
  #endif

  // Flow meter
  pinMode(FLOWMETER_PIN, INPUT_PULLUP);
  
  // Enter Rx ON state
  gwap.enterSystemState(SYSTATE_RXON);

  // Transmit periodic Tx interval
  gwap.getRegister(REGI_TXINTERVAL)->getData();
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
  // Read flow meter
  for(uint8_t i=0 ; i<60 ; i++)
  {
    readFlow();
    delay(samplingInterval);
  }

  // Read water pressure sensor
  readPressure();

  // Transmit values
  gwap.getRegister(REGI_SENSOR)->getData();

  // Reset water consumption value
  totalMilliLitres = 0;
}
