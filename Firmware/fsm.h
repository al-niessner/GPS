
#ifndef GPS_FSM_H
#define GPS_FSM_H

#include "memory_types.h"

void fsm_initialize(void);
void fsm_process(void);
void fsm_request_state (fsm_state_t state);
void fsm_set_state (fsm_state_t state);

#endif
