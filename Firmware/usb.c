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
#include "memory_version.h"
#include "usb.h"
#include "usb_eeprom.h"

#define GPS_PRODUCT_ID 0x00,0x03

#pragma romdata
static const rom usb_device_info_t usb_dev_info =
  {
    GPS_PRODUCT_ID,
    GPS_MAJOR_VERSION, GPS_MINOR_VERSION, GPS_BUGFIX_VERSION,
    "GPS on XuDL: Al Niessner", // Description string.
    0x00               // Checksum (filled in later).
  };

#pragma udata

static usb_data_packet_t usb_inbound;
static usb_data_packet_t usb_outbound;

#pragma code

void usb_handle(void)
{
  USBDeviceTasks();
}

void usb_initialize(void)
{
  USBDeviceInit();
  USBDeviceAttach();
}

void usb_process(void)
{
  unsigned char len;

  if (fifo_fetch_usb (&usb_inbound, &len))
    {
      unsigned char buffer_cntr;
      unsigned char num_return_bytes; // Number of bytes to return in response
      unsigned char cmd;              // Store the command in the rcved packet
      unsigned int  lcntr;
      
      num_return_bytes = 0;  // Initially, assume nothing needs to be returned
      switch (usb_inbound.cmd)
        {
        case ID_BOARD_CMD:
          usb_outbound.cmd = cmd;
          num_return_bytes = 1;
          break;

        case INFO_CMD:
          usb_outbound._word[2]  = 0;
          usb_outbound._word[3]  = 0;
          usb_outbound._word[4]  = 0;
          num_return_bytes = 16;
          break;
          
        case GPS_VER_CMD:
          // Return a packet with information about this USB interface device.
          usb_outbound.cmd = cmd;
          memcpypgm2ram (&usb_outbound.info,
                         &usb_dev_info,
                         sizeof (usb_device_info_t));
          num_return_bytes  = sizeof (usb_device_info_t) + 1;
          break;
              
        case READ_EEDATA_CMD:
          usb_outbound.cmd = usb_inbound.cmd;
          for (buffer_cntr=0; buffer_cntr < usb_inbound.len; buffer_cntr++)
            {
              usb_inbound.data[buffer_cntr] = 
                eeprom_read ((unsigned char)usb_inbound.ADR.pAdr + buffer_cntr);
            }
          num_return_bytes = buffer_cntr + 5;
          break;
              
        case WRITE_EEDATA_CMD:
          usb_outbound.cmd = usb_inbound.cmd;
          for(buffer_cntr=0; buffer_cntr < usb_inbound.len; buffer_cntr++)
            {
              eeprom_write ((BYTE)usb_inbound.ADR.pAdr + buffer_cntr,
                            usb_inbound.data[buffer_cntr]);
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

      if (num_return_bytes != 0U)
        fifo_push_usb (&usb_outbound, num_return_bytes);
    }
}

