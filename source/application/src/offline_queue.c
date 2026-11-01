#include "offline_queue.h"

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

void offline_queue_init(offline_queue_t *q) {
    q->head = q->tail = NULL;
    q->size = 0;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_empty, NULL);
}

/* No capacity limit by design: release 3.4 removed the old fixed-size ring
 * buffer (AGS-2231) so that a brief outage never drops a reading. A long
 * outage simply grows this queue -- see docs/release_notes.md. */
void offline_queue_push(offline_queue_t *q, const sensor_reading_t *r) {
    queue_node_t *n = malloc(sizeof(queue_node_t));
    if (!n) return; /* OOM: drop the reading rather than crash */
    n->reading = *r;
    n->next = NULL;

    pthread_mutex_lock(&q->lock);
    if (q->tail) {
        q->tail->next = n;
    } else {
        q->head = n;
    }
    q->tail = n;
    q->size++;
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}

void offline_queue_push_front(offline_queue_t *q, const sensor_reading_t *r) {
    queue_node_t *n = malloc(sizeof(queue_node_t));
    if (!n) return;
    n->reading = *r;

    pthread_mutex_lock(&q->lock);
    n->next = q->head;
    q->head = n;
    if (!q->tail) q->tail = n;
    q->size++;
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}

int offline_queue_pop(offline_queue_t *q, sensor_reading_t *out, int wait_ms) {
    struct timespec deadline;
    clock_gettime(CLOCK_REALTIME, &deadline);
    deadline.tv_sec += wait_ms / 1000;
    deadline.tv_nsec += (wait_ms % 1000) * 1000000L;
    if (deadline.tv_nsec >= 1000000000L) {
        deadline.tv_sec++;
        deadline.tv_nsec -= 1000000000L;
    }

    pthread_mutex_lock(&q->lock);
    while (q->head == NULL) {
        int rc = pthread_cond_timedwait(&q->not_empty, &q->lock, &deadline);
        if (rc != 0) {
            pthread_mutex_unlock(&q->lock);
            return 0;
        }
    }

    queue_node_t *n = q->head;
    q->head = n->next;
    if (!q->head) q->tail = NULL;
    q->size--;
    pthread_mutex_unlock(&q->lock);

    *out = n->reading;
    free(n);
    return 1;
}

long offline_queue_size(offline_queue_t *q) {
    long sz;
    pthread_mutex_lock(&q->lock);
    sz = q->size;
    pthread_mutex_unlock(&q->lock);
    return sz;
}
