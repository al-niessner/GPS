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
 * Time Event
**/

typedef enum { FALLING_EDGE, RISING_EDGE, SS_HIGH, SS_LOW } button_event_t;
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


/**
  * USB Types
 **/

#include "usb_config.h"

typedef enum
{
  READ_EEDATA_CMD   = 0x04,  // Read from the device EEPROM.
  WRITE_EEDATA_CMD  = 0x05,  // Write to the device EEPROM.

  GPS_VER_CMD       = 0x80,  // Get the version of the GPS firmware
  GPS_REQUEST_CMD   = 0x81,  // Set the state
  GPS_STATE_REQ     = 0x82,  // Get the current and next state and timing

  RESET_CMD         = 0xff   // Cause a power-on reset.
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
    fsm_state_t current;
    fsm_state_t next;
    fsm_state_t requested;
    fsm_state_t required;
    unsigned long int timing;
    unsigned char data[USBGEN_EP_SIZE - 9];
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

typedef struct user_request
{
  usb_cmd_t command;
  union
  {
    struct
    {
      bool_t force;
      fsm_state_t state;
      unsigned int duration;
    };
  } details;
} user_request_t;

#endif
