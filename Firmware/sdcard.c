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
 * read/write use null terminated strings. In both cases they are limited
 * to len if the terminator is not found before then. read returns the
 * actual length of the string not including the terminator.
 *
 *********************************************************************/

#include <delays.h>
#include <spi.h>

#include "HardwareProfile.h"
#include "sdcard.h"

typedef enum { SD_GO_IDLE_STATE=0x40, // CMD0   0x00
               SD_SEND_OP_COND=0x69,  // ACMD41 0x29
               SD_APP_CMD=0x77,       // CMD55  0x37
               SD_NULL=0xff           // with bit 7 set, command is invalid
               } sd_command_t;
typedef union
{
  unsigned char val;
  struct 
  {
    unsigned idle          :1;
    unsigned erase_reset   :1;
    unsigned illegal_cmd   :1;
    unsigned cmd_crc_err   :1;
    unsigned erase_seq_err :1;
    unsigned addr_err      :1;
    unsigned param_err     :1;
    unsigned valid         :1;
  };
} sd_R1_t;

#pragma udata
static bool_t sdcard_is_init;

#pragma code

sd_R1_t sdcard_comm (sd_command_t c, unsigned long int arg)
{
  unsigned char i;
  sd_R1_t result;

  SD_CS = 0; // select the SD card
  putcSPI (SD_NULL); // put some time on the bus while we wait for CS to clk
  putcSPI (c);
  putcSPI ((arg >> 24) & 0xff);
  putcSPI ((arg >> 16) & 0xff);
  putcSPI ((arg >>  8) & 0xff);
  putcSPI ((arg)       & 0xff);
  putcSPI (0x95); // only used with the init and this correct
  for (i = 0xff ; i && !DataRdySPI() ; i--) Delay1TCY();
  result.val = getcSPI();

  return result;
}

void sdcard_erase(void)
{
}

bool_t sdcard_initialize(void)
{
  sd_R1_t reply;
  unsigned char i;

  reply.val = 0x80;
  sdcard_is_init = SD1;
  SD_CS_INIT();
  SD_CLK_INIT();
  SD_MOSI_INIT();
  SD_MISO_INIT();
  OpenSPI (SPI_FOSC_64, MODE_00, SMPMID); // initial contact must be < 400 KHz  
  
  // CS and MOSI (DI) must be high for 74 sclks to enter native command mode
  SD_CS = 1;
  SD_MOSI = 1;
  Delay100TCYx (48);

  // put the sdcard into idle SPI mode
  reply = sdcard_comm (SD_GO_IDLE_STATE, 0u);

  if (reply.idle == 1u && reply.valid == 0u)
    {
      sdcard_is_init = SD2;
      OpenSPI (SPI_FOSC_4, MODE_00, SMPMID); // speed up now that we are talking
      Delay10TCYx (2); // give the hardware some time for the clock change
      reply = sdcard_comm (SD_APP_CMD, 0u);

      if (reply.valid == 0u) // will get set to 1 on timeout?
        {
          sdcard_is_init = SD3;
          reply = sdcard_comm (SD_SEND_OP_COND, 0u);
          sdcard_is_init = reply.val == 0u;
        }
    }
  
  return sdcard_is_init;
}

unsigned char sdcard_read (unsigned char *s, unsigned char len)
{
  unsigned char result = 0u;
  return result;
}

void sdcard_write (unsigned char *s, unsigned char len)
{
}
