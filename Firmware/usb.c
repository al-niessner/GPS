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

#include <USB/usb.h>
#include <USB/usb_function_generic.h>

#include "memory_eeprom.h"
#include "memory_version.h"
#include "usb.h"

#define GPS_PRODUCT_ID 0x00,0x03

#pragma romdata
static const far rom usb_device_info_t usb_dev_info =
  {
    GPS_PRODUCT_ID,
    GPS_MAJOR_VERSION, GPS_MINOR_VERSION, GPS_BUGFIX_VERSION,
    "GPS on XuDL: Al Niessner", // Description string.
    0x00               // Checksum (filled in later).
  };

#pragma udata overlay gps_fsm
static fsm_shared_block_t fsm;

#pragma udata overlay gps_sdcard
static sdcard_shared_block_t sdcard;

#pragma udata overlay gps_usb
static usb_shared_block_t usb;

#pragma udata usb_stack_ram
static usb_data_packet_t usb_out[2]; // buffers for sending packets from host
static usb_data_packet_t usb_in[2];  // buffers for rcving packets to host

#pragma udata
static USB_HANDLE usb_in_h[2];   // endpoint handles rcving packets
static USB_HANDLE usb_out_h[2]; // endpoint handles sending packets
static unsigned char usb_in_idx;
static unsigned char usb_out_idx;

static bool_t ready;

#pragma code

void check_usb(void)
{
  ready = !((USBGetDeviceState() < CONFIGURED_STATE) ||
            USBIsDeviceSuspended()                   ||
            USBHandleBusy (usb_in_h[usb_in_idx]));
}

void initialize_usb(void)
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

void usb_broadcast_state (unsigned long int timing)
{
  check_usb();

  if (ready) ready = usb_in[usb_in_idx].cmd == GPS_STATE_REQ;
  if (ready)
    {
      usb_in_h[usb_in_idx] =  USBGenRead (USBGEN_EP_NUM,
                                          (unsigned char*)&usb_in[usb_in_idx],
                                          USBGEN_EP_SIZE);
      usb_in_idx ^= 1;
      usb.outbound.cmd = GPS_STATE_REQ;
      usb.outbound.bits.my_true = true;
      usb.outbound.bits.xfer_is_reading = sdcard.isReading;
      usb.outbound.bits.xfer_crc_match = sdcard.isValidCRC;
      usb.outbound.current = fsm.current;
      usb.outbound.next = fsm.next;
      usb.outbound.requested = fsm.requested;
      usb.outbound.required = fsm.required;
      usb.outbound.timing = timing;
      usb.outbound.sdcard_init = sdcard.step;
      usb.outbound.last_r1 = sdcard.last_r1;
      usb.outbound.sdcard_version = sdcard.ver;
      usb.outbound.xfer_r1 = sdcard.block_r1;
      usb_push (USBGEN_EP_SIZE - sizeof (usb.outbound.unused_req));
    }
}

unsigned char usb_fetch(void)
{
  unsigned char len;

  check_usb();

  if (ready)
    {
      len = USBHandleGetLength (usb_in_h[usb_in_idx]);
      memcpy (&usb.inbound, (const void*)&usb_in[usb_in_idx], len);
      usb_in_h[usb_in_idx] = USBGenRead (USBGEN_EP_NUM,
                                         (unsigned char*)&usb_in[usb_in_idx],
                                         USBGEN_EP_SIZE);
      usb_in_idx ^= 1;
    }
  else len = 0;

  return len;
}

void usb_handle(void)
{
  USBDeviceTasks();
}

void usb_initialize(void)
{
  USBDeviceInit();

#if defined (USB_INTERRUPT)
  USBDeviceAttach();
#endif
}

bool_t usb_process (void)
{
  static bool_t do_more;
  static unsigned char len;
  do_more = false;

  if (0u < usb_fetch())
    {
      static unsigned char buffer_cntr;
      static unsigned char num_return_bytes;
      static unsigned int  lcntr;
      
      num_return_bytes = 0;  // Initially, assume nothing needs to be returned
      switch (usb.inbound.cmd)
        {
        case GPS_REQUEST_CMD:
        case GPS_SDC_CONFIG_REQ:
        case GPS_SDC_STATE_REQ:
        case GPS_POP:
        case GPS_PUSH:
        case GPS_SEND:
          do_more = true;
          break;

        case GPS_VER_CMD:
          // Return a packet with information about this USB interface device.
          usb.outbound.cmd = usb.inbound.cmd;
          memcpypgm2ram ((void*)&usb.outbound.info,
                         (const far rom void*)&usb_dev_info,
                         sizeof (usb_device_info_t));
          num_return_bytes  = sizeof (usb_device_info_t) + 1;
          break;
              
        case READ_EEDATA_CMD:
          usb.outbound.cmd = usb.inbound.cmd;
          for (buffer_cntr=0; buffer_cntr < usb.inbound.len; buffer_cntr++)
            {
              usb.inbound.data[buffer_cntr] = 
                eeprom_read ((unsigned char)usb.inbound.ADR.pAdr + buffer_cntr);
            }
          num_return_bytes = buffer_cntr + 5;
          break;
              
        case WRITE_EEDATA_CMD:
          usb.outbound.cmd = usb.inbound.cmd;
          for(buffer_cntr=0; buffer_cntr < usb.inbound.len; buffer_cntr++)
            {
              eeprom_write ((BYTE)usb.inbound.ADR.pAdr + buffer_cntr,
                            usb.inbound.data[buffer_cntr]);
            }
          num_return_bytes = 1;
          break;
              
        case RESET_CMD:
          // When resetting, make sure to drop the device off the bus
          // for a period of time. Helps when the device is suspended.
          UCONbits.USBEN = 0;
          lcntr = 0xFFFF;
          for(lcntr = 0xFFFF; lcntr; lcntr--) ;
          Reset();
          break;
          
        default:
          num_return_bytes = 0;
          break;
        } /* switch */

      usb_push (num_return_bytes);
    }

  return do_more;
}

void usb_push (unsigned char len)
{
  unsigned char idx;

  if (0u < len && len <= USBGEN_EP_SIZE)
    {
      memcpy ((void*)&usb_out[usb_out_idx], (const void*)&usb.outbound, len);
      usb_out_h[usb_out_idx] =
        USBGenWrite (USBGEN_EP_NUM,
                     (unsigned char*)&usb_out[usb_out_idx],
                     len);
      usb_out_idx ^= 1;
      while (USBHandleBusy (usb_out_h[usb_out_idx])) {}
    }
}

bool_t usb_is_waiting(void)
{
  check_usb();

  if (ready) ready = usb_in[usb_in_idx].cmd != GPS_STATE_REQ;

  return ready;
}
