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

#ifndef _COMMONREGS_H
#define _COMMONREGS_H

#include "gwstatus.h"
#include "nvolat.h"
#include "register.h"
#include "register.h"
#include "gwap.h"

const void setSysState(uint8_t id, uint8_t *state);
const void setFreqChannel(uint8_t id, uint8_t *channel);
const void setNetworkId(uint8_t id, uint8_t *nId);
const void setTxInterval(uint8_t id, uint8_t *interval);

#define DECLARE_COMMON_CALLBACKS()

/**
 * Macros for the definition of common register indexes
 */
#define DEFINE_COMMON_REGINDEX_START()  \
enum CUSTOM_REGINDEX                    \
{                                       \
  REGI_PRODUCTCODE = 0,                 \
  REGI_HWVERSION,                       \
  REGI_FWVERSION,                       \
  REGI_SYSSTATE,                        \
  REGI_FREQCHANNEL,                     \
  REGI_NETWORKID,                       \
  REGI_TXINTERVAL,

#define DEFINE_COMMON_REGINDEX_END()    };

#define DEFINE_REGINDEX_START()   DEFINE_COMMON_REGINDEX_START()
#define DEFINE_REGINDEX_END()     DEFINE_COMMON_REGINDEX_END()

/**
 * Macro for the definition of registers common to all GWAP devices
 */
#define DEFINE_COMMON_REGISTERS()                                                                                            \
/* Product code */                                                                                                           \
static uint8_t dtProductCode[4] = {(uint8_t)(GWAP_PRODUCT_CODE >> 24), (uint8_t)(GWAP_PRODUCT_CODE >> 16) , (uint8_t)(GWAP_PRODUCT_CODE >> 8), (uint8_t)(GWAP_PRODUCT_CODE)};  \
REGISTER regProductCode(dtProductCode, sizeof(dtProductCode), NULL, NULL);                                                   \
/* Hardware version */                                                                                                       \
static uint8_t dtHwVersion[4] = {(uint8_t)(HARDWARE_VERSION >> 24), (uint8_t)(HARDWARE_VERSION >> 16) , (uint8_t)(HARDWARE_VERSION >> 8), (uint8_t)(HARDWARE_VERSION)};     \
REGISTER regHwVersion(dtHwVersion, sizeof(dtHwVersion), NULL, NULL);                                                         \
/* Firmware version */                                                                                                       \
static uint8_t dtFwVersion[4] = {(uint8_t)(FIRMWARE_VERSION >> 24), (uint8_t)(FIRMWARE_VERSION >> 16) , (uint8_t)(FIRMWARE_VERSION >> 8), (uint8_t)(FIRMWARE_VERSION)};     \
REGISTER regFwVersion(dtFwVersion, sizeof(dtFwVersion), NULL, NULL);                                                         \
/* System state */                                                                                                           \
REGISTER regSysState(&gwap.systemState, sizeof(gwap.systemState), NULL, &setSysState);                                       \
/* Frequency channel */                                                                                                      \
REGISTER regFreqChannel(&panstamp.radio.channel, sizeof(panstamp.radio.channel), NULL, &setFreqChannel, GWDTYPE_INTEGER, NVOLAT_FREQ_CHANNEL);  \
/* Network Id */                                                                                                             \
REGISTER regNetworkId(panstamp.radio.syncWord, sizeof(panstamp.radio.syncWord), NULL, &setNetworkId, GWDTYPE_OTHER, NVOLAT_SYNC_WORD);  \
/* Periodic Tx interval */                                                                                                   \
REGISTER regTxInterval((uint8_t*)&gwap.txInterval, sizeof(gwap.txInterval), NULL, &setTxInterval, GWDTYPE_INTEGER, NVOLAT_TX_INTERVAL);

/**
 * Macros for the declaration of global table of registers
 */
#define DECLARE_REGISTERS_START()    \
REGISTER *regTable[] = {             \
        &regProductCode,             \
        &regHwVersion,               \
        &regFwVersion,               \
        &regSysState,                \
        &regFreqChannel,             \
        &regNetworkId,               \
        &regTxInterval,

#define DECLARE_REGISTERS_END()      \
};                                   \
/* Size of regTable */               \
uint8_t regTableSize = sizeof(regTable)/sizeof(*regTable);

/**
 * Macro for the definition of getter/setter functions related to all common registers
 */
#define DEFINE_COMMON_CALLBACKS()                           \
/**                                                         \
 * setSysState                                              \
 *                                                          \
 * Set system state                                         \
 *                                                          \
 * @param id Register ID                                    \
 * @param state New system state                            \
 */                                                         \
const void setSysState(uint8_t id, uint8_t *state)          \
{                                                           \
  gwap.systemState = state[0];                              \
  switch(state[0])                                          \
  {                                                         \
    case SYSTATE_RESTART:                                   \
      /* Send status message before restarting the mote */  \
      gwap.getRegister(REGI_SYSSTATE)->sendGwapStatus();    \
      panstamp.reset();                                     \
      break;                                                \
    case SYSTATE_UPGRADE:                                   \
      panstamp.goToWirelessBoot();                          \
      break;                                                \
    default:                                                \
      break;                                                \
  }                                                         \
}                                                           \
                                                            \
/**                                                         \
 * setFreqChannel                                           \
 *                                                          \
 * Set frequency channel                                    \
 *                                                          \
 * @param id Register ID                                    \
 * @param channel New channel                               \
 */                                                         \
const void setFreqChannel(uint8_t id, uint8_t *channel)     \
{                                                           \
  if (channel[0] != regFreqChannel.value[0])                \
  {                                                         \
    /* Send status message before entering the new          \
    frequency channel */                                    \
    GWSTATUS packet = GWSTATUS(regFreqChannel.id, channel, regFreqChannel.length); \
    packet.send();                                          \
    /* Update register value */                             \
    panstamp.radio.setChannel(channel[0]);                  \
    /* Restart device */                                    \
    panstamp.reset();                                       \
  }                                                         \
}                                                           \
                                                            \
/**                                                         \
 * setNetworkId                                             \
 *                                                          \
 * Set network id                                           \
 *                                                          \
 * @param id Register ID                                    \
 * @param nId New network id                                \
 */                                                         \
const void setNetworkId(uint8_t id, uint8_t *nId)           \
{                                                           \
  if ((nId[0] != regNetworkId.value[0]) ||                  \
      (nId[1] != regNetworkId.value[1]))                    \
  {                                                         \
    /* Send status before taking the new network ID */      \
    GWSTATUS packet = GWSTATUS(regNetworkId.id, nId, regNetworkId.length); \
    packet.send();                                          \
    /* Update register value */                             \
    panstamp.radio.setSyncWord(nId);                        \
  }                                                         \
}                                                           \
/**                                                         \
 * setTxInterval                                            \
 *                                                          \
 * Set periodic Tx interval                                 \
 *                                                          \
 * @param id Register ID                                    \
 * @param interval New interval (in seconds)                \
 */                                                         \
const void setTxInterval(uint8_t id, uint8_t *interval)     \
{                                                           \
  /* Set new Tx interval. BE to LE conversion */            \
  regTxInterval.setValueFromBeBuffer(interval);             \
}
#endif

