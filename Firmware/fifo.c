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
#include <USB/usb_function_generic.h>

#include "fifo.h"

#pragma udata circ_buf

static unsigned char read_buffer[512];
static unsigned char write_buffer[1024];

#pragma udata

static fsm_state_t state_c, state_n, state_request, state_require;

#pragma code

void   fifo_initialize(void)
{
  state_c       = UNDEFINED;
  state_n       = UNDEFINED;
  state_request = UNDEFINED;
  state_require = UNDEFINED;
}

void fifo_pop_state  (fsm_state_t *current, fsm_state_t *next,
                      fsm_state_t *requested, fsm_state_t *required)
{
  *current   = state_c;
  *next      = state_n;
  *requested = state_request;
  *required  = state_require;
}

void fifo_push_state (fsm_state_t current, fsm_state_t next,
                      fsm_state_t requested, fsm_state_t required)
{
  if (current != UNDEFINED) state_c = current;
  if (next != UNDEFINED)    state_n = next;
  state_request = requested;
  state_require = required;
}

bool_t fifo_fetch_time_event(button_event_t *button) { return false; }
void   fifo_push_time_event(button_event_t *button) {}

char   fifo_fetch_next(void) {}
bool_t fifo_is_receiving(void) { return false; }
void   fifo_push_message (char *s) {}
void   fifo_push_serial (char c) {}
void   fifo_set_valid (bool_t b) {}
void   fifo_set_allow (bool_t b) {}
