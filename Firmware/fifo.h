
#ifndef GPS_FIFO_H
#define GPS_FIFO_H

void   fifo_initialize(void);

bool_t fifo_fetch_time_event(void);
void   fifo_push_time_event(void);

void   fifo_fetch_usb(void);
void   fifo_push_usb(void);

char   fifo_fetch_next(void);
bool_t fifo_is_receiving(void);
void   fifo_push_message (char *s);
void   fifo_push_serial (char c);
void   fifo_set_valid (bool_t b);

#endif
