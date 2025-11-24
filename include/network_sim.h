#ifndef NETWORK_SIM_H
#define NETWORK_SIM_H

#include <stdint.h>
#include "event_scheduler.h"
#include "packet.h"

typedef enum {
    SENDER_FIXED_INTERVAL,    // Fixed interval sending
    SENDER_EXPONENTIAL    // Exponentially distributed sending
} SenderMode;

typedef struct SenderContext {    // Context for the sender
    int packets_sent;    // Total packets sent
    uint64_t interval;    // Interval for fixed sending
    uint64_t finish_time;    // Time to stop sending (simulation end time)
    SenderMode mode;    // Sending mode (fixed or exponential)
    double lambda;    // Lambda for exponential mode, i.e. arrival rate
    struct NetworkContext *network;    // Reference to network context
    uint64_t packet_size;          // size of each generated packet
    uint64_t total_bytes_sent;     // accumulated bytes sent
} SenderContext;

typedef struct NetworkContext {
    int packets_forwarded;
    uint64_t min_delay;    // Minimum network delay
    uint64_t max_delay;    // Maximum network delay
    struct ReceiverContext *receiver;    // Reference to receiver context
    uint64_t total_bytes_forwarded; // accumulated bytes forwarded
} NetworkContext;

typedef struct ReceiverContext {
    int packets_received;
    uint64_t last_receive_time;    // Timestamp of last received packet
    uint64_t time_between_packets;    // Time between last two packets
    int first_packet;    // Flag to indicate if it's the first packet received
    uint64_t total_bytes_received;  // accumulated bytes received
    uint64_t total_latency;         // sum of (receive_time - creation_time)
    uint64_t last_latency;          // latency of last packet
} ReceiverContext;

extern EventScheduler *global_scheduler;    // Global event scheduler. Defined in main.c, used for scheduling events across tasks

/*
*    these functions are for creating, destroying, and handling tasks for sender, network, and receiver components
*/
SenderContext *sender_create(uint64_t interval, uint64_t finish_time, SenderMode mode, double lambda, uint64_t packet_size);
void sender_destroy(SenderContext *sender);
void sender_task(void *context);
void sender_print_stats(SenderContext *sender);

NetworkContext *network_create(uint64_t min_delay, uint64_t max_delay);
void network_destroy(NetworkContext *network);
void network_task(void *context);
void network_print_stats(NetworkContext *network);

ReceiverContext *receiver_create(void);
void receiver_destroy(ReceiverContext *receiver);
void receiver_task(void *context);
void receiver_print_stats(ReceiverContext *receiver);

void run_network_simulation(uint64_t sender_interval, uint64_t finish_time, SenderMode mode, double lambda, uint64_t net_min_delay, uint64_t net_max_delay, uint64_t packet_size);
// the main inlet function to run the network simulation with specified parameters

#endif

