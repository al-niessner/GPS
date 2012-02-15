
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
