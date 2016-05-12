/*
 * levelsensor
 *
 * Copyright (c) 2016 panStamp <contact@panstamp.com>
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
 * Creation date: 05/11/2016
 *
 * Device:
 * Ultrasonic level + temperature + accelerometer sensor device
 *
 * Description:
 * This device periodically measures fill level or distance to an obstacle.
 * It also measures ambient temperature and can be awaken from an
 * accelerometer event
 *
 * This device is low-power enabled so it will enter low-power mode just
 * after reading the binary states and transmitting them over the GWAP
 * network.
 *
 * Associated Device Definition File, defining registers, endpoints and
 * configuration parameters:
 * levelsensor. (Binary/Counter input module)
 */
 
#include "gwap.h"
#include "regtable.h"
#include "thermistor.h"

/**
 * Pin definitions
 */
#define INPUT_PIN  8
#define ENABLE_SENSOR 20
#define ENABLE_SENSOR_POWER 16
#define SENSOR_TRIGGER 14
#define SENSOR_ECHO 15
#define NTC_PIN A5

// Macros
#define powerSensorOn()   digitalWrite(ENABLE_SENSOR_POWER, HIGH); digitalWrite(ENABLE_SENSOR, HIGH)
#define powerSensorOff()  digitalWrite(ENABLE_SENSOR_POWER, LOW); digitalWrite(ENABLE_SENSOR, LOW)

/**
 * Global variables
 */
uint16_t level;       // Level returned by the ultrasonic sensor
uint16_t temperature; // Temperature returned by NTC

// Thermistor object
THERMISTOR thermistor(NTC_PIN,        // Analog pin
                      10000,          // Nominal resistance at 25 ºC
                      3950,           // thermistor's beta coefficient
                      10000);         // Value of the series resistor
                      
/**
 * setup
 *
 * Arduino setup function
 */
void setup()
{
  int i;

  digitalWrite(LED, LOW);

  // Configure ultrasonic sensor pins
  pinMode(ENABLE_SENSOR_POWER, OUTPUT);
  digitalWrite(ENABLE_SENSOR_POWER, LOW);
  pinMode(ENABLE_SENSOR, OUTPUT);  
  digitalWrite(ENABLE_SENSOR, LOW);
  pinMode(SENSOR_TRIGGER, OUTPUT);  
  digitalWrite(SENSOR_TRIGGER, LOW);
  pinMode(SENSOR_ECHO, INPUT);

  // Enter high Tx power mode
  panstamp.setHighTxPower();
  // Long distance board?
  //panstamp.radio.enableHGM();
  
  // Configure input pin
  pinMode(INPUT_PIN, INPUT);

  // Init GWAP stack
  gwap.init();
  
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
}

/**
 * loop
 *
 * Arduino main loop
 */
void loop()
{
  // Read sensor data
  powerSensorOn();        // Power thermistor
  delay(100);

  // Read temperature
  temperature = thermistor.read();   // Read temperature

  // Read level
  level = readLevel();
  
  powerSensorOff();       // Unpower thermistor  

  gwap.getRegister(REGI_SENSOR)->getData();

  gwap.goToSleep();
}

/**
 * readLevel
 * 
 * Read level from ultrasonic transducer
 * 
 * @return level detected in cm
 */
uint16_t readLevel(void)
{
  uint16_t res;
  
  // Read level
  digitalWrite(SENSOR_TRIGGER, HIGH);
  delayMicroseconds(50);
  digitalWrite(SENSOR_TRIGGER, LOW);
  // Compute distance
  res = pulseIn(SENSOR_ECHO, HIGH);
  res = res/58;    // Distance in cm

  return res;
}

