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

#include "fifo.h"
#include "fsm.h"
#include "HardwareProfile.h"
#include "usb.h"

#pragma udata

static bool_t basic;
static fsm_state_t current   = INDETERMINATE,
                   requested = UNDEFINED,
                   required  = UNDEFINED;
static unsigned int duration[2] = {0, 0};
static user_request_t user_req;

#pragma code

fsm_state_t fsm_adjust (fsm_state_t desire)
{
  fsm_state_t next = desire;

  if (required < UNDEFINED)
    {
      next = required;
      required = UNDEFINED;
    }

  if (requested < UNDEFINED && (next == S0 || next == S2))
    {
      next = requested;
      requested = UNDEFINED;
    }

  return next;
}

fsm_state_t fsm_clear(void)
{
  if (duration[0] == 3u && duration[1] == 3u)
    {
      LED_ON();
      // TODO: erase SD memory card
    }

  return S0;
}

fsm_state_t fsm_idle (bool_t full)
{
  bool_t two_button_action;
  button_event_t be[2];
  fsm_state_t next = full ? S1:S0;

  basic = !full;
  fifo_set_allow (full);

  if      (fifo_waiting_usb())  next = S5;
  else if (fifo_is_receiving()) next = full ? S4:S0;
  else if (fifo_fetch_time_event (be))
    {
      // update duration or generate events if duration is surpassed
      if (be[0] == RISING_EDGE || be[0] == SS_HIGH) duration[0]++;
      if (be[0] == SS_LOW)                          duration[0] = 0;
      if (be[1] == RISING_EDGE || be[1] == SS_HIGH) duration[1]++;
      if (be[1] == SS_LOW)                          duration[1] = 0;

      two_button_action = (be[0] == be[1]) && (duration[1] - duration[0]) == 0u;

      if (two_button_action) next = S6;
      else // one button action
        {
          if (be[0] == SS_HIGH) next = S2;
          if (be[1] == SS_HIGH) next = S3;
        }

      if (duration[0] == 0u && duration[1] == 0u) LED_OFF();
    }

  return next;
}

fsm_state_t fsm_pop(void)
{
  // TODO: get the next message from the FIFO and send it back via the USB  
  return basic ? S0:S1;
}

fsm_state_t fsm_push(void)
{
  fifo_set_valid (false);
  return S1;
}

fsm_state_t fsm_send(void)
{
  // TODO: send a message to the GPS receiver via the serial bus
  return basic ? S0:S1;
}

fsm_state_t fsm_track(void)
{
  if (duration[0] == 2u)
    {
      LED_ON();
      // TODO: send $PDIYNT„*1E<CR><LF>
    }

  return S1;
}

fsm_state_t fsm_uart(void)
{
  fifo_set_valid (true);
  return fifo_is_receiving() ? S4:S7;
}

fsm_state_t fsm_usb(void)
{

  if (usb_process (&user_req))
    {
      switch (user_req.command)
        {
        case GPS_REQUEST_CMD:
          if (user_req.details.force) fsm_set_state (user_req.details.state);
          else fsm_request_state (user_req.details.state);
          break;

        default:
          break;
        }
    }

  return S1;
}

fsm_state_t fsm_waypt(void)
{
  if (duration[1] == 2u)
    {
      LED_ON();
      // TODO: send $PDIYWP,*2F<CR><LF>
    }

  return S1;
}

void fsm_initialize(void)
{
  current = S0;
  duration[0] = 0;
  duration[1] = 0;
  requested = UNDEFINED;
  required = UNDEFINED;
  LED_OFF();
}

void fsm_process (void)
{
  fsm_state_t next=INDETERMINATE, working=fsm_adjust (current);

  switch (working)
    {
    case S0: next = fsm_idle (false); break;
    case S1: next = fsm_idle (true);  break;
    case S2: next = fsm_track();      break;
    case S3: next = fsm_waypt();      break;
    case S4: next = fsm_uart ();      break;
    case S5: next = fsm_usb  ();      break;
    case S6: next = fsm_clear();      break;
    case S7: next = fsm_push ();      break;
    case S8: next = fsm_send ();      break;
    case S9: next = fsm_pop  ();      break;

    case UNDEFINED:
    case INDETERMINATE:
      next = INDETERMINATE;
      break;
    }
  current = fsm_adjust (next);
}

void fsm_request_state (fsm_state_t state)
{
  if (state < UNDEFINED) requested = state;
}

void fsm_set_state (fsm_state_t state)
{
  if (state < UNDEFINED) required = state;
}
