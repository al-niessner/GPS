/**********************************************************************
 * Copyright (C) 2012 Al Niessner
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110, USA
 *
 *====================================================================
 *
 * Module Description:
 *
 *********************************************************************/

#include <Compiler.h>

#include "eeprom.h"

unsigned char eeprom_read  (unsigned char address)
{
    EECON1 = 0x00;
    EEADR = address;
    EECON1bits.RD = 1;
    return EEDATA;
}

void          eeprom_write (unsigned char address, unsigned char data)
{
    EEADR = address;
    EEDATA = data;
    EECON1 = 0b00000100;    //Setup writes: EEPGD=0,WREN=1
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    while(EECON1bits.WR);       //Wait till WR bit is clear
}
