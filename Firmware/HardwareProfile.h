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

#ifndef HARDWAREPROFILE_H
#define HARDWAREPROFILE_H

//Uncomment one of the following lines to make the output HEX of this
//  project work with the HID or Microchip Bootloader
//#define PROGRAMMABLE_WITH_USB_HID_BOOTLOADER
#define PROGRAMMABLE_WITH_USB_MCHPUSB_BOOTLOADER

#define CLOCK_FREQ  48000000           // Clock frequency in Hz.
#define MIPS        12                 // Number of processor instructions per microsecond.

#define PRODUCTION_VERSION

#define INPUT_PIN   1
#define OUTPUT_PIN  0

/** Pin definition macros *******************************************/
#define TRIS( P, B )        ( TRIS ## P ## bits.TRIS ## P ## B )
#define PORT( P, B )        ( PORT ## P ## bits.R ## P ## B )
#define PORT_ASM( P, B )    PORT ## P ##, B, ACCESS
#define LATCH( P, B )       ( LAT ## P ## bits.LAT ## P ## B )
#define LATCH_ASM( P, B )   LAT ## P ##, B, ACCESS


/** LED **************************************************************/
#define MON_PORT        C
#define MON_BIT         0
#define MON_MASK        ( 1 << MON_BIT )
#define MON_TRIS        TRIS( C, 0 )
#define MON             LATCH( C, 0 )
#define MON_ASM         PORT_ASM( C, 0 )
#define MON_OFF()       MON = 0
#define MON_TOGGLE()    MON = !MON
#define INIT_MON()      MON_OFF(), MON_TRIS = OUTPUT_PIN

/** General-purpose digital I/O 0 pin control ************************/
#define GPIO0_PORT      C
#define GPIO0_BIT       0
#define GPIO0_MASK      ( 1 << GPIO0_BIT )
#define GPIO0_TRIS      TRIS( C, 0 )
#define GPIO0_DIR       GPIO0_TRIS
#define GPIO0           PORT( C, 0 )
#define GPIO0_ASM       PORT_ASM( C, 0 )
#define INIT_GPIO0()    GPIO0 = 0, GPIO0_TRIS = INPUT_PIN

/** General-purpose digital I/O 1 pin control ************************/
#define GPIO1_PORT      C
#define GPIO1_BIT       1
#define GPIO1_MASK      ( 1 << GPIO1_BIT )
#define GPIO1_TRIS      TRIS( C, 1 )
#define GPIO1_DIR       GPIO1_TRIS
#define GPIO1           PORT( C, 1 )
#define GPIO1_ASM       PORT_ASM( C, 1 )
#define INIT_GPIO1()    GPIO1 = 0, GPIO1_TRIS = INPUT_PIN

/** General-purpose digital I/O 2 pin control ************************/
#define GPIO2_PORT      C
#define GPIO2_BIT       2
#define GPIO2_MASK      ( 1 << GPIO2_BIT )
#define GPIO2_TRIS      TRIS( C, 2 )
#define GPIO2_DIR       GPIO2_TRIS
#define GPIO2           PORT( C, 2 )
#define GPIO2_ASM       PORT_ASM( C, 2 )
#define INIT_GPIO2()    GPIO2 = 0, GPIO2_TRIS = INPUT_PIN

/** General-purpose digital I/O 3 pin control ************************/
#define GPIO3_PORT      C
#define GPIO3_BIT       3
#define GPIO3_MASK      ( 1 << GPIO3_BIT )
#define GPIO3_TRIS      TRIS( C, 3 )
#define GPIO3_DIR       GPIO3_TRIS
#define GPIO3           PORT( C, 3 )
#define GPIO3_ASM       PORT_ASM( C, 3 )
#define INIT_GPIO3()    GPIO3 = 0, GPIO3_TRIS = INPUT_PIN

