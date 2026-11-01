#ifndef GATEWAY_MQTT_CLIENT_H
#define GATEWAY_MQTT_CLIENT_H

#include "config.h"
#include "session.h"
#include <pthread.h>

typedef struct {
    gateway_config_t *cfg;
    volatile int running;
    pthread_mutex_t session_lock;
    session_ctx_t *current_session; /* guarded by session_lock on the read side only */
} mqtt_client_t;

void mqtt_client_init(mqtt_client_t *mc, gateway_config_t *cfg);
void *mqtt_client_reconnect_thread(void *arg);
/* Returns 0 on success, non-zero if not currently connected or publish failed. */
int mqtt_client_publish(mqtt_client_t *mc, const char *topic, const char *payload, int len);

#endif /* GATEWAY_MQTT_CLIENT_H */
