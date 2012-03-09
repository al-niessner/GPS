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
static USB_HANDLE usb_in_h[2];   // endpoint handles rcving packets
static USB_HANDLE usb_out_h[2]; // endpoint handles sending packets
static unsigned char usb_in_idx;
static unsigned char usb_out_idx;

static bool_t ready;
static sdcard_init_step_t last_step;
static unsigned char last_r1, last_ver;

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

void fifo_broadcast_sdcard_usb (sdcard_init_step_t step,
                                unsigned char r1,
                                unsigned char ver)
{
  last_step = step;
  last_r1 = r1;
  last_ver = ver;
}

void fifo_broadcast_state_usb (fsm_state_t current, fsm_state_t next,
                               fsm_state_t requested, fsm_state_t required,
                               unsigned long int timing)
{
  ready = !((USBGetDeviceState() < CONFIGURED_STATE) ||
            USBIsDeviceSuspended()                   ||
            USBHandleBusy (usb_in_h[usb_in_idx]));

  if (ready) ready = usb_in[usb_in_idx].cmd == GPS_STATE_REQ;

  if (ready)
    {
      usb_data_packet_t item;

      usb_in_h[usb_in_idx] =  USBGenRead (USBGEN_EP_NUM,
                                          (unsigned char*)&usb_in[usb_in_idx],
                                          USBGEN_EP_SIZE);
      usb_in_idx ^= 1;
      item.cmd = GPS_STATE_REQ;
      item.my_true = true;
      item.current = current;
      item.next = next;
      item.requested = requested;
      item.required = required;
      item.timing = timing;
      item.sdcard_init = last_step;
      item.last_r1 = last_r1;
      item.sdcard_version = last_ver;
      fifo_push_usb (&item, USBGEN_EP_SIZE - sizeof (item.unused_req));
    }
}

bool_t fifo_fetch_usb (usb_data_packet_t *result, unsigned char *len)
{
  ready = !((USBGetDeviceState() < CONFIGURED_STATE) ||
            USBIsDeviceSuspended()                   ||
            USBHandleBusy (usb_in_h[usb_in_idx]));

  if (ready)
    {
      *len =  USBHandleGetLength (usb_in_h[usb_in_idx]);
      memcpy ((void*)result, (const void*)&usb_in[usb_in_idx], *len);
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

  if (0u < len && len <= USBGEN_EP_SIZE)
    {
      memcpy (&usb_out[usb_out_idx], item, len);
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
  ready = !((USBGetDeviceState() < CONFIGURED_STATE) ||
            USBIsDeviceSuspended()                   ||
            USBHandleBusy (usb_in_h[usb_in_idx]));

  if (ready) ready = usb_in[usb_in_idx].cmd != GPS_STATE_REQ;

  return ready;
}
