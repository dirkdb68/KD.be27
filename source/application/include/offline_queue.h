#ifndef GATEWAY_OFFLINE_QUEUE_H
#define GATEWAY_OFFLINE_QUEUE_H

#include "sensors.h"
#include <pthread.h>

typedef struct queue_node {
    sensor_reading_t reading;
    struct queue_node *next;
} queue_node_t;

/* FIFO used to buffer readings across broker outages (the "offline
 * buffering" feature added in 3.4 to avoid data loss, AGS-2231). */
typedef struct {
    queue_node_t *head;
    queue_node_t *tail;
    long size;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
} offline_queue_t;

void offline_queue_init(offline_queue_t *q);
void offline_queue_push(offline_queue_t *q, const sensor_reading_t *r);
void offline_queue_push_front(offline_queue_t *q, const sensor_reading_t *r);
/* Waits up to wait_ms for an item; returns 1 and fills *out, or 0 on timeout. */
int offline_queue_pop(offline_queue_t *q, sensor_reading_t *out, int wait_ms);
long offline_queue_size(offline_queue_t *q);

#endif /* GATEWAY_OFFLINE_QUEUE_H */
