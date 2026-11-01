#ifndef GATEWAY_DIAGNOSTICS_H
#define GATEWAY_DIAGNOSTICS_H

#include "offline_queue.h"
#include "mqtt_client.h"

typedef struct {
    offline_queue_t *queue;
    mqtt_client_t *mqtt;
    long long started_ms;
} diagnostics_t;

void diagnostics_init(diagnostics_t *d, offline_queue_t *q, mqtt_client_t *mqtt);
void *diagnostics_thread(void *arg);

#endif /* GATEWAY_DIAGNOSTICS_H */
