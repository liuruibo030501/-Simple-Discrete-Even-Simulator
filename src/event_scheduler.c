#include <stdio.h>
#include <stdlib.h>
#include "event_scheduler.h"

//switch two events
static void swap(Event **a, Event **b) {
    Event *tmp = *a;
    *a = *b;
    *b = tmp;
}

//creater
EventScheduler *event_scheduler_create(size_t capacity){
    EventScheduler *scheduler = malloc(sizeof(EventScheduler));
    if (!scheduler) return NULL;

    scheduler->heap = malloc(sizeof(Event *) * capacity);
    if (!scheduler->heap) {
        free(scheduler);
        return NULL;
    }

    scheduler->size = 0;
    scheduler->capacity = capacity;
    return scheduler;
}

//destroyer
void event_scheduler_destroy(EventScheduler *scheduler){
    if (!scheduler) return;
    free(scheduler->heap);
    free(scheduler);
}

//find parent and swap if needed
static void heapify_up(EventScheduler *scheduler, size_t idx){
    while (idx > 0) {
        size_t parent = (idx - 1) / 2;
        if (scheduler->heap[parent]->time <= scheduler->heap[idx]->time)
            break;
        swap(&scheduler->heap[parent], &scheduler->heap[idx]);
        idx = parent;
    }
}

//find children and swap if needed
static void heapify_down(EventScheduler *scheduler, size_t idx){
    while (1) {
        size_t left = idx * 2 + 1;
        size_t right = idx * 2 + 2;
        size_t smallest = idx;

        if (left < scheduler->size &&
            scheduler->heap[left]->time < scheduler->heap[smallest]->time)
            smallest = left;

        if (right < scheduler->size &&
            scheduler->heap[right]->time < scheduler->heap[smallest]->time)
            smallest = right;

        if (smallest == idx) break;

        swap(&scheduler->heap[idx], &scheduler->heap[smallest]);
        idx = smallest;
    }
}


int event_scheduler_push(EventScheduler *scheduler, Event *ev){
    if (scheduler->size >= scheduler->capacity)
        return -1; 

    scheduler->heap[scheduler->size] = ev;
    heapify_up(scheduler, scheduler->size);
    scheduler->size++;

    return 0;
}


Event *event_scheduler_pop(EventScheduler *scheduler){
    if (scheduler->size == 0)
        return NULL;

    Event *ev = scheduler->heap[0];

    scheduler->heap[0] = scheduler->heap[scheduler->size - 1];
    scheduler->size--;

    heapify_down(scheduler, 0);

    return ev;
}


void event_scheduler_print(EventScheduler *scheduler){
    printf("EventScheduler size=%zu capacity=%zu\n",
           scheduler->size, scheduler->capacity);

    for (size_t i = 0; i < scheduler->size; i++) {
        printf("  [%zu] time=%llu type=%d\n",
               i,
               (unsigned long long)scheduler->heap[i]->time,
               scheduler->heap[i]->type);
    }
}
