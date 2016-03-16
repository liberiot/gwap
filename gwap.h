/**
 * Copyright (c) 2016 panStamp <contact@panstamp.com>
 * 
 * This file is part of the panStamp project.
 * 
 * panStamp  is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * any later version.
 * 
 * panStamp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with panStamp; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 
 * USA
 * 
 * Author: Daniel Berenguer
 * Creation date: 11/03/2016
 */

#ifndef _GWAP_H
#define _GWAP_H

#include "register.h"
#include "gwpacket.h"

/**
 * Macros
 */
#define eepromToFactoryDefaults()   gwap.nvolatToFactoryDefaults()
#define gwapNetworkId    radio.syncWord
#define setGwapStatusCallBack(ptrFunc)     statusReceived = ptrFunc

/**
 * System states
 */
enum SYSTATE
{
  SYSTATE_RESTART = 0,
  SYSTATE_RXON,
  SYSTATE_RXOFF,
  SYSTATE_SYNC,
  SYSTATE_LOWBAT,
  SYSTATE_UPGRADE
};

/**
 * Types of GWAP message for attachInterrupt
 */
#define STATUS  0
#define QUERY   1
#define COMMAND 2

/**
 * Array of registers
 */
extern REGISTER* regTable[];
extern uint8_t regTableSize;

/**
 * Class: GWAP
 * 
 * Description:
 * GWAP protocol class
 */
class GWAP
{   
  public:      
    /**
     * GWAP address
     */
    uint8_t address[GWAP_ADDRESS_LENGTH];
   
    /**
     * Security nonce
     */
    uint8_t nonce;

    /**
     * System state
     */
    uint8_t systemState;

    /**
     * Interval between periodic transmissions. 0 for asynchronous transmissions
     */
    uint16_t txInterval;

    /**
     * GWAP status packet received. Callback function
     */
    void (*statusReceived)(GWPACKET *status);
    
    /**
     * GWAP
     *
     * Class constructor
     */
    GWAP(void);

    /**
     * init
     *
     * Initialize GWAP registers
     */
    void init(void);

    /**
     * enterSystemState
     *
     * Enter system state
     *
     * @param state New system state
     */
    void __inline__ enterSystemState(SYSTATE state)
    {
      // System state register -> id = 3
      regTable[3]->setData((uint8_t *) &state);
    }
    
    /**
     * goToSleep
     *
     * put the MCU in sleep mode during txInterval seconds
     */
    void goToSleep(void);

    /**
     * nvolatToFactoryDefaults
     * 
     * Write default config values in non-volatile memory
     */
    void nvolatToFactoryDefaults(void);

    /**
     * attachInterrupt
     * 
     * Declare custom ISR, to be called whenever a GWAP status packet is received
     * 
     * @param type of packet that triggers the user function
     * @param funct pointer to the custom function
     */
     inline void attachInterrupt(uint8_t type, void (*funct)(GWPACKET*))
     {
       if (type == STATUS)
         statusReceived = funct;
     }
     
    /**
     * getRegister
     *
     * Return pointer to register with ID = regId
     *
     * @param regId Register ID
     */
    inline REGISTER * getRegister(unsigned char regId)
    {
      return regTable[regId]; 
    }
};

/**
 * Global GWAP object
 */
extern GWAP gwap;

#endif

