#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>

//the type of events further use switch to handle different events
typedef enum {
    EVENT_PACKET_RECEIVED,
    EVENT_TIMEOUT,
    EVENT_SEND_PACKET,
    EVENT_CUSTOM//can add more
} EventType;

// Forward declaration - actual definition in packet.h
struct Packet;

typedef void (*EventTask)(void *context);

typedef struct Event {
    uint64_t time;          
    EventType type;          
    EventTask task;          
    void *context;          
    struct Packet *packet;   
} Event;

//creater
Event *event_create(uint64_t time,
                    EventType type,
                    EventTask task,
                    void *context,
                    struct Packet *packet);

void event_destroy(Event *ev);


#endif // EVENT_H
