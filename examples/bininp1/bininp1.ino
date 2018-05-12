/*
 * bininp1
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
 * Creation date: 04/06/2016
 *
 * Device:
 * Binary input
 *
 * Description:
 * Device that reports the binary state of a single digital input.
 *
 * This sketch can be used to detect key/switch presses, binary alarms or
 * any other binary sensor.
 *
 * This device is low-power enabled so it will enter low-power mode just
 * after reading the binary states and transmitting them over the GWAP
 * network.
 *
 * Associated Device Definition File, defining registers, endpoints and
 * configuration parameters:
 * 00010032.json (Single binary input module)
 */
 
#include "gwap.h"
#include "regtable.h"

/**
 * Input pin
 */
#define INPUT_PIN  8
 
/**
 * Pin event flag
 */
bool pinEvent = false;

/**
 * pinChange
 * 
 * Pin change interrupt
 */
void pinChange(void)
{
  pinEvent = true;
}

/**
 * setup
 *
 * Arduino setup function
 */
void setup()
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  // Long distance board?
  panstamp.radio.enableHGM();
  
  // Configure input pin
  pinMode(INPUT_PIN, INPUT);

  // Init GWAP stack
  gwap.init();
  
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
  // Transmit initial binary state
  gwap.getRegister(REGI_BININPUT)->getData();
  delay(GWAP_TX_SILENCE);  
  // Switch to Rx OFF state
  gwap.enterSystemState(SYSTATE_RXOFF);
  delay(GWAP_TX_SILENCE);
  
  // Attach custom function to Pin Change Interrupt
  attachInterrupt(INPUT_PIN, pinChange, CHANGE);
}

/**
 * loop
 *
 * Arduino main loop
 */
void loop()
{
  if (pinEvent)
  {
    pinEvent = false;
    gwap.getRegister(REGI_BININPUT)->getData();
    delay(GWAP_TX_SILENCE);
    gwap.getRegister(REGI_BININPUT)->getData();
    delay(GWAP_TX_SILENCE);
    gwap.getRegister(REGI_BININPUT)->getData();
  }
  else
    gwap.goToSleep();
}

