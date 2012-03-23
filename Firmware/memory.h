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

#ifndef GPS_MEMORY_H
#define GPS_MEMORY_H

typedef enum { false=0==1, true=0==0 } bool_t;

/**
  * FSM Types
  *
  * fsm_state_t states S0..S9 inclusive are defined in the design documenation.
  * The states UNDEFINED and INDETERMINATE are implementation additions to help
  * detect errors in the FSM engine. UNDEFINED is used to mark a variable as
  * not being set. This is helpful for when the user sends or does not send a
  * state request or demand. INDETERMINATE means that the engine made a mistake
  * somewhere along the way and the FSM is now in a bad state.
 **/

typedef enum { S0=0, S1, S2, S3, S4, S5, S6, S7, S8, S9,
               UNDEFINED=0xfe, INDETERMINATE=0xff } fsm_state_t;

typedef struct fsm_shared_block
{
  fsm_state_t current;
  fsm_state_t next;
  fsm_state_t requested;
  fsm_state_t required;
} fsm_shared_block_t;


/**
  * SD Card interface
 **/

typedef enum { SD_POWER_UP=0x00, // device is turned on
               SD_SPI_MODE=0x01, // attempted to enter SPI mode
               SD_VOLT_CHK=0x02, // sent a SEND_IF_COND check volts
               SD_VOLT_R00=0x03, // checked reply R1 if reported rev 2
               SD_VOLT_R03=0x04, // checked reply V if reported rev 2
               SD_VOLT_R04=0x05, // checked reply echo if reported rev 2
               SD_CLK_RATE=0x06, // pick up the speed
               SD_INIT_SCD=0x07, // sent APP_SEND_OP_COND to init HW
               SD_WAIT_SCD=0x09, // waited for the card to finish
               SD_OCR_READ=0x08, // read the OCR
               SD_CID_READ=0x0a, // read the CID
               SD_CSD_READ=0x0b, // read the CSD
               SD_INIT_DONE_SDSC=0x80,
               SD_INIT_DONE_SDSH_X=0xc0
} sdcard_init_step_t;

typedef struct sdcard_shared_block
{
  unsigned char cid[16];
  unsigned char csd[16];
  unsigned long int read_page;
  unsigned long int total_pages;
  unsigned long int write_page;
} sdcard_shared_block_t;

/**
  * Time Event
**/

typedef enum { FALLING_EDGE, RISING_EDGE, SS_HIGH, SS_LOW } button_event_t;

/**
  * USB Types
 **/

#include "usb_config.h"

typedef enum
{
  READ_EEDATA_CMD    = 0x04,  // Read from the device EEPROM.
  WRITE_EEDATA_CMD   = 0x05,  // Write to the device EEPROM.

  GPS_VER_CMD        = 0x80,  // Get the version of the GPS firmware
  GPS_REQUEST_CMD    = 0x81,  // Set the state
  GPS_STATE_REQ      = 0x82,  // Get the current and next state and timing
  GPS_SDC_CONFIG_REQ = 0x83,  // Get the CDS and CIS from the SD Card
  GPS_SDC_STATE_REQ = 0x84,  // Get the status, last read, and last written

  RESET_CMD          = 0xff   // Cause a power-on reset.
} usb_cmd_t;

typedef struct usb_device_info
{
  unsigned char product_id[2];
  unsigned char version_id[3];
  unsigned char desc[USBGEN_EP_SIZE - 7]; // 7 is number of bytes in union below
  unsigned char checksum;
} usb_device_info_t;

typedef union usb_data_packet
{
  unsigned char      _byte[USBGEN_EP_SIZE];
  unsigned short int _word[USBGEN_EP_SIZE / 2];

  struct // for version information
  {
    usb_cmd_t         cmd;
    usb_device_info_t info;
  };

  struct // GPS_REQUEST_CMD
  {
    usb_cmd_t cmd;
    unsigned char new_state;
    bool_t force;
    unsigned int duration;
    unsigned char data[USBGEN_EP_SIZE - 5];
  };

  struct // GPS_STATE_REQ
  {
    usb_cmd_t cmd;
    struct
    {
      unsigned my_true         : 1;
      unsigned xfer_is_reading : 1;
      unsigned xfer_crc_match  : 1;
      unsigned unused_bits     : 5;
    } bits;
    fsm_state_t current;
    fsm_state_t next;
    fsm_state_t requested;
    fsm_state_t required;
    unsigned long int timing;
    sdcard_init_step_t sdcard_init;
    unsigned char last_r1;
    unsigned char sdcard_version;
    unsigned char xfer_r1;
    unsigned char unused_req[USBGEN_EP_SIZE - 14];
  };

  struct // GPS_SDC_CONFIG_REQ
  {
    unsigned int  sdc_status;
    unsigned char cid[15];
    unsigned char csd[15];
  };

  struct // GPS_SDC_STATE_REQ
  {
    usb_cmd_t cmd;
    unsigned long int next_page_to_read;
    unsigned long int next_page_to_write;
    unsigned long int total_pages;
    unsigned char unused[USBGEN_EP_SIZE - 13];
  };

  struct // EEPROM read/write structure
  {
    usb_cmd_t     cmd;
    unsigned char len;
    union
    {
      rom far char *pAdr; // Address pointer
      struct
      {
        unsigned char low;   // Little-endian order
        unsigned char high;
        unsigned char upper;
      };
    } ADR;
    unsigned char data[USBGEN_EP_SIZE - 5];
  };
} usb_data_packet_t;

typedef struct usb_shared_block
{
  usb_data_packet_t inbound;
  usb_data_packet_t outbound;
} usb_shared_block_t;

#endif
