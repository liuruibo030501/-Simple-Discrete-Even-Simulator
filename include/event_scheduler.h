#ifndef EVENT_SCHEDULER_H
#define EVENT_SCHEDULER_H

#include <stddef.h>
#include "event.h"

//min-heap
typedef struct {
    Event **heap;     // heap of Event* 
    size_t size;     
    size_t capacity;  
} EventScheduler;

// creater
EventScheduler *event_scheduler_create(size_t capacity);
void event_scheduler_destroy(EventScheduler *scheduler);

// push and pop
int event_scheduler_push(EventScheduler *scheduler, Event *ev);
Event *event_scheduler_pop(EventScheduler *scheduler);

// printer
void event_scheduler_print(EventScheduler *scheduler);

#endif // EVENT_SCHEDULER_H
