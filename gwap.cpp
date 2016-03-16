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

#include "panstamp.h"
#include "gwap.h"
#include "gwpacket.h"
#include "commonregs.h"


DEFINE_COMMON_REGINDEX_START()
DEFINE_COMMON_REGINDEX_END()

/**
 * packetReceived
 *
 * CCPACKET received
 *
 * @param packet CCPACKET received
 */
void packetReceived(CCPACKET *packet)
{
  if (packet->length <= GWAP_DATA_HEAD_LEN)
    return;
  
  GWPACKET gwPacket = GWPACKET(packet);
  REGISTER *reg;

  // Function
  switch(gwPacket.function)
  {
    case GWAPFUNCT_CMD:
      // Command not addressed to us?
      if (memcmp(gwPacket.address, gwap.address, sizeof(gwap.address)))
        break;
      // Valid register?
      if ((reg = gwap.getRegister(gwPacket.regId)) == NULL)
        break;
      // Filter incorrect data lengths
      if (gwPacket.value.length == reg->length)
        reg->setData(gwPacket.value.data);
      else
        reg->sendGwapStatus();
      break;
    case GWAPFUNCT_QRY:
      // Query not addressed to us?
      if (memcmp(gwPacket.address, gwap.address, sizeof(gwap.address)))
        break;
      // Valid register?
      if ((reg = gwap.getRegister(gwPacket.regId)) == NULL)
        break;
      reg->getData();
      break;
    case GWAPFUNCT_STA:
      // User callback function declared?
      if (gwap.statusReceived != NULL)
        gwap.statusReceived(&gwPacket);
      break;
    default:
      break;
  }
}

/**
 * GWAP
 *
 * Class constructor
 */
GWAP::GWAP(void)
{
  nonce = 0;
  statusReceived = NULL;
}

/**
 * init
 *
 * Initialize GWAP registers and stack
 */
void GWAP::init(void)
{
  uint8_t i;
  STORAGE nvMem;

  // Read signature from info/eeprom memory
  uint8_t signature[2];
  nvMem.read(signature, DEFAULT_NVOLAT_SECTION, NVOLAT_SIGNATURE, sizeof(signature));

  // Correct signature in non-volatile memory?
  if ((signature[0] != NVOLAT_SIGNATURE_HIGH) || (signature[1] != NVOLAT_SIGNATURE_LOW))
    nvolatToFactoryDefaults(); // Copy default settings in non-volatile memory  

  // Intialize registers
  for(i=0 ; i<regTableSize ; i++)
    regTable[i]->init();

  // Build UID
  panstamp.core.getUID(address);
  for(i=8 ; i<GWAP_ADDRESS_LENGTH ; i++)
    address[i] = regTable[REGI_PRODUCTCODE]->value[i];

  // Config radio settings
  panstamp.radio.devAddress = address[0]; 
  panstamp.radio.setCCregs();
  
  // Attach RF ISR
  panstamp.attachInterrupt(packetReceived);
}

/**
 * goToSleep
 *
 * put the MCU in sleep mode during txInterval seconds
 */
void GWAP::goToSleep(void)
{
  systemState = SYSTATE_RXOFF;
  panstamp.sleepSec(txInterval);
  systemState = SYSTATE_RXON;
}

/**
 * nvolatToFactoryDefaults
 * 
 * Write default config values in non-volatile memory
 */
void GWAP::nvolatToFactoryDefaults(void)
{
  STORAGE nvMem;

  // Signature
  uint8_t signature[] = {NVOLAT_SIGNATURE_HIGH, NVOLAT_SIGNATURE_LOW};
  nvMem.write(signature, DEFAULT_NVOLAT_SECTION, NVOLAT_SIGNATURE, sizeof(signature));
  
  // Frequency channel
  uint8_t channel[] = {CCDEF_CHANNR};
  nvMem.write(channel, DEFAULT_NVOLAT_SECTION, NVOLAT_FREQ_CHANNEL, sizeof(channel));
  
  // Sync word
  uint8_t syncW[] = {CCDEF_SYNC1, CCDEF_SYNC0};
  nvMem.write(syncW, DEFAULT_NVOLAT_SECTION, NVOLAT_SYNC_WORD, sizeof(syncW));
 
  // TX interval
  uint8_t txInt[] = {0xFF, 0};
  nvMem.write(txInt, DEFAULT_NVOLAT_SECTION, NVOLAT_TX_INTERVAL, sizeof(txInt));
}

/**
 * Pre-instantiate GWAP object
 */
GWAP gwap;

