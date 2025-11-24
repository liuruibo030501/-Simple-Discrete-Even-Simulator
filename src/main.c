#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "event.h"
#include "event_scheduler.h"
#include "event_loop.h"
#include "network_sim.h"

// A simple test task
void test_task(void *context) {
    int id = *(int *)context;
    printf("[Task] Executed task id = %d at sim time = %llu\n",
           id, current_sim_time);
}

void run_simple_test(void) {
    printf("\n=== Running Simple Test ===\n");

    // Create scheduler with initial capacity
    EventScheduler *scheduler = event_scheduler_create(10);
    if (!scheduler) {
        fprintf(stderr, "Failed to create scheduler!\n");
        return;
    }

    // Test context variables
    int a = 1, b = 2, c = 3;

    // Create events with different times
    Event *e1 = event_create(30, EVENT_CUSTOM, test_task, &a, NULL);
    Event *e2 = event_create(10, EVENT_CUSTOM, test_task, &b, NULL);
    Event *e3 = event_create(20, EVENT_CUSTOM, test_task, &c, NULL);

    // Push events into scheduler
    event_scheduler_push(scheduler, e1);
    event_scheduler_push(scheduler, e2);
    event_scheduler_push(scheduler, e3);

    // Run simulation loop
    printf("=== Starting Event Loop ===\n");
    event_loop_run(scheduler);
    printf("=== Event Loop Finished ===\n");

    // Cleanup
    event_scheduler_destroy(scheduler);
}

void print_usage(const char *progname) {
    printf("Usage: %s [mode]\n", progname);
    printf("\nModes:\n");
    printf("  test      - Run simple event scheduler test\n");
    printf("  demo1     - Network simulation with fixed interval (100ms, 20 packets)\n");
    printf("  demo2     - Network simulation with exponential distribution\n");
    printf("  demo3     - Long running simulation (1000 packets)\n");
    printf("  default   - Run demo1 (if no mode specified)\n");
    printf("\nExamples:\n");
    printf("  %s test\n", progname);
    printf("  %s demo1\n", progname);
    printf("  %s demo2\n", progname);
}

int main(int argc, char *argv[]) {
    const char *mode = "demo1";  // Default mode

    if (argc > 1) {
        mode = argv[1];
    }

    if (strcmp(mode, "help") == 0 || strcmp(mode, "-h") == 0 || strcmp(mode, "--help") == 0) {
        print_usage(argv[0]);
        return 0;
    }

    if (strcmp(mode, "test") == 0) {
        run_simple_test();
    }
    else if (strcmp(mode, "demo1") == 0) {
        printf("\n=== DEMO 1: Fixed Interval Network Simulation ===\n");
        printf("Sender: Every 100 time units\n");
        printf("Finish: After 2000 time units (~20 packets)\n");
        printf("Network delay: 10-50 time units\n\n");

        run_network_simulation(
            100,
            2000,
            SENDER_FIXED_INTERVAL,
            0.01,
            10,
            50,
            512 // packet_size bytes
        );
    }
    else if (strcmp(mode, "demo2") == 0) {
        printf("\n=== DEMO 2: Exponential Distribution Network Simulation ===\n");
        printf("Sender: Exponentially distributed (lambda=0.01)\n");
        printf("Finish: After 2000 time units\n");
        printf("Network delay: 10-50 time units\n\n");

        run_network_simulation(
            0,
            2000,
            SENDER_EXPONENTIAL,
            0.01,
            10,
            50,
            512 // packet_size bytes
        );
    }
    else if (strcmp(mode, "demo3") == 0) {
        printf("\n=== DEMO 3: Long Running Simulation ===\n");
        printf("Sender: Every 10 time units\n");
        printf("Finish: After 10000 time units (~1000 packets)\n");
        printf("Network delay: 5-20 time units\n\n");

        run_network_simulation(
            10,
            10000,
            SENDER_FIXED_INTERVAL,
            0.1,
            5,
            20,
            1024 // larger packet size for long run
        );
    }
    else {
        printf("Unknown mode: %s\n", mode);
        printf("Run '%s help' for usage information.\n\n", argv[0]);
        printf("Running default demo1...\n");

        run_network_simulation(
            100,
            2000,
            SENDER_FIXED_INTERVAL,
            0.01,
            10,
            50,
            512 // packet_size bytes
        );
    }

    return 0;
}
