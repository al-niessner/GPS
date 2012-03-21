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

#ifndef GPS_FIFO_H
#define GPS_FIFO_H

#include "memory.h"

void   fifo_initialize(void);
void   fifo_initialize_usb(void);

bool_t fifo_fetch_time_event(button_event_t *button); // array of 2
void   fifo_push_time_event(button_event_t *button); // array of 2

void   fifo_broadcast_xfer_usb (bool_t isReading,
                                unsigned char r1,
                                bool_t isValidCRC);
void   fifo_broadcast_sdcard_usb (sdcard_init_step_t step,
                                  unsigned char r1,
                                  unsigned char ver);
void   fifo_broadcast_state_usb (unsigned long int timing);

unsigned char fifo_fetch_usb(void);
void          fifo_push_usb (unsigned char len);
bool_t        fifo_waiting_usb(void); // returns true if there is a message waiting to be processed

char   fifo_fetch_next(void);
bool_t fifo_is_receiving(void);
void   fifo_push_message (char *s);
void   fifo_push_serial (char c);
void   fifo_set_valid (bool_t b);
void   fifo_set_allow (bool_t b);
#endif
