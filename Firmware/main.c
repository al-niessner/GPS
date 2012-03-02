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

#include "fifo.h"
#include "fsm.h"
#include "HardwareProfile.h"
#include "memory.h"
#include "usb.h"

/** VECTOR REMAPPING *******************************************/
#define REMAPPED_RESET_VECTOR_ADDRESS 0x800
#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS 0x808
#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS 0x818

void main_hpi(void);
void main_initialize(void);
void main_lpi(void);

#pragma code REMAPPED_RESET_VECTOR = REMAPPED_RESET_VECTOR_ADDRESS
extern void _startup( void );        // See c018i.c in your C18 compiler dir
void _reset( void )
{
    _asm goto _startup _endasm
}

#pragma code REMAPPED_HIGH_INTERRUPT_VECTOR = REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS
void Remapped_High_ISR( void )
{
    _asm goto main_hpi _endasm
}

#pragma code REMAPPED_LOW_INTERRUPT_VECTOR = REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS
void Remapped_Low_ISR( void )
{
    _asm goto main_lpi _endasm
}



#pragma code

void main(void)
{
  main_initialize();
  while (true)
    {
      fsm_process();
    }
}

void main_initialize(void)
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
  INIT_LED();

  #if defined( USE_USB_BUS_SENSE_IO )
  tris_usb_bus_sense = INPUT_PIN;
  #endif

  fsm_initialize();
  usb_initialize();
  fifo_initialize();

  // interrupts are on
  RCONbits.IPEN   = 1;      // Enable prioritized interrupts.
  INTCONbits.GIEH = 1;
  INTCONbits.GIEL = 1;
}

#pragma interrupt main_hpi
void main_hpi(void)
{
  usb_handle();
}

#pragma interruptlow main_lpi
void main_lpi(void)
{
}

