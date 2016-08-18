/*
 * binouts
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
 * Creation date: 04/05/2016
 *
 * Device:
 * Binary output + PWM module
 *
 * Description:
 * Device that provides 8 controllable binary (ON/OFF) outputs and 4 PWM
 * outputs
 *
 * Binary outputs : pins 3, 4, 5, 6, 7, 8, 9 and 10
 * PWM outputs: pins 3, 18, 20 and 21 
 *
 * This sketch can be used to control loads (ON/OFF) and even run some type
 * of progressive control via PWM (dim lights, control motors, blinds, etc)
 *
 * Associated Device Definition File, defining registers, endpoints and
 * configuration parameters:
 * 00010007.json (Binary/PWM output module)
 */

#include "gwap.h"
#include "regtable.h"

// Binary output pins (Arduino digital pins)
uint8_t binaryPin[] = {15, 14, 13, 12, 11, 10, 9, 8};
// PWM output pins (Arduino digital pins)
uint8_t pwmPin[] = {18, 19, 1, 2};

// Address of three binary sensors
const uint8_t addrSensor[3][12] = {{0x0C,0x00,0x25,0x00,0x32,0x07,0x32,0x47,0x00,0x01,0x00,0x32},  // First binary sensor
                                   {0x1E,0x00,0x15,0x00,0x32,0x07,0x32,0x47,0x00,0x01,0x00,0x32},  // Second binary sensor
                                   {0x17,0x00,0x24,0x00,0x32,0x07,0x32,0x47,0x00,0x01,0x00,0x32}}; // Third binary sensor
uint8_t currentStates = 0;

/**
 * gwapReceived
 *
 * Function automatically called by the panStamp API whenever a GWAP packet is received
 * packet is received
 *
 * 'pkt'    GWAP packet received
 */
void gwapReceived(GWPACKET *pkt)
{
  uint8_t i;

  /**
   * This function drives six relays depending on the binary state reported by
   * our three sensors
   */
 
  for(i=0 ; i<3 ; i++)
  {
    // Check address
    if (!memcmp(pkt->address, addrSensor[i], sizeof(addrSensor[i])))
    {
      // GWAP status packet?
      if (pkt->function == GWAPFUNCT_STA)
      {
        // Sensor register?
        if (pkt->regId == 7)
        {
          // Binary state received
          if (pkt->value.data[2])
          {
            currentStates |= (1 << i*2);
            currentStates &= ~(2 << i*2);
          }
          else
          {
            currentStates &= ~(1 << i*2);
            currentStates |= (2 << i*2);
          }
          // Drive relays and send packet
          gwap.getRegister(REGI_BINOUTPUTS)->setData(&currentStates);
          break;
        }
      }
    }
  }
}

/**
 * setup
 *
 * Arduino setup function
 */
void setup()
{
  uint8_t i;

  digitalWrite(LED, LOW);

  // Disable address check from the RF IC
  panstamp.radio.disableAddressCheck();
  // Enter high Tx power mode
  panstamp.setHighTxPower();
  
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
  // GWAP callback function
  gwap.attachInterrupt(STATUS, gwapReceived);
  
  // Enter Rx ON state
  gwap.enterSystemState(SYSTATE_RXON);
  delay(GWAP_TX_SILENCE);
  // Transmit initial binary states
  gwap.getRegister(REGI_BINOUTPUTS)->getData();
  delay(GWAP_TX_SILENCE);
  // Transmit initial PWM values
  gwap.getRegister(REGI_PWMOUTPUTS)->getData();
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
  delay(100);
  digitalWrite(LED, LOW);
  delay(4900);
}

