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

#include <Compiler.h>

#include "fsm.h"
#include "HardwareProfile.h"
#include "sdcard.h"
#include "serial.h"
#include "usb.h"

#define MSG_SIZE 0x0fu
#define POP_ERR 0x1du
#pragma romdata
static const far rom char new_track[MSG_SIZE + 1]    = "\r\n$PDIYNT,*1E\r\n";
static const far rom char new_waypoint[MSG_SIZE + 1] = "\r\n$PDIYWP,*2F\r\n";
static const far rom char pop_err_msg[POP_ERR + 1] = "Must be in S0 to GPS_POP/PUSH";

#pragma udata overlay gps_fsm
static fsm_shared_block_t fsm;

#pragma udata overlay gps_sdcard
static sdcard_shared_block_t sdcard;

#pragma udata overlay access gps_timing 
static near timing_shared_block_t timer;

#pragma udata overlay gps_usb
static usb_shared_block_t usb;


#pragma udata
static bool_t basic;
static unsigned char duration[2] = {0, 0};
static unsigned char idx, val;

#pragma code

void fsm_clear(void)
{
  if (duration[0] == 3u && duration[1] == 3u)
    {
      LED_ON();
      serial_set_allow (false);
      serial_set_valid (false);
      sdcard_erase();
    }

  fsm.next = S0;
}

void fsm_idle (bool_t full)
{
  static bool_t two_button_action;

  fsm.next = full ? S1:S0;
  basic = !full;
  serial_set_allow (full);

  if      (usb_is_waiting())  fsm.next = S5;
  else if (serial_is_receiving()) fsm.next = full ? S4:S0;
  else if (false) // time event check goes here
    {
      // update duration or generate events if duration is surpassed
      if (timer.event[0] == RISING_EDGE || timer.event[0] == SS_HIGH)
        duration[0]++;
      if (timer.event[0] == SS_LOW)
        duration[0] = 0;
      if (timer.event[1] == RISING_EDGE || timer.event[1] == SS_HIGH)
        duration[1]++;
      if (timer.event[1] == SS_LOW)
        duration[1] = 0;

      two_button_action = (timer.event[0] == timer.event[1]) &&
                          (duration[1] - duration[0]) == 0u;

      if (two_button_action)
        {
          fsm.next = S6;
          timer.event[0] = SS_HIGH;
          timer.event[1] = SS_HIGH;
        }
      else // one button action
        {
          if (timer.event[0] == SS_HIGH) fsm.next = S2;
          if (timer.event[1] == SS_HIGH) fsm.next = S3;
        }

      if (duration[0] == 0u && duration[1] == 0u) LED_OFF();
    }
}

void fsm_push(void)
{
  serial_set_valid (false);
  fsm.next = S1;
}

void fsm_track(void)
{
  fsm.next = S1;

  if (duration[0] == 2u)
    {
      LED_ON();
      for (idx = 0 ; idx < MSG_SIZE ; idx++)
        {
          memcpypgm2ram ((void*)&val, (const far rom void*)&new_track[idx], 1);
          sdcard_write (val);
        }
      fsm.next = S7;
    }
}

void fsm_uart(void)
{
  serial_set_valid (true);
  fsm.next = serial_is_receiving() ? S4:S7;
}

void fsm_usb(void)
{
  if (usb_process())
    {
      switch (usb.inbound.cmd)
        {
        case GPS_REQUEST_CMD: // tested
          if (usb.inbound.force) fsm.required = usb.inbound.new_state;
          else fsm.requested = usb.inbound.new_state;

          if (usb.inbound.new_state == S2)
            duration[0] = usb.inbound.duration;

          if (usb.inbound.new_state == S3)
            duration[1] = usb.inbound.duration;

          if (usb.inbound.new_state == S6)
            {
              duration[0] = usb.inbound.duration;
              duration[1] = usb.inbound.duration;
            }
          usb.outbound.cmd = GPS_REQUEST_CMD;
          usb_push (1);
          break;

        case GPS_SDC_CONFIG_REQ: // tested
          usb.outbound.sdc_status = sdcard_get_status();
          memcpy (usb.outbound.cid, (const void*)sdcard.cid, 15);
          memcpy (usb.outbound.csd, (const void*)sdcard.csd, 15);
          usb_push (USBGEN_EP_SIZE);
          break;

        case GPS_SDC_STATE_REQ: // tested
          usb.outbound.cmd = GPS_SDC_STATE_REQ;
          usb.outbound.next_page_to_read = sdcard.read_page;
          usb.outbound.next_page_to_write = sdcard.write_page;
          usb.outbound.total_pages = sdcard.total_pages;
          usb_push (USBGEN_EP_SIZE);
          break;

        case GPS_POP: // tested
          if (basic)
            for (idx = 0 ; idx < USBGEN_EP_SIZE ; idx++)
              usb.outbound._byte[idx] = sdcard_read();
          else
            memcpypgm2ram ((void*)&usb.outbound._byte,
                           (const far rom void*)pop_err_msg,
                           POP_ERR + 1);

          usb_push (USBGEN_EP_SIZE);
          break;

        case GPS_PUSH: // tested
          if (basic)
            {
              for (idx = 0 ; idx < usb.inbound.len ; idx++)
                sdcard_write (usb.inbound.data_block[idx]);
              usb.outbound.len = 0;
            }
          else
            {
              usb.outbound.len = POP_ERR;
              memcpypgm2ram (&usb.outbound._byte[2],
                             (const far rom void*)pop_err_msg,
                             POP_ERR);
            }

          usb.outbound.cmd = GPS_PUSH;
          usb_push (usb.outbound.len + 2);
          break;

        case GPS_SEND:
          break;

        default:
          break;
        }
    }

  fsm.next = basic ? S0:S1;
}

void fsm_waypt(void)
{
  fsm.next = S1;

  if (duration[1] == 2u)
    {
      LED_ON();
      for (idx = 0 ; idx < MSG_SIZE ; idx++)
        {
          memcpypgm2ram ((void*)&val,
                         (const far rom void*)&new_waypoint[idx], 1);
          sdcard_write (val);
        }
      fsm.next = S7;
    }
}

void fsm_initialize(void)
{
  fsm.current = S0;
  fsm.next = S0;
  fsm.requested = UNDEFINED;
  fsm.required = UNDEFINED;
  duration[0] = 0;
  duration[1] = 0;
  LED_OFF();
}

void fsm_process (void)
{
  fsm.current = fsm.next;

  if (fsm.required < UNDEFINED)
    {
      fsm.current  = fsm.required;
      fsm.required = UNDEFINED;
    }

  if (fsm.requested < UNDEFINED && (fsm.current == S0 || fsm.current == S1))
    {
      fsm.current   = fsm.requested;
      fsm.requested = UNDEFINED;
    }

  switch (fsm.current)
    {
    case S0: fsm_idle (false); break;
    case S1: fsm_idle (true);  break;
    case S2: fsm_track();      break; // tested
    case S3: fsm_waypt();      break; // tested
    case S4: fsm_uart ();      break;
    case S5: fsm_usb  ();      break; // tested
    case S6: fsm_clear();      break; // tested
    case S7: fsm_push ();      break; 

    case UNDEFINED:
    case INDETERMINATE:
      if (usb_is_waiting()) fsm_usb(); // keep the USB alive
      fsm.next = INDETERMINATE;
      break;
    }
}
