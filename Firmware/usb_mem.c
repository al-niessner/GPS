
#include "main_version.h"
#include "usb_mem.h"

#define GPS_PRODUCT_ID 0x00,0x04

#pragma romdata
static const rom usb_device_info_t usb_dev_info =
  {
    PRODUCT_ID,
    GPS_MAJOR_VERSION, GPS_MINOR_VERSION, GPS_BUGFIX_VERSION,
    "DIY GPS on XuDL", // Description string.
    0x00               // Checksum (filled in later).
  };

#pragma udata
static USB_HANDLE usb_in_h[2] = {0,0};   // endpoint handles rcving packets
static USB_HANDLE usb_out_h[2]  = {0,0}; // endpoint handles sending packets

#pragma udata usb_data_ram
static usb_data_packet_t usb_in[2];  // buffers for rcving packets from host
static usb_data_packet_t usb_out[2]; // buffers for sending packets to host
