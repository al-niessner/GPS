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

#include "fifo.h"
#include "memory.h"
#include "usb.h"
#include "USB/usb.h"
#include "USB/usb_function_generic.h"

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
      unsigned char num_return_bytes; // Number of bytes to return in response
      unsigned char cmd;              // Store the command in the rcved packet
      unsigned int  lcntr;

      num_return_bytes = 0;  // Initially, assume nothing needs to be returned
      blink_counter    = 10; // Blink the LED whenever a USB 
      switch (inbound.bcmd)
        {
        case ID_BOARD_CMD:
          // Blink the LED in order to identify the board.
          blink_counter                  = 50;
          InPacket->cmd                  = cmd;
          num_return_bytes               = 1;
          break;

        case INFO_CMD:
          usb_outbound._dword[0] = numBlocks;
          usb_outbound._word[2]  = rdBlockSize;
          usb_outbound._word[3]  = wrBlockSize;
          usb_outbound._word[4]  = eraseSize;
          num_return_bytes = 16;
          break;
          
          // Return a packet with information about this USB interface device.
          usb_outbound.cmd = cmd;
          memcpypgm2ram ((void*)( (BYTE *)usb_outbound.info),
                         (const rom void*)&usb_dev_info,
                         sizeof (usb_device_info));
          num_return_bytes  = sizeof (usb_device_info) + 1;
          break;
              
        case READ_EEDATA_CMD:
          usb_outbound->cmd = usb_inbound->cmd;
          for(buffer_cntr=0; buffer_cntr < usb_inbound.len; buffer_cntr++)
            {
              usb_inbound.data[buffer_cntr] = ReadEeprom((unsigned char)usb_inbound.ADR.pAdr + buffer_cntr);
            }
          num_return_bytes = buffer_cntr + 5;
          break;
              
        case WRITE_EEDATA_CMD:
          usb_outbound->cmd = usb_inbound->cmd;
          for(buffer_cntr=0; buffer_cntr < usb_inbound.len; buffer_cntr++)
            {
              WriteEeprom((BYTE)usb_inbound.ADR.pAdr + buffer_cntr,
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
      
      if ( num_return_bytes != 0U )
        {
          InHandle[InIndex] = USBGenWrite( USBGEN_EP_NUM, (BYTE *)InPacket, num_return_bytes ); // Now send the packet.
          InIndex ^= 1;
          while ( USBHandleBusy( InHandle[InIndex] ) )
            {
              ;                           // Wait until transmitter is not busy.
            }
          InPacket = &InBuffer[InIndex];
            }
    }
}

void usb_respond(void)
{
}

