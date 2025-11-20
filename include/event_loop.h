#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <stdint.h>
#include "event_scheduler.h"
#include "event.h"

// Global simulation time
extern uint64_t current_sim_time;

// Run the event loop until no events remain
void event_loop_run(EventScheduler *scheduler);

#endif // EVENT_LOOP_H
