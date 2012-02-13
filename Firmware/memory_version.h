
#ifndef GPS_MEMORY_VERSION
#define GPS_MEMORY_VERSION

#pragma romdata
/*
 * major  : architectural and design choices
 * minor  : implementation choices
 * bugfix : changes in code to match design and implementation choices
 */
unsigned byte gps_major  = 1
unsigned byte gps_minor  = 0
unsigned byte gps_bugfix = 0

#define GPS_PRODUCT_ID 0x00,0x04

#endif
