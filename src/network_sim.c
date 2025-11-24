#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "network_sim.h"
#include "event.h"
#include "event_scheduler.h"
#include "event_loop.h"
#include "packet.h"

EventScheduler *global_scheduler = NULL;

static double random_double(void) {    // 这是个轮子, used to generate a random double between 0 and 1
    return (double)rand() / RAND_MAX;
}

static uint64_t random_exponential(double lambda) {    // 另一个轮子, used to generate an exponentially distributed random variable
    double u = random_double();
    return (uint64_t)(-log(1.0 - u) / lambda);
}

static uint64_t random_delay(uint64_t min, uint64_t max) {    // 第三个轮子, used to generate a random delay between min and max
    if (min >= max)
        return min;
    return min + (rand() % (max - min + 1));
}

// ez create
SenderContext *sender_create(uint64_t interval, uint64_t finish_time, SenderMode mode, double lambda, uint64_t packet_size) {
    SenderContext *sender = malloc(sizeof(SenderContext));
    if (!sender)
        return NULL;
    sender->packets_sent = 0;
    sender->interval = interval;
    sender->finish_time = finish_time;
    sender->mode = mode;
    sender->lambda = lambda;
    sender->network = NULL;
    sender->packet_size = packet_size;
    sender->total_bytes_sent = 0;
    return sender;
}

// ez destroy
void sender_destroy(SenderContext *sender) {
    if (sender)
        free(sender);
}

/* this is kind of complex...
* the sender_task is responsible for sending packets at specified intervals or based on an exponential distribution.
* 1. Check if the current simulation time has reached the finish time. If so, it stops sending packets.
* 2. If not, it sends a packet, prints the diary, adds the packet_sent count.
* 3. Immediately (current_sim_time + 1) schedules a network event to simulate packet arrival at the network.
* 4. Depending on the mode (fixed interval or exponential), it calculates the next sending time:
*    - For fixed mode, uses interval.
*    - For exponential mode, uses random_exponential to get the next interval (>=1).
* 5. If the next sending time is before the finish time, it schedules the next sender_task event. This creates a loop where the sender continues to send packets until the finish time is reached.
*/
void sender_task(void *context) {
    SenderContext *sender = (SenderContext *)context;
    if (current_sim_time >= sender->finish_time) {
        printf("[Sender] Stopped at time %llu\n", (unsigned long long)current_sim_time);
        return;
    }

    // create a packet object
    Packet *pkt = packet_create(sender->packets_sent + 1, current_sim_time, sender->packet_size);
    if (!pkt) {
        fprintf(stderr, "[Sender] Packet allocation failed\n");
        return;
    }

    printf("[Sender] Sent packet #%d at time %llu size=%llu bytes\n",
           sender->packets_sent + 1, (unsigned long long)current_sim_time, (unsigned long long)packet_get_size(pkt));
    sender->packets_sent++;
    sender->total_bytes_sent += packet_get_size(pkt);

    Event *network_event = event_create(current_sim_time + 1, EVENT_PACKET_RECEIVED,
                                       network_task, sender->network, pkt);
    if (event_scheduler_push(global_scheduler, network_event) != 0) {
        fprintf(stderr, "[Sender] Failed to schedule network event!\n");
        packet_destroy(pkt);
        event_destroy(network_event);
        return;
    }

    uint64_t next_interval;
    if (sender->mode == SENDER_FIXED_INTERVAL) {
        next_interval = sender->interval;
    } else {
        next_interval = random_exponential(sender->lambda);
        if (next_interval < 1)
            next_interval = 1;
    }

    uint64_t next_time = current_sim_time + next_interval;
    if (next_time < sender->finish_time) {
        Event *next_send = event_create(next_time, EVENT_SEND_PACKET, sender_task, sender, NULL);
        if (event_scheduler_push(global_scheduler, next_send) != 0) {
            fprintf(stderr, "[Sender] Failed to schedule next send!\n");
            event_destroy(next_send);
        }
    }
}

// ez print
void sender_print_stats(SenderContext *sender) {
    printf("\n=== SENDER STATISTICS ===\n");
    printf("  Packets sent: %d\n", sender->packets_sent);
    printf("  Total bytes sent: %llu\n", (unsigned long long)sender->total_bytes_sent);
    printf("  Mode: %s\n", sender->mode == SENDER_FIXED_INTERVAL ? "Fixed interval" : "Exponential");
    if (sender->mode == SENDER_FIXED_INTERVAL) {
        printf("  Interval: %llu\n", (unsigned long long)sender->interval);
    } else {
        printf("  Lambda: %.6f\n", sender->lambda);
    }
    printf("  Packet size: %llu\n", (unsigned long long)sender->packet_size);
}

