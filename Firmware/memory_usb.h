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
#include "usb_types.h"

extern usb_device_info_t usb_dev_info;


extern USB_HANDLE usb_in_h[2];  // endpoint handles rcving packets from host
extern USB_HANDLE usb_out_h[2]; // endpoint handles sending packets to host
extern usb_data_packet_t usb_in[2];  // buffers for rcving packets from host
extern usb_data_packet_t usb_out[2]; // buffers for sending packets to host

extern unsigned char usb_in_idx;
extern unsigned char usb_out_idx;

#endif
