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

#ifndef GPS_MEMORY_H
#define GPS_MEMORY_H

typedef enum { false=0==1, true=0==0 } bool_t;


/**
  * USB Types
 **/

#include "usb.h"
#include "usb_config.h"

typedef struct usb_device_info
{
  unsigned char product_id[2];
  unsigned char version_id[3];
  unsigned char desc[USBGEN_EP_SIZE - 7]; // 7 is number of bytes in union below
  unsigned char checksum;
} usb_device_info_t;

typedef union usb_data_packet
{
  unsigned char  _byte[USBGEN_EP_SIZE];      // For byte access.
  unsigned short _word[USBGEN_EP_SIZE / 2];  // For word access.
  unsigned long  _dword[USBGEN_EP_SIZE / 4]; // For double-word access.
  struct
  {
    usb_cmd_t     cmd;
    unsigned char len;
  };
  struct
  {
    usb_cmd_t     cmd;
    usb_device_info_t info;
  };
  struct // EEPROM read/write structure
  {
    usb_cmd_t     cmd;
    unsigned char len;
    union
    {
      rom far char *pAdr; // Address pointer
      struct
      {
        unsigned char low;   // Little-endian order
        unsigned char high;
        unsigned char upper;
      };
    } ADR;
    unsigned char data[USBGEN_EP_SIZE - 5];
  };
} usb_data_packet_t;

#endif