NetworkContext *network_create(uint64_t min_delay, uint64_t max_delay) {
    NetworkContext *network = malloc(sizeof(NetworkContext));
    if (!network)
        return NULL;
    network->packets_forwarded = 0;
    network->min_delay = min_delay;
    network->max_delay = max_delay;
    network->receiver = NULL;
    network->total_bytes_forwarded = 0;
    return network;
}

void network_destroy(NetworkContext *network) {
    if (network)
        free(network);
}

/* So, the second task...
* 1. Prints the timestamp when a packet is received.
* 2. Increments the packets_forwarded count.
* 3. Generates a random delay within the specified min and max range.
* 4. Schedules a receiver event during (current_sim_time + delay) to simulate delayed packet arrival at the receiver.
*/
void network_task(void *context) {
    NetworkContext *network = (NetworkContext *)context;
    Packet *pkt = current_event ? current_event->packet : NULL;
    printf("[Network] Received packet at time %llu\n", (unsigned long long)current_sim_time);

    if (pkt) {
        network->packets_forwarded++;
        network->total_bytes_forwarded += packet_get_size(pkt);
    }

    uint64_t delay = random_delay(network->min_delay, network->max_delay);
    printf("[Network] Forwarding with delay %llu\n", (unsigned long long)delay);

    // Forward to receiver keeping same packet pointer
    Event *receiver_event = event_create(current_sim_time + delay, EVENT_PACKET_RECEIVED,
                                        receiver_task, network->receiver, pkt);
    if (event_scheduler_push(global_scheduler, receiver_event) != 0) {
        fprintf(stderr, "[Network] Failed to schedule receiver event!\n");
        // drop packet (this is very 细节)
        if (pkt) packet_destroy(pkt);
        event_destroy(receiver_event);
    }
}

void network_print_stats(NetworkContext *network) {
    printf("\n=== NETWORK STATISTICS ===\n");
    printf("  Packets forwarded: %d\n", network->packets_forwarded);
    printf("  Total bytes forwarded: %llu\n", (unsigned long long)network->total_bytes_forwarded);
    printf("  Delay range: %llu - %llu\n",
           (unsigned long long)network->min_delay, (unsigned long long)network->max_delay);
}

ReceiverContext *receiver_create(void) {
    ReceiverContext *receiver = malloc(sizeof(ReceiverContext));
    if (!receiver)
        return NULL;
    receiver->packets_received = 0;
    receiver->last_receive_time = 0;
    receiver->time_between_packets = 0;
    receiver->first_packet = 1;
    receiver->total_bytes_received = 0;
    receiver->total_latency = 0;
    receiver->last_latency = 0;
    return receiver;
}

void receiver_destroy(ReceiverContext *receiver) {
    if (receiver)
        free(receiver);
}

/* finally, the receiver task...
* 1. Increase the packets_received count.
* 2. If it's not the first packet, calculates the time gap since the last received packet and prints it.
* 3. If it's the first packet, it just notes that and sets the first_packet flag to false.
* 4. Updates the last_receive_time to the current simulation time.
*/
void receiver_task(void *context) {
    ReceiverContext *receiver = (ReceiverContext *)context;
    Packet *pkt = current_event ? current_event->packet : NULL;
    receiver->packets_received++;

    if (pkt) {
        receiver->total_bytes_received += packet_get_size(pkt);
        uint64_t latency = current_sim_time - packet_get_creation_time(pkt);
        receiver->total_latency += latency;
        receiver->last_latency = latency;
    }

    if (!receiver->first_packet) {
        receiver->time_between_packets = current_sim_time - receiver->last_receive_time;
        printf("[Receiver] Received packet #%d at time %llu (gap: %llu)",
               receiver->packets_received, (unsigned long long)current_sim_time,
               (unsigned long long)receiver->time_between_packets);
        if (pkt) {
            printf(" latency=%llu size=%llu\n", (unsigned long long)receiver->last_latency, (unsigned long long)packet_get_size(pkt));
        } else {
            printf(" (no packet object)\n");
        }
    } else {
        printf("[Receiver] Received first packet at time %llu", (unsigned long long)current_sim_time);
        if (pkt) {
            printf(" latency=%llu size=%llu\n", (unsigned long long)receiver->last_latency, (unsigned long long)packet_get_size(pkt));
        } else {
            printf(" (no packet object)\n");
        }
        receiver->first_packet = 0;
    }
    receiver->last_receive_time = current_sim_time;

    // Destroy packet after final consumption
    if (pkt) {
        packet_destroy(pkt);
        current_event->packet = NULL; // prevent double free
    }
}

