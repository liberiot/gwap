/*
 * bininp8
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
 * Creation date: 05/16/2016
 *
 * Device:
 * Binary input
 *
 * Description:
 * Device detecting state changes from 8 different binary inputs and with
 * 2 binary outputs to drive external relays.
 *
 * This sketch can be used to detect key/switch presses, binary alarms or
 * any other binary sensor. Binary outputs can be used to trigger alarms
 * and control relays.
 *
 * Associated Device Definition File, defining registers, endpoints and
 * configuration parameters:
 * 00010035.json (8 binary input and 2 binary output module)
 */
 
#include "gwap.h"
#include "regtable.h"

/**
 * Input pins
 */
const uint8_t inputPin[] = {15, 14, 13, 12, 9, 7, 2, 1};
#define NBOF_INPUTS  sizeof(inputPin)/sizeof(*inputPin)

/**
 * Output pins
 */
const uint8_t outputPin[] = {0, 22};
#define NBOF_OUTPUTS  sizeof(outputPin)/sizeof(*outputPin)

/**
 * Vcc monitor pin
 */
#define VCC_MONITOR_PIN  8

/**
 * LED pins
 */
#define GREEN_LED  18
#define RED_LED    19

/**
 * Pin event flag
 */
bool pinEvent = false;

/**
 * Vcc outage alarm
 */
uint8_t alarm = 0;

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
 * outage
 * 
 * Main voltage outage
 */
void outage(void)
{
  alarm = digitalRead(VCC_MONITOR_PIN);
  gwap.getRegister(REGI_ALARM)->getData();
  digitalWrite(RED_LED, HIGH);
}

/**
 * setup
 *
 * Arduino setup function
 */
void setup()
{
  uint8_t i;

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  // Long distance board?
  //panstamp.radio.enableHGM();

  // Config input pins
  for(i=0 ; i<NBOF_INPUTS ; i++)
  {
    // Configure input pin
    pinMode(inputPin[i], INPUT);
    // Attach custom function to Pin Change Interrupt
    attachInterrupt(inputPin[i], pinChange, CHANGE);
  }

  // Config output pins
  for(i=0 ; i<NBOF_OUTPUTS ; i++)
  {
    pinMode(outputPin[i], OUTPUT);
    digitalWrite(outputPin[i], LOW);
  }

  // Config Vcc monitor pin
  pinMode(VCC_MONITOR_PIN, INPUT);
  attachInterrupt(VCC_MONITOR_PIN, outage, FALLING);

  // Config status LED's
  pinMode(RED_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(GREEN_LED, LOW);
    
  // Init GWAP stack
  gwap.init();
  
  // Enter SYNC state
  gwap.enterSystemState(SYSTATE_RXON);

  // Transmit periodic Tx interval
  gwap.getRegister(REGI_TXINTERVAL)->getData();
  delay(GWAP_TX_SILENCE);
  // Transmit initial binary states
  gwap.getRegister(REGI_BININPUT)->getData();
  delay(GWAP_TX_SILENCE);
  gwap.getRegister(REGI_BINOUTPUT0)->getData();
  delay(GWAP_TX_SILENCE);
  gwap.getRegister(REGI_BINOUTPUT1)->getData();
  delay(GWAP_TX_SILENCE);
  gwap.getRegister(REGI_ALARM)->getData();
  delay(GWAP_TX_SILENCE);
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
  }
}

