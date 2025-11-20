#include "event_loop.h"
#include <stdio.h>

uint64_t current_sim_time = 0;

void event_loop_run(EventScheduler *scheduler){
    if (!scheduler)
        return;

    while (scheduler->size > 0) {

        // Pop the next event with the smallest timestamp
        Event *ev = event_scheduler_pop(scheduler);
        if (!ev)
            break;

        //update
        current_sim_time = ev->time;
        if (ev->task) {
            ev->task(ev->context);
        }

        event_destroy(ev);
    }
}
