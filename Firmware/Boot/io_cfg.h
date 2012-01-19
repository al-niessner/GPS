/*----------------------------------------------------------------------------------
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
    02111-1307, USA.

    ©2011 - X Engineering Software Systems Corp.
   ----------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------
    Module Description:
    This module maps pins to their functions.  This provides a layer
    of abstraction.
   ----------------------------------------------------------------------------------*/

#ifndef IO_CFG_H
#define IO_CFG_H

#include "AutoFiles\usbcfg.h"

#define CLOCK_FREQ  48000000           // Clock frequency in Hz.
#define MIPS        12                 // Number of processor instructions per microsecond.

#define INPUT_PIN   1
#define OUTPUT_PIN  0

/** Pin definition macros *******************************************/
#define TRIS( P, B )        ( TRIS ## P ## bits.TRIS ## P ## B )
#define PORT( P, B )        ( PORT ## P ## bits.R ## P ## B )
#define PORT_ASM( P, B )    PORT ## P ##, B, ACCESS
#define LATCH( P, B )       ( LAT ## P ## bits.LAT ## P ## B )
#define LATCH_ASM( P, B )   LAT ## P ##, B, ACCESS

/** Sense presence of external power (not used) *********************/
#if defined( USE_SELF_POWER_SENSE_IO )
#define tris_self_power     TRIS( A, 2 )
#define self_power          PORT( A, 2 )
#else
#define self_power          1
#endif

/** Sense presence of USB bus (not used) *****************************/
#if defined( USE_USB_BUS_SENSE_IO )
#define tris_usb_bus_sense  TRIS( C, 1 )
#define USB_BUS_SENSE       PORT( C, 1 )
#else
#define USB_BUS_SENSE       1
#endif

/** Firmware update jumper sense ************************************/
#define FMWB_PORT   B
#define FMWB_BIT    5
#define FMWB_MASK   ( 1 << FMWB_BIT )
#define FMWB_TRIS   TRIS( B, 5 )
#define FMWB        PORT( B, 5 )
#define FMWB_ASM    PORT_ASM( B, 5 )
#define INIT_FMWB() ANSELH = 0, INTCON2bits.NOT_RABPU = 0, LATCH( B, 5 ) = 1, FMWB_TRIS = INPUT_PIN

/** LED *************************************************************/
#define LED_PORT        C
#define LED_BIT         4
#define LED_MASK        ( 1 << LED_BIT )
#define LED_TRIS        TRIS( C, 4 )
#define LED             LATCH( C, 4 )
#define LED_ASM         PORT_ASM( C, 4 )
#define LED_OFF()       LED = 0
#define LED_ON()        LED = 1
#define LED_TOGGLE()    LED = !LED
#define INIT_LED()      LED_OFF(), LED_TRIS = OUTPUT_PIN

/** Some common uC bits ********************************************/
// ALU carry bit.
#define CARRY_POS       0
#define CARRY_BIT_ASM   STATUS, CARRY_POS, ACCESS
// MSSP buffer-full bit.
#define MSSP_BF_POS     0
#define MSSP_BF_ASM     WREG, MSSP_BF_POS, ACCESS

// Converse of using ACCESS flag for destination register.
#define TO_WREG         0


#endif
