#include <stdlib.h>
#include "packet.h"

// I think these functions are easy to read. C'est facile XD
Packet *packet_create(int id, uint64_t creation_time, uint64_t size) {
    Packet *pkt = malloc(sizeof(Packet));
    if (!pkt)
        return NULL;
    pkt->id = id;
    pkt->creation_time = creation_time;
    pkt->size = size;
    return pkt;
}

void packet_destroy(Packet *pkt) {
    if (pkt)
        free(pkt);
}

// yeah so as the header said
int packet_get_id(const Packet *pkt){ return pkt ? pkt->id : -1; }
uint64_t packet_get_creation_time(const Packet *pkt){ return pkt ? pkt->creation_time : 0; }
uint64_t packet_get_size(const Packet *pkt){ return pkt ? pkt->size : 0; }
void packet_set_size(Packet *pkt, uint64_t new_size){ if (pkt) pkt->size = new_size; }
