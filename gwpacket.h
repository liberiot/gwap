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
 * Creation date: 03/11/2016
 */

#ifndef _GWPACKET_H
#define _GWPACKET_H

#include "ccpacket.h"
#include "datatypes.h"

/**
 * GWAP definitions
 */
#define GWAP_ADDRESS_LENGTH    12
#define GWAP_DATA_HEAD_LEN     GWAP_ADDRESS_LENGTH + 2
#define GWAP_REG_VAL_LEN       CC1101_DATA_LEN - GWAP_DATA_HEAD_LEN     // SWAP data payload - max length
#define GWAP_NB_TX_TRIES       3                                        // Number of transmission retries
#define GWAP_TX_DELAY          10                                       // Delay before sending (msec)
#define GWAP_POS_NONCE         GWAP_ADDRESS_LENGTH                      // Position of nonce in GWAP packet
#define GWAP_POS_FUNCTION      GWAP_POS_NONCE + 1                       // Position of function code in GWAP packet
#define GWAP_POS_REGID         GWAP_POS_FUNCTION + 1                    // Position of register ID in GWAP packet

/**
 * GWAP message functions
 */
enum GWAPFUNCT
{
  GWAPFUNCT_STA = 0x00,
  GWAPFUNCT_QRY,
  GWAPFUNCT_CMD,
};

/**
 * GWDATA : GWAP data structure
 */
struct GWDATA
{
    /**
     * Data buffer
     */
    uint8_t *data;

    /**
     * Data length
     */
    uint8_t length;

    /**
     * Data type
     */
    GWDTYPE type;
};

class GWPACKET
{
  private:
    /**
     * Raw packet
     */
     CCPACKET ccPacket;
     
  public:
    /**
     * Source address
     */
    uint8_t address[GWAP_ADDRESS_LENGTH];

    /**
     *  Security nonce
     */
    uint8_t nonce;

    /**
     * Function byte
     */
    uint8_t function;

    /**
     * Register id
     */
    uint8_t regId;

    /**
     * Register value
     */
    GWDATA value;

    /**
     * Packet CRC
     */
    uint8_t crc;

    /**
     * GWPACKET
     * 
     * Class constructor
     * 
     * @param packet Pointer to the raw CC1101 packet
     */
    GWPACKET(CCPACKET *packet);

    /**
     * GWPACKET
     * 
     * Class constructor
     */
    GWPACKET(void);
    
    /**
     * send
     * 
     * Send GWAP packet
     *
     * @return:
     *  True if the transmission succeeds
     *  False otherwise
     */
    bool send(void);

    /**
     * checkCrc
     * 
     * Check CRC field
     *
     * @return
     *  True if the CRC succeeds
     *  False otherwise
     */
    bool checkCrc(void);
};

#endif
