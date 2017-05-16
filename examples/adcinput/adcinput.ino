/**
 * Copyright (c) 2017 panStamp S.L.U. <contact@panstamp.com>
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
 * Author: Francisco Barrena
 * Creation date: 05/16/2017
 *
 * Device:
 * Singoe input analog sensor
 *
 * Description:
 * This is a basic device measuring ADC values from pin A0. An output pin
 * (9) is provided to power the external circuit (Ex: potentiometer)
 *
 * Thos device is low-power enabled so it will enter low-power mode
 * just after reading the sensor value and transmitting it over the
 * GWAP network.
 *
 * Associated Device Definition Files, defining registers, endpoints and
 * configuration parameters:
 * 00010015.json (Single input analog sensor)
 */

#include "gwap.h"
#include "regtable.h"

#define ADC_PIN A0
#define POWER_PIN 9

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
  
  pinMode(POWER_PIN, OUTPUT);    // Configure power pin. This pin will be used to power the external circuit
  
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
  digitalWrite(LED, HIGH);
  // Transmit temperature value
  gwap.getRegister(REGI_SENSOR)->getData();
  digitalWrite(LED, LOW);

  // Sleep
  gwap.goToSleep();
}

