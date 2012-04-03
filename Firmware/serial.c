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
#include <delays.h>

#include "HardwareProfile.h"
#include "serial.h"

#pragma udata overlay access gps_serial
static near serial_shared_block_t serial;

#pragma udata overlay gps_serial_tx
static serial_tx_shared_block_t transmit;

#pragma udata
static bool_t        active, receiving, valid;
static unsigned char idx, wa;

#pragma code

void serial_initialize(void)
{
  active = false;
  receiving = false;
  valid = false;
  serial.read_addr  = 0x0;
  serial.write_addr = 0x0;

  INIT_RX();
  IPR1bits.RCIP  = 1; // Make RX overflow a high-priority interrupt.
  PIR1bits.RCIF  = 0; // Clear RX interrupt flag.
  PIE1bits.RCIE  = 1; // Enable RX interrupt.

  INIT_TX();
  IPR1bits.TXIP  = 1; // Make TX overflow a high-priority interrupt.
  PIR1bits.TXIF  = 0; // Clear TX interrupt flag.
  PIE1bits.TXIE  = 0; // Disable TX interrupt.

  TXSTA   = 0b00100100; // Enable transmitter, async, on high speed
  RCSTA   = 0b10010000; // Enable the serial port and enables receiving
  BAUDCON = 0b00000000; // Set up baud data
  SPBRG   = 12;         // 57.6 K with 12 MHz Fosc
}

bool_t serial_is_receiving(void)
{
  if (active)
    {
      wa = serial.write_addr;
      
      if (!receiving && (serial.read_addr != wa))
        {
          while (serial.buffer[serial.read_addr] != '$' &&
                 serial.read_addr != wa)
            serial.read_addr = (serial.read_addr + 1) & 0x1f;
          receiving = serial.buffer[serial.read_addr] == '$';

          if (serial.read_addr < wa)
            receiving &= (wa - serial.read_addr < 29u);
          if (wa < serial.read_addr)
            receiving &= ((wa + 0x20) - serial.read_addr < 29u);
        }
    }
  else receiving = false;

  return receiving;
}

unsigned char serial_pop(void)
{
  unsigned char val = 0;

  if (serial.read_addr != serial.write_addr)
    {
      val = serial.buffer[serial.read_addr];
      serial.read_addr = (serial.read_addr + 1) & 0x1f;
      receiving = val != '\n';
    }

  return val;
}

bool_t serial_send (unsigned char offset, unsigned char len)
{
  bool_t result = false;

  for (idx = 0xff ; idx && transmit.nidx != transmit.nlen ; idx--)
    Delay100TCYx (1);

  if (transmit.nidx == transmit.nlen)
    {
      result = true;
      transmit.nidx = offset;
      transmit.nlen = offset + len;
      PIE1bits.TXIE = 1;
    }

  return result;
}

unsigned char serial_send_offset(void)
{
  return (30u < transmit.len) ? 0 : 30;
}

void serial_set_allow (bool_t b)
{
  active = b;
}

void serial_set_valid (bool_t b)
{
  valid = b;
}

