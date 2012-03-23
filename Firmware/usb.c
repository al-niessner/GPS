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

#include "fifo.h"
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

#pragma udata overlay gps_usb
static usb_shared_block_t usb;

#pragma udata

#pragma code

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

  if (0u < fifo_fetch_usb())
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

      fifo_push_usb (num_return_bytes);
    }

  return do_more;
}

