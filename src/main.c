#include <stdio.h>
#include <stdlib.h>
#include "event.h"
#include "event_scheduler.h"
#include "event_loop.h"

// A simple test task
void test_task(void *context) {
    int id = *(int *)context;
    printf("[Task] Executed task id = %d at sim time = %llu\n",
           id, current_sim_time);
}

int main() {
    // Create scheduler with initial capacity
    EventScheduler *scheduler = event_scheduler_create(10);
    if (!scheduler) {
        fprintf(stderr, "Failed to create scheduler!\n");
        return 1;
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

    return 0;
}
