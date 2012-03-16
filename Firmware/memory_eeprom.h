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

#ifndef GPS_EEPROM_H
#define GPS_EEPROM_H

/**
  * Definitions of flags stored in EEPROM of the uC.
 **/

// a 25 byte buffer from 0xe0 to 0xf8 inclusive
#define SD_CIRC_BUFF_INDICES_ADDR 0xe0 

#define JTAG_DISABLE_FLAG_ADDR 0xfd
#define DISABLE_JTAG 0x69

#define FLASH_ENABLE_FLAG_ADDR 0xfe
#define ENABLE_FLASH 0xac

#define BOOT_SELECT_FLAG_ADDR 0xff
#define BOOT_INTO_USER_MODE 0xc5
#define BOOT_INTO_REFLASH_MODE 0x3a

/**
  * Access methods
 **/
unsigned char eeprom_read  (unsigned char address);
void          eeprom_write (unsigned char address, unsigned char data);

#endif
