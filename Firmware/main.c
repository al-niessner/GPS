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
#include "memory.h"
#include "sdcard.h"
#include "serial.h"
#include "usb.h"

/** VECTOR REMAPPING *******************************************/
#define REMAPPED_RESET_VECTOR_ADDRESS 0x800
#define REMAPPED_HIGH_INTERRUPT_VECTOR_ADDRESS 0x808
#define REMAPPED_LOW_INTERRUPT_VECTOR_ADDRESS 0x818

void main_hpi(void);
void main_initialize(void);
void main_lpi(void);
void flash (void);

#pragma udata access fast_access
static near unsigned char count[2], idx;
static near unsigned int  flash_counter;
static near unsigned int  led_rate;

#pragma udata overlay access gps_serial
static near serial_shared_block_t serial;

#pragma udata overlay gps_serial_tx
static serial_tx_shared_block_t transmit;

#pragma udata overlay access gps_timing 
static near timing_shared_block_t timer;

#pragma udata

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
/*
void flash (void)
{
  INIT_LED();
  for (flash_counter = 0x100 ; flash_counter ; flash_counter--)
    {
      for (timer_counter = 0xffff ; timer_counter ; timer_counter--) ;
      
      if ((flash_counter & 0x010) == 0x0u) LED_TOGGLE();
    }
   _asm goto _startup _endasm
}
*/

void main(void)
{
  static unsigned int ltc, tc;
  static unsigned long int cost, last, now;

  last = 0u;
  ltc = 0u;
  led_rate = 0x40;
  main_initialize();
  LED_OFF();
  while (true)
    {
      tc = timer.counter;
      now = tc;
      now = now << 16;
      now |= TMR3H;
      now = (now << 8) | TMR3L; // shifts are in place

      if (now < last) 
        {
          if (tc == ltc) cost = 0x00010000;
          else cost = (0xffffffff - last) + now + 1;
        }
      else cost = now - last;

      if ((tc & led_rate) == 0x0u) LED_OFF();
      else LED_ON();

      last = now;
      ltc = tc;
      count[0] = count[1] = 0;
      for (idx = 0x0 ; idx < 0x20u ; idx--)
        {
          count[0] += (timer.button[0] >> idx) & 0x1;
          count[1] += (timer.button[1] >> idx) & 0x1;
        }

      if (24u < count[0] || (20u < count[0] && 24u < count[1]))
        timer.event[0] = timer.event[0] == SS_LOW ? RISING_EDGE:timer.event[0];
      else timer.event[0] = SS_LOW;

      if (24u < count[1] || (20u < count[1] && 24u < count[0]))
        timer.event[1] = timer.event[1] == SS_LOW ? RISING_EDGE:timer.event[1];
      else timer.event[1] = SS_LOW;

#if defined (USB_POLLING)
      usb_handle();
#endif

      usb_broadcast_state (cost);
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
  IPR2bits.TMR3IP  = 1;   // Make TIMER3 overflow a high-priority interrupt.
  PIR2bits.TMR3IF  = 0;   // Clear TIMER3 interrupt flag.
  PIE2bits.TMR3IE  = 1;   // Enable TIMER3 interrupt.
  T3CONbits.TMR3ON = 1;   // Enable TIMER3.
  
  #if defined (USE_USB_BUS_SENSE_IO)
  tris_usb_bus_sense = INPUT_PIN;
  #endif

  LED_ON();
  fsm_initialize();
  sdcard_initialize();
  serial_initialize();
  usb_initialize();
  
  // interrupts are on
  RCONbits.IPEN   = 1; // Enable prioritized interrupts.
  INTCONbits.GIEH = 1; // Enable high priority interrupts
  INTCONbits.GIEL = 1; // Enable low priority interrupts
}

#pragma interrupt main_hpi
void main_hpi(void)
{
  if (PIR2bits.TMR3IF) // interrupt is from the timer to generate time events
    {
      // preload the counter for a 64 Hz rate. 45 Hz is the slowest.
      TMR3L = 0x62;
      TMR3H = 0x49;
      PIR2bits.TMR3IF = 0;
      timer.counter++;

      if (timer.counter & 0x1)
        {
          timer.button[0] = (timer.button[0] << 1) | 0;
          timer.button[1] = (timer.button[1] << 1) | 0;
        }
    }

  if (PIR1bits.RCIF)
    {
      serial.buffer[serial.write_addr] = RCREG;
      serial.write_addr = (serial.write_addr + 1) & 0x1f;
      PIR1bits.RCIF = 0;
    }

  if (PIR1bits.TXIF)
    {
      if (transmit.idx == transmit.len)
        {
          transmit.idx = transmit.nidx;
          transmit.len = transmit.nlen;
          transmit.nidx = transmit.nlen = 0;
        }

      TXREG = transmit.buffer[transmit.idx];
      transmit.idx++;

      if (transmit.idx  == transmit.len &&
          transmit.nidx == transmit.nidx) PIE1bits.TXIE = 0;

      PIR1bits.TXIF = 0;
    }
}

#pragma interruptlow main_lpi
void main_lpi(void)
{
#if defined (USB_INTERRUPT)
  usb_handle();
#endif
}

#pragma code
