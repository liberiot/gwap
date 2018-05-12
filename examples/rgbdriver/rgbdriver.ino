/**
 * Copyright (c) 2016 Daniel Berenguer <contact@panstamp.com>
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
 * RGB LED driver
 *
 * Description:
 * This device controls a RGB LED or a RGB LED strip using three PWM
 * outputs. RGB levels can be controlled via the RGBlevel register
 * (see Device Definition File for details). 
 *
 * Associated Device Definition File: 00010003.json
 */

#include "gwap.h"
#include "regtable.h"
#include "rgbled.h"

#define CHRONOS_PRODUCT_CODE  {0,0,0,1,0,0,0,2}
#define CHRONOS_REGID_ACC     14

// Global RGBLED object. Define your own PWM outputs here (Arduino digital pins):
RGBLED rgbLed = RGBLED(19, 3, 4);

/**
 * setup
 *
 * One-time setup function
 */
void setup() 	 
{
  digitalWrite(LED, LOW);
 
  // Init SWAP stack
  gwap.init();

  // Enter Rx ON state
  gwap.enterSystemState(SYSTATE_RXON);
  delay(GWAP_TX_SILENCE);
  // Transmit initial RGB value
  gwap.getRegister(REGI_RGBLEVELS)->getData();
  delay(GWAP_TX_SILENCE);
}

/**
 * loop
 *
 * Main loop
 */
void loop()
{
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(4900);
}

