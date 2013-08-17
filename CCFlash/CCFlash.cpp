/*
  MspFlash.h - Read/Write flash memory library for MSP430 Energia 
  Copyright (c) 2012 Peter Brier.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "CCFlash.h"
#include <msp430.h>
#include <Energia.h>

// erase flash, make sure pointer is in the segment you wish to erase, otherwise you may erase you program or some data
void CCFlashClass::erase(unsigned char *flash)
{
  disableWatchDog();        // Disable WDT
  FCTL3 = FWKEY;            // Clear LOCK
  FCTL1 = FWKEY+ERASE;      //Enable segment erase
  *flash = 0;               // Dummy write, erase Segment
  FCTL3 = FWKEY+LOCK;       // Done, set LOCK
  enableWatchDog();         // Enable WDT
}

// load from memory, at segment boundary
void CCFlashClass::read(unsigned char *flash, unsigned char *dest, int len)
{
  while(len--)
   *(dest++) = *(flash++); 
}

// save in to flash (at segment boundary)
void CCFlashClass::write(unsigned char *flash, unsigned char *src, int len)
{
 disableWatchDog();        // Disable WDT
 FCTL3 = FWKEY;            // Clear LOCK
 FCTL1 = FWKEY+WRT;        // Enable write
 while(len--)      // Copy data
   *(flash++) = *(src++);
 FCTL1 = FWKEY;            //Done. Clear WRT
 FCTL3 = FWKEY+LOCK;       // Set LOCK
 enableWatchDog();         // Enable WDT
}

CCFlashClass Flash;
