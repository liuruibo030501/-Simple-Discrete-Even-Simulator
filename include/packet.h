#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>

typedef struct Packet {
    int id;    // Unique packet identifier
    uint64_t creation_time;    // Timestamp when the packet was created
    uint64_t size;    // Size of the packet in bytes
    // char* payload; // Optional future extension
} Packet;

// Constructor / destructor
Packet *packet_create(int id, uint64_t creation_time, uint64_t size);    // create and initialize a new packet
void packet_destroy(Packet *pkt);    // destroy a packet and free memory

// Accessors (needed by network_sim.c)
// these functions are to avoid the warning of implicit declaration (putain C99)
int packet_get_id(const Packet *pkt);
uint64_t packet_get_creation_time(const Packet *pkt);
uint64_t packet_get_size(const Packet *pkt);
void packet_set_size(Packet *pkt, uint64_t new_size); // optional mutator

#endif