/** General-purpose digital I/O 4 pin control ************************/
#define GPIO4_PORT      C
#define GPIO4_BIT       4
#define GPIO4_MASK      ( 1 << GPIO4_BIT )
#define GPIO4_TRIS      TRIS( C, 4 )
#define GPIO4_DIR       GPIO4_TRIS
#define GPIO4           PORT( C, 4 )
#define GPIO4_ASM       PORT_ASM( C, 4 )
#define INIT_GPIO4()    GPIO4 = 0, GPIO4_TRIS = INPUT_PIN

/** LED **************************************************************/
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

/** Sense presence of USB bus ****************************************/
#if defined( USE_USB_BUS_SENSE_IO )
#define tris_usb_bus_sense  TRIS( C, 5 )
#define USB_BUS_SENSE       PORT( C, 5 )
#else
#define USB_BUS_SENSE       1
#endif

/** Self-powered flag ************************************************/
#define self_power      1

/** UART TX **********************************************************/
#define TX_PORT         B
#define TX_BIT          7
#define TX_MASK         ( 1 << TX_BIT )
#define TX_TRIS         TRIS( B, 7 )
#define TX_DIR          TX_TRIS
#define TX              LATCH( B, 7 )
#define TX_ASM          PORT_ASM( B, 7 )
#define INIT_TX()       TX = 0, TX_TRIS = OUTPUT_PIN

/** UART RX **********************************************************/
#define RX_PORT         B
#define RX_BIT          5
#define RX_MASK         ( 1 << RX_BIT )
#define RX_TRIS         TRIS( B, 5 )
#define RX_DIR          RX_TRIS
#define RX              PORT( B, 5 )
#define RX_ASM          PORT_ASM( B, 5 )
#define INIT_RX()       RX = 0, RX_TRIS = INPUT_PIN

/** SD CARD CHIP-SELECT **********************************************/
#define SD_CS_PORT      C
#define SD_CS_BIT       6
#define SD_CS_MASK      (1 << SD_CS_BIT)
#define SD_CS_TRIS      TRIS( C, 6 )
#define SD_CS_DIR       SD_CS_TRIS
#define SD_CS           LATCH( C, 6 )
#define SD_CS_ASM       PORT_ASM( C, 6 )
#define SD_CS_INIT()    SD_CS = 1, SD_CS_TRIS = OUTPUT_PIN

/** SD CARD CLK ******************************************************/
#define SD_CLK_PORT      B
#define SD_CLK_BIT       6
#define SD_CLK_MASK      (1 << SD_CLK_BIT)
#define SD_CLK_TRIS      TRIS( B, 6 )
#define SD_CLK_DIR       SD_CLK_TRIS
#define SD_CLK           LATCH( B, 6 )
#define SD_CLK_ASM       PORT_ASM( B, 6 )
#define SD_CLK_INIT()    SD_CLK = 0, SD_CLK_TRIS = OUTPUT_PIN

/** SD CARD MOSI *****************************************************/
#define SD_MOSI_PORT    C
#define SD_MOSI_BIT     7
#define SD_MOSI_MASK    (1 << SD_MOSI_BIT)
#define SD_MOSI_TRIS    TRIS( C, 7 )
#define SD_MOSI_DIR     SD_MOSI_TRIS
#define SD_MOSI         LATCH( C, 7 )
#define SD_MOSI_ASM     PORT_ASM( C, 7 )
#define SD_MOSI_INIT()  SD_MOSI = 0, SD_MOSI_TRIS = OUTPUT_PIN

/** SD CARD MISO *****************************************************/
#define SD_MISO_PORT    B
#define SD_MISO_BIT     4
#define SD_MISO_MASK    (1 << SD_MISO_BIT)
#define SD_MISO_TRIS    TRIS( B, 4 )
#define SD_MISO_DIR     SD_MISO_TRIS
#define SD_MISO         PORT( B, 4 )
#define SD_MISO_ASM     PORT_ASM( B, 4 )
#define SD_MISO_INIT()  SD_MISO = 1, SD_MISO_TRIS = INPUT_PIN

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
