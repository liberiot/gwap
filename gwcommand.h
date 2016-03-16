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

#ifndef _GWCOMMAND_H
#define _GWCOMMAND_H

#include "gwpacket.h"


class GWCOMMAND : public GWPACKET
{
  public:
    /**
     * GWCOMMAND
     * 
     * Class constructor
     * 
     * @param destAddr Destination address
     * @param rId Register id
     * @param val New value
     * @param len Buffer length
     */
    GWCOMMAND(uint8_t *destAddr, uint8_t rId, uint8_t *val, uint8_t len);
};
#endif
