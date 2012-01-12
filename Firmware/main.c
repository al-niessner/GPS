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
    This module starts the user code or else it controls
    the programming of the flash with a new program.
   ----------------------------------------------------------------------------------*/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include "System\typedefs.h"                        // Required
#include "System\usb\usb.h"                         // Required
#include "io_cfg.h"                                 // Required
#include "eeprom_flags.h"                           // Required

#include "System\usb\usb_compile_time_validation.h" // Optional

#pragma code _HIGH_INTERRUPT_VECTOR = 0x000008
void _high_ISR (void)
{
    _asm goto RM_HIGH_INTERRUPT_VECTOR _endasm
}

#pragma code _LOW_INTERRUPT_VECTOR = 0x000018
void _low_ISR (void)
{
    _asm goto RM_LOW_INTERRUPT_VECTOR _endasm
}

#pragma code

void main(void)
{
    // Initialize firmware update input pin so pullup has time to work.
//    INIT_FMWB();

    TRISC = 0xFF & ~LED_MASK; // Outputs: LED.

    // Check to see if the user-mode firmware is being updated.
    // During boot, the uC checks EEPROM location BOOT_SELECT_FLAG_ADDR.
    // If this location contains BOOT_INTO_USER_MODE, then the uC
    // jumps to the user program.  If this location contains
    // BOOT_INTO_REFLASH_MODE, then the uC initializes its USB interface
    // and waits for packets to reprogram the part of the flash that
    // contains the user program.  If the location contains neither code,
    // then the uC will jump to the user program if the FMWB pin is high.
    // Otherwise, it will jump to the code for reprogramming the flash.
    EECON1 = 0x00;
    EEADR  = BOOT_SELECT_FLAG_ADDR;
    EECON1_RD = 1;
    if( EEDATA==BOOT_INTO_USER_MODE || (EEDATA!=BOOT_INTO_REFLASH_MODE && FMWB==1) )
    { // Go into user mode.
        _asm goto RM_RESET_VECTOR _endasm
    }
    
    // Initiate mode to update firmware via USB.
    mInitializeUSBDriver();     // See usbdrv.h
    USBCheckBusStatus();        // Modified to always enable USB module
    while(1)
    {
        USBDriverService();     // See usbdrv.c
        BootService();          // See boot.c
    }
}

/* Place the flag to boot into reflash mode into location 0xFF of the EEPROM. */
#pragma romdata EEPROMSECT = 0xF00000
rom unsigned char eedata[BOOT_SELECT_FLAG_ADDR]; // Move to location for boot-select flag.
rom unsigned char boot_select_flag[] = {BOOT_INTO_REFLASH_MODE};

/* Return to the default romdata section. */
#pragma romdata

#pragma code user = RM_RESET_VECTOR
