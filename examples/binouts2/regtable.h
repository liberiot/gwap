/**
 * regtable.h
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
 * Creation date: 05/24/2016
 */

#ifndef _REGTABLE_H
#define _REGTABLE_H

#include "register.h"
#include "commonregs.h"

/**
 * Register indexes
 */
DEFINE_REGINDEX_START()
  REGI_BINOUTPUT0,
  REGI_BINOUTPUT1,
  REGI_BINOUTPUT2,
  REGI_BINOUTPUT3,
  REGI_BINOUTPUT4,
  REGI_BINOUTPUT5,
  REGI_BINOUTPUT6,
  REGI_BINOUTPUT7,
  REGI_PWMOUTPUT0,
  REGI_PWMOUTPUT1,
  REGI_PWMOUTPUT2,
  REGI_PWMOUTPUT3
DEFINE_REGINDEX_END()

#endif

