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

#include "gwpacket.h"
#include "gwap.h"
#include "panstamp.h"

/**
 * GWPACKET
 * 
 * Class constructor
 * 
 * @param packet Pointer to the raw CC1101 packet
 */
GWPACKET::GWPACKET(CCPACKET *packet) 
{
  uint8_t i;
   
  // Save raw data and length
  ccPacket.length = packet->length;
  for(i=0 ; i<ccPacket.length ; i++)
    ccPacket.data[i] = packet->data[i];
  
  crc = ccPacket.data[ccPacket.length-1];

  for(i=0 ; i<GWAP_ADDRESS_LENGTH ; i++)
    address[i] = ccPacket.data[i];

  nonce = ccPacket.data[GWAP_POS_NONCE];
  function = ccPacket.data[GWAP_POS_FUNCTION];
  regId = ccPacket.data[GWAP_POS_REGID];

  value.data = ccPacket.data + GWAP_DATA_HEAD_LEN + 1;
  value.length = ccPacket.length - GWAP_DATA_HEAD_LEN - 2;
}

/**
 * GWPACKET
 * 
 * Class constructor
 */
GWPACKET::GWPACKET(void) 
{
}

/**
 * send
 * 
 * Send GWAP packet. Do up to 5 retries if necessary
 *
 * @return
 *  True if the transmission succeeds
 *  False otherwise
 */
bool GWPACKET::send(void)
{
  byte i;
  boolean res;

  // LE -> BE conversion for numeric values
  if (value.type == GWDTYPE_INTEGER)
  {
    for(i=0 ; i<value.length ; i++)
      ccPacket.data[i+GWAP_DATA_HEAD_LEN + 1] = value.data[value.length-1-i];
  }
  else
  {
    for(i=0 ; i<value.length ; i++)
      ccPacket.data[i+GWAP_DATA_HEAD_LEN + 1] = value.data[i];
  }

  ccPacket.length = value.length + GWAP_DATA_HEAD_LEN + 2;

  for(i=0 ; i<GWAP_ADDRESS_LENGTH ; i++)
    ccPacket.data[i] = address[i];

  ccPacket.data[GWAP_POS_NONCE] = gwap.nonce++;
  ccPacket.data[GWAP_POS_FUNCTION] = function;
  ccPacket.data[GWAP_POS_REGID] = regId;

  // Calculate CRC
  crc = 0;
  for(i=0 ; i<ccPacket.length-1 ; i++)
    crc += ccPacket.data[i];
  ccPacket.data[i] = crc; // Place CRC byte at the end of the frame

  i = GWAP_NB_TX_TRIES;
  while(!(res = panstamp.sendData(ccPacket)) && i>1)
  {
    i--;
    delay(GWAP_TX_DELAY);
  }

  return res;
}

/**
 * checkCrc
 * 
 * Check CRC field
 *
 * @return
 *  True if the CRC succeeds
 *  False otherwise
 */
bool GWPACKET::checkCrc(void)
{
  uint8_t i, tmpCrc = 0;

  for(i=0 ; i<ccPacket.length-1 ; i++)
    tmpCrc += ccPacket.data[i];

  return (tmpCrc == crc);
}

