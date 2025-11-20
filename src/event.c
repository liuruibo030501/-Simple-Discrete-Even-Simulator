#include <stdlib.h>
#include "event.h"

Event *event_create(uint64_t time, EventType type, EventTask task, void *context, struct Packet *packet){

    Event *ev = malloc(sizeof(Event));
    if (!ev)
        return NULL;

    ev->time = time;
    ev->type = type;
    ev->task = task;
    ev->context = context;
    ev->packet = packet;

    return ev;
}


void event_destroy(Event *ev){
    if (!ev)
        return;
    free(ev);
}
