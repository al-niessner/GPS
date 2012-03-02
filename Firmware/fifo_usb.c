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

#include <stdlib.h>
#include <USB/usb.h>
#include <USB/usb_function_generic.h>

#include "fifo.h"

#pragma udata usb_stack_ram
static usb_data_packet_t usb_out[2]; // buffers for sending packets from host
static usb_data_packet_t usb_in[2];  // buffers for rcving packets to host

#pragma udata
static USB_HANDLE usb_in_h[2] = {0,0};   // endpoint handles rcving packets
static USB_HANDLE usb_out_h[2]  = {0,0}; // endpoint handles sending packets
static unsigned char usb_in_idx;
static unsigned char usb_out_idx;

#pragma code

void   fifo_initialize_usb(void)
{
  // Enable the endpoint.
  USBEnableEndpoint (USBGEN_EP_NUM,
                     USB_OUT_ENABLED | USB_IN_ENABLED |
                     USB_HANDSHAKE_ENABLED | USB_DISALLOW_SETUP);
  // Now begin waiting for the first packets to be received from the host
  usb_in_idx = 0;
  usb_in_h[0] = USBGenRead (USBGEN_EP_NUM,
                            (unsigned char*)&usb_in[0],
                            USBGEN_EP_SIZE);
  usb_in_h[1] = USBGenRead (USBGEN_EP_NUM,
                            (unsigned char*)&usb_in[1], 
                            USBGEN_EP_SIZE);
  // Initialize the pointer to the buffer which will return data to the host
  usb_out_idx = 0;
}

bool_t fifo_fetch_usb (usb_data_packet_t *result, unsigned char *len)
{
  bool_t ready = ((USBGetDeviceState() < CONFIGURED_STATE) ||
                  USBIsDeviceSuspended()                   ||
                  USBHandleBusy (usb_in_h[usb_in_idx]));

  if (ready)
    {
      *len =  USBHandleGetLength (usb_in_h[usb_in_idx]);
      memcpy ((void*)&usb_in[usb_in_idx], (void*)result, *len);
      usb_in_h[usb_in_idx] =  USBGenRead (USBGEN_EP_NUM,
                                          (unsigned char*)&usb_in[usb_in_idx],
                                          USBGEN_EP_SIZE);
      usb_in_idx ^= 1;
    }

  return ready;
}

void   fifo_push_usb (usb_data_packet_t *item, unsigned char len)
{
  unsigned char idx;

  if (len != 0u && len <= USBGEN_EP_NUM)
    {
      for (idx = 0 ; idx < len ; idx++)
        {
          usb_out[usb_out_idx]._byte[idx] = item->_byte[idx];
        }
      usb_out_h[usb_out_idx] =
        USBGenWrite (USBGEN_EP_NUM,
                     (unsigned char*)&usb_out[usb_out_idx],
                     len);
      usb_out_idx ^= 1;
      while (USBHandleBusy (usb_out_h[usb_out_idx])) {}
    }
}

bool_t fifo_waiting_usb(void)
{
  bool_t ready = ((USBGetDeviceState() < CONFIGURED_STATE) ||
                  USBIsDeviceSuspended()                   ||
                  USBHandleBusy (usb_in_h[usb_in_idx]));
  return ready;
}
