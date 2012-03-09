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

#include "fifo.h"
#include "HardwareProfile.h"
#include "sdcard.h"

typedef enum { SD_GO_IDLE_STATE    = 0x40,// CMD0   0x00
               SD_SEND_IF_COND     = 0x48,// CMD8
               SD_SEND_CSD         = 0x49,// CMD9
               SD_SEND_CID         = 0x4a,// CMD10
               SD_SEND_STATUS      = 0x4d,// CMD13
               SD_SET_BLOCKLEN     = 0x50,// CMD16
               SD_READ_BLOCK       = 0x51,// CMD17
               SD_APP_CMD          = 0x77,// CMD55
               SD_READ_OCR         = 0x7a,// CMD58
               SD_APP_SEND_OP_COND = 0x69,// ACMD41 0x29
               SD_NULL             = 0xff // with bit 7 set, command is invalid
               } sd_command_t;
typedef enum { R1, R1b, R2, R3, R7 } sd_response_name_t;

typedef struct
{
  unsigned idle          :1;
  unsigned erase_reset   :1;
  unsigned illegal_cmd   :1;
  unsigned cmd_crc_err   :1;
  unsigned erase_seq_err :1;
  unsigned addr_err      :1;
  unsigned param_err     :1;
  unsigned valid         :1;
} r1_t;

typedef union
{
  unsigned char val[5];
  struct // R1 and R1b
  {
    r1_t r1;
    unsigned unused_r1[4];
  };

  struct // R2
  {
    unsigned int r2_val;
    unsigned unused_r2[3];
  };

  struct // R3
  {
    r1_t r1;
    unsigned long int ocr;
  };

  struct // R7
  {
    r1_t r1;
    struct
    {
      unsigned resv : 4;
      unsigned cver : 4;
    } high;
    struct
    {
      unsigned volt : 4;
      unsigned resv : 4;
    } low;
    unsigned char echo;
  };
} sd_response_t;


#pragma udata

static sd_response_t reply;
static unsigned char version;
static unsigned char cid[16];
static unsigned char csd[16];

#pragma code

unsigned char sdcard_crc7 (unsigned char data, unsigned char prev)
{
  unsigned char crc, i;

  crc = prev;
  for (i = 0 ; i < 8u ; i++)
    {
      crc = crc << 1;
      if ((data ^ crc) & 0x80) crc = crc ^ 0x09;
      data = data << 1;

    }

  return crc;
}

unsigned int sdcard_crc16 (unsigned char data, unsigned int crc) 
{ 
  unsigned int x;

  x = (crc >> 8) & 0x00ff;
  x = x ^ data;
  x = x ^ (x >> 4); 
  crc = (crc << 8) ^ ((x << 8) << 4) ^ ((x << 4) << 1) ^ x; 

  return crc; 
}

// length is determined from command.
void sdcard_block (sd_command_t c, unsigned char *block)
{
}

void sdcard_process (sd_command_t c,
                     unsigned long int arg,
                     sd_response_name_t r)
{
  unsigned char crc,data,i,j,n;

  SD_CS = 0;
  putcSPI (c);
  crc = sdcard_crc7 (c, 0);
  data = (arg >> 24) & 0xff;
  putcSPI (data);
  crc = sdcard_crc7 (data, crc);
  data = (arg >> 16) & 0xff;
  putcSPI (data);
  crc = sdcard_crc7 (data, crc);
  data = (arg >> 8) & 0xff;
  putcSPI (data);
  crc = sdcard_crc7 (data, crc);
  data =  arg & 0xff;
  putcSPI (data);
  crc = sdcard_crc7 (data, crc);
  crc = (crc << 1) | 1;
  putcSPI (crc);
  for (i = 0 ; i < 5u ;  i++) reply.val[i] = SD_NULL;
  switch (r)
    {
    case R1:
    case R1b:
      n = 1;
      break;

    case R2:
      n = 2;
      break;

    case R3:
    case R7:
      n = 5;
      break;
    }
  for (i = 0x09 ; i && reply.val[0] == SD_NULL ; i--) //Ncr delay
    {
      putcSPI (SD_NULL); // keep sclk moving
      reply.val[0] = getcSPI();
    }
  for (j = 1 ; j < n ; j++)
    {
      reply.val[j] = getcSPI();
    }

  if (r == R1b)
    {
      do { putcSPI (SD_NULL); } // keep sclk moving
      while (getcSPI() == 0x00u);
    }

  SD_CS = 1;
}

