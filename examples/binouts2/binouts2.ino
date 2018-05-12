/*
 * binouts2
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
 * Creation date: 05/24/2016
 *
 * Device:
 * Binary output + PWM module
 *
 * Description:
 * Device providing 8 controllable binary (ON/OFF) outputs and 4 PWM
 * outputs. Each output is controlled by an independent register so there
 * is no need to know the state of the outputs to be kept unmodified.
 *
 * Binary outputs : pins 3, 4, 5, 6, 7, 8, 9 and 10
 * PWM outputs: pins 3, 18, 20 and 21
 *
 * This sketch can be used to control loads (ON/OFF) and even run some type
 * of progressive control via PWM (dim lights, control motors, blinds, etc)
 *
 * Associated Device Definition File, defining registers, endpoints and
 * configuration parameters:
 * 0001000C.json (Binary/PWM output module)
 */
 
#include "gwap.h"
#include "regtable.h"


// Binary output pins (Arduino digital pins)
uint8_t binaryPin[] = {15, 14, 13, 12, 11, 10, 9, 8};
// PWM output pins (Arduino digital pins)
uint8_t pwmPin[] = {18, 19, 1, 2};

/**
 * setup
 *
 * Arduino setup function
 */
void setup()
{
  uint8_t i;
  
  digitalWrite(LED, LOW);
 
  // Configure output pins
  for(i=0 ; i<sizeof(binaryPin) ; i++)
  {
    pinMode(binaryPin[i], OUTPUT);
    digitalWrite(binaryPin[i], LOW);
  }
  for(i=0 ; i<sizeof(pwmPin) ; i++)
  {
    pinMode(pwmPin[i], OUTPUT);
    digitalWrite(pwmPin[i], LOW);
  }

  // Init GWAP stack
  gwap.init();
  
  // Enter Rx ON state
  gwap.enterSystemState(SYSTATE_RXON);
  delay(GWAP_TX_SILENCE);
  // Transmit initial binary states
  for(i=0 ; i<sizeof(binaryPin) ; i++)
  {
    gwap.getRegister(REGI_BINOUTPUT0 + i)->getData();
    delay(GWAP_TX_SILENCE);
  }
  // Transmit initial PWM values
  for(i=0 ; i<sizeof(pwmPin) ; i++)
  {
    gwap.getRegister(REGI_PWMOUTPUT0 + i)->getData();
    delay(GWAP_TX_SILENCE);
  }
}

/**
 * loop
 *
 * Arduino main loop
 */
void loop()
{
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(4900);
}

