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
#include "memory.h"
#include "usb.h"

/** VECTOR REMAPPING *******************************************/
#define REMAPPED_RESET_VECTOR_ADDRESS 0x800
#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS 0x808
#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS 0x818

void main_hpi(void);
void main_initialize(void);
void main_lpi(void);

#pragma udata
static unsigned int timer_counter;

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
  fsm_state_t c, n, m, r;
  unsigned int cost, last, now;

  last = 0u;
  main_initialize();
  while (true)
    {
      now = timer_counter;

      if (last <= now) cost = now - last;
      else cost = 0xffff - last + now + 1;

      last = now;
      fifo_pop_state (&c, &n, &m, &r);
      fifo_broadcast_state_usb (c, n, m, r, cost);
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

  // Initialize the timer
  T3CON            = 0b00000000;  // 12 MHz clock input to TIMER3 and disabled.
  IPR2bits.TMR3IP  = 0;   // Make TIMER3 overflow a low-priority interrupt.
  PIR2bits.TMR3IF  = 0;   // Clear TIMER3 interrupt flag.
  PIE2bits.TMR3IE  = 1;   // Enable TIMER3 interrupt.
  T3CONbits.TMR3ON = 1;   // Enable TIMER3.
  
  #if defined( USE_USB_BUS_SENSE_IO )
  tris_usb_bus_sense = INPUT_PIN;
  #endif

  fsm_initialize();
  usb_initialize();
  fifo_initialize();

  // interrupts are on
  RCONbits.IPEN   = 1; // Enable prioritized interrupts.
  INTCONbits.GIEH = 1; // Enable high priority interrupts
  INTCONbits.GIEL = 1; // Enable low priority interrupts
}

#pragma interrupt main_hpi
void main_hpi(void)
{
  usb_handle();
}

#pragma interruptlow main_lpi
void main_lpi(void)
{
  if (PIR2bits.TMR3IF) // interrupt is from the timer to generate time events
    {
      PIR2bits.TMR3IF = 0;
      timer_counter++;
    }
}

