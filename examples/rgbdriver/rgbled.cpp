/**
 * rgbled.pde
 *
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
 */

#include "rgbled.h"

/**
 * Class constructor
 *
 * 'redPin'     Arduino pin connected to the red LED
 * 'greenPin'   Arduino pin connected to the green LED
 * 'bluePin'    Arduino pin connected to the blue LED
*/
RGBLED::RGBLED(int redPin, int greenPin, int bluePin) 	 
{
  pRed = redPin;
  pGreen = greenPin;
  pBlue = bluePin;
  
  pinMode(pRed, OUTPUT);
  pinMode(pGreen, OUTPUT);
  pinMode(pBlue, OUTPUT);
}

/**
 * setColor
 *
 * Set color levels on the RGB LED
 *
 * 'red'      Bright level of the red LED
 * 'green'    Bright level of the green LED
 * 'blue'     Bright level of the blue LED
 */
void RGBLED::setColor(byte red, byte green, byte blue)
{
  analogWrite(pRed, red); 	 
  analogWrite(pGreen, green);
  analogWrite(pBlue, blue);
}