void sdcard_erase(void)
{
}

unsigned char *sdcard_get_CID(void) { return cid; }
unsigned char *sdcard_get_CSD(void) { return csd; }
unsigned int sdcard_get_status(void)
{
  sdcard_process (SD_SEND_STATUS, 0x0u, R2);
  return reply.r2_val;
}

void sdcard_initialize(void)
{
  bool_t sdsc;
  unsigned char i;
  unsigned long int acmd41_arg;

  sdsc = true;
  acmd41_arg = 0x00u;
  version = 0x00u;
  reply.val[0] = 0x80;
  SD_CS_INIT();
  Delay1KTCYx (0); // make sure power has been on for 1 ms
  OpenSPI (SPI_FOSC_64, MODE_00, SMPEND); // initial contact must be < 400 KHz

  // CS and MOSI (DI) must be high for 74 sclks to enter native command mode
  SD_CS = 1;
  for (i = 0x0b ; i ; i--) putcSPI (SD_NULL); // keep MOSI high and sclk moving
  fifo_broadcast_sdcard_usb (SD_POWER_UP, 0x00u, version);

  // put the sdcard into idle SPI mode
  sdcard_process (SD_GO_IDLE_STATE, 0u, R1);
  fifo_broadcast_sdcard_usb (SD_SPI_MODE, reply.val[0], reply.val[1]);

  if (reply.val[0] == 0x01u)
    {
      sdcard_process (SD_SEND_IF_COND, 0x000001aau, R7);
      fifo_broadcast_sdcard_usb (SD_VOLT_CHK, reply.val[0], version);

      if (reply.r1.illegal_cmd && (reply.val[0] & 0xfa) == 0x00u) version = 1;
      else if (reply.val[0] == 0x01u)
        {
          acmd41_arg = 0x40000000;
          version = 2;
        }
      else return;

      if (version == 2u)
        {
          fifo_broadcast_sdcard_usb (SD_VOLT_R00, reply.val[0], version);
          if (reply.val[0] != 0x01u) return;

          fifo_broadcast_sdcard_usb (SD_VOLT_R03, reply.val[3], reply.val[2]);
          if ((reply.val[3] & 0x0f) != 0x01u) return;
          
          fifo_broadcast_sdcard_usb (SD_VOLT_R04, reply.val[4], version);
          if (reply.val[4] != 0xaau) return;
        }

      OpenSPI (SPI_FOSC_4, MODE_00, SMPMID); // speed up now that we are talking
      Delay10TCYx (12); // give the hardware some time for the clock change
      fifo_broadcast_sdcard_usb (SD_CLK_RATE, 0x00u, version);

      // tell the sdcard to initialize
      sdcard_process (SD_APP_CMD, 0x00u, R1);
      sdcard_process (SD_APP_SEND_OP_COND, acmd41_arg, R1);
      fifo_broadcast_sdcard_usb (SD_INIT_SCD, reply.val[0], version);

      if ((reply.val[0] & 0xfe) != 0x00u) return;

      while (reply.val[0] == 0x01u)
        {
          sdcard_process (SD_APP_CMD, 0x00u, R1);
          sdcard_process (SD_APP_SEND_OP_COND, acmd41_arg, R1);
        }
      fifo_broadcast_sdcard_usb (SD_WAIT_SCD, reply.val[0], version);

      if (reply.val[0] != 0x00u) return;

      if (version == 2u)
        {
          sdcard_process (SD_READ_OCR, 0x00u, R3);
          fifo_broadcast_sdcard_usb (SD_OCR_READ, reply.val[0], version);
          sdsc = 0u == (reply.val[1] & 0x40);

          if (reply.val[0] != 0x00u) return;
        }

      // make all cards compatible since SDCH/X cards are fixed at 512 anyway
      sdcard_process (SD_SET_BLOCKLEN, 0x200, R1);
      fifo_broadcast_sdcard_usb (sdsc ? SD_INIT_DONE_SDSC : SD_INIT_DONE_SDSH_X,
                                 reply.val[0], version);
    }
}

unsigned char sdcard_read (unsigned char *s, unsigned char len)
{
  unsigned char result = 0u;
  return result;
}

void sdcard_write (unsigned char *s, unsigned char len)
{
}
