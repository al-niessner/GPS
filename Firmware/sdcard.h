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
 * read/write use null terminated strings. In both cases they are limited
 * to len if the terminator is not found before then. read returns the
 * actual length of the string not including the terminator.
 *
 *********************************************************************/

#ifndef GPS_SDCARD_H
#define GPS_SDCARD_H

#include "memory.h"

#define SD_PAGE_SIZE (0x200u)

void              sdcard_erase(void);
unsigned char*    sdcard_get_CID(void);
unsigned char*    sdcard_get_CSD(void);
unsigned int      sdcard_get_status(void);
unsigned long int sdcard_get_read_page(void);
unsigned long int sdcard_get_write_page(void);
unsigned long int sdcard_get_total_pages(void);
void              sdcard_initialize(void);
void              sdcard_read (unsigned char *pages, unsigned char page_count);
void              sdcard_write (unsigned char *pages, unsigned char page_count);
#endif
