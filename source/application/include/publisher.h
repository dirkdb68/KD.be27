#ifndef GATEWAY_PUBLISHER_H
#define GATEWAY_PUBLISHER_H

#include "offline_queue.h"
#include "mqtt_client.h"

typedef struct {
    offline_queue_t *queue;
    mqtt_client_t *mqtt;
    volatile int running;
} publisher_t;

void publisher_init(publisher_t *p, offline_queue_t *q, mqtt_client_t *mqtt);
void *publisher_thread(void *arg);

#endif /* GATEWAY_PUBLISHER_H */