void receiver_print_stats(ReceiverContext *receiver) {
    printf("\n=== RECEIVER STATISTICS ===\n");
    printf("  Packets received: %d\n", receiver->packets_received);
    if (receiver->packets_received > 0) {
        printf("  Last receive time: %llu\n", (unsigned long long)receiver->last_receive_time);
        printf("  Total bytes received: %llu\n", (unsigned long long)receiver->total_bytes_received);
        printf("  Last latency: %llu\n", (unsigned long long)receiver->last_latency);
        uint64_t avg_latency = receiver->packets_received ? receiver->total_latency / receiver->packets_received : 0;
        printf("  Average latency: %llu\n", (unsigned long long)avg_latency);
    }
    if (receiver->packets_received > 1) {
        printf("  Last inter-packet gap: %llu\n", (unsigned long long)receiver->time_between_packets);
    }
}

/* now is the time to run the whole simulation...
* 1. Initializes the random number generator and creates the global event scheduler.
* 2. Creates the global scheduler, and receiver/network/sender.
* 3. Links the components together (network to receiver, sender to network).
* 4. Prints the simulation configuration.
* 5. Schedules the initial sender event to kick off the simulation.
* 6. Runs the event loop until all events are processed.
* 7. After completion, prints final statistics for each component and overall packet loss/delivery rates.
* 8. Cleans up all allocated resources (contexts and scheduler).
*/
void run_network_simulation(uint64_t sender_interval, uint64_t finish_time, SenderMode mode,
                           double lambda, uint64_t net_min_delay, uint64_t net_max_delay, uint64_t packet_size) {
    srand(time(NULL));

    global_scheduler = event_scheduler_create(10000);
    if (!global_scheduler) {
        fprintf(stderr, "Failed to create scheduler!\n");
        return;
    }

    ReceiverContext *receiver = receiver_create();
    NetworkContext *network = network_create(net_min_delay, net_max_delay);
    SenderContext *sender = sender_create(sender_interval, finish_time, mode, lambda, packet_size);

    if (!receiver || !network || !sender) {
        fprintf(stderr, "Failed to create components!\n");
        goto cleanup;
    }

    network->receiver = receiver;
    sender->network = network;

    printf("\n========================================\n");
    printf("  NETWORK SIMULATION STARTING\n");
    printf("========================================\n");
    printf("Config:\n");
    printf("  Sender interval: %llu\n", (unsigned long long)sender_interval);
    printf("  Finish time: %llu\n", (unsigned long long)finish_time);
    printf("  Network delay: %llu - %llu\n",
           (unsigned long long)net_min_delay, (unsigned long long)net_max_delay);
    printf("  Mode: %s\n", mode == SENDER_FIXED_INTERVAL ? "Fixed" : "Exponential");
    printf("========================================\n\n");

    Event *initial_send = event_create(0, EVENT_SEND_PACKET, sender_task, sender, NULL);
    if (event_scheduler_push(global_scheduler, initial_send) != 0) {
        fprintf(stderr, "Failed to schedule initial event!\n");
        event_destroy(initial_send);
        goto cleanup;
    }

    event_loop_run(global_scheduler);

    printf("\n========================================\n");
    printf("  SIMULATION COMPLETED\n");
    printf("  Final time: %llu\n", (unsigned long long)current_sim_time);
    printf("========================================\n");

    sender_print_stats(sender);
    network_print_stats(network);
    receiver_print_stats(receiver);

    printf("\n=== OVERALL STATISTICS ===\n");
    if (sender->packets_sent > 0) {
        double loss_rate = 100.0 * (sender->packets_sent - receiver->packets_received) / sender->packets_sent;
        printf("  Packet loss rate: %.2f%%\n", loss_rate);
        printf("  Delivery rate: %.2f%%\n", 100.0 - loss_rate);
    }
    printf("\n========================================\n\n");

cleanup:
    if (sender) sender_destroy(sender);
    if (network) network_destroy(network);
    if (receiver) receiver_destroy(receiver);
    if (global_scheduler) event_scheduler_destroy(global_scheduler);
}
