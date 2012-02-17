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

#include "usb.h"

#pragma code

void ctxt_initialize(void)
{
  // interrupts are off
  INTCONbits.GIEH = 0;
  INTCONbits.GIEL = 0;

  // Enable high slew-rate for the I/O pins.
  SLRCON = 0;

  // Turn off analog input mode on I/O pins.
  ANSEL = 0;
  ANSELH = 0;

  // Initialize the I/O pins.
  INIT_GPIO0();
  INIT_GPIO1();
  INIT_GPIO2();
  INIT_GPIO3();

  #if defined( USE_USB_BUS_SENSE_IO )
  tris_usb_bus_sense = INPUT_PIN;
  #endif

  usb_initialize();
  // interrupts are on
  RCONbits.IPEN     = 1;      // Enable prioritized interrupts.
  INTCONbits.GIEH = 1;
  INTCONbits.GIEL = 1;
}

#pragma interrupt ctxt_hpi
void ctxt_hpi(void)
{
  usb_handler();
}

#pragma interruptlow ctxt_lpi
void ctxt_lpi(void)
{
}

void ctxt_nominal(void)
{
  usb_process();
}
