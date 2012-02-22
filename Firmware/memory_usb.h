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

#ifndef GPS_MEMORY_USB_H
#define GPS_MEMORY_USB_H

#include <USB/usb.h>

#include "memory_version.h"
#include "memory.h"

#pragma udata usb_stack_ram
static usb_data_packet_t usb_out[2]; // buffers for sending packets from host
static usb_data_packet_t usb_in[2];  // buffers for rcving packets to host

#pragma udata
static USB_HANDLE usb_in_h[2] = {0,0};   // endpoint handles rcving packets
static USB_HANDLE usb_out_h[2]  = {0,0}; // endpoint handles sending packets
static unsigned char usb_in_idx;
static unsigned char usb_out_idx;


#endif
