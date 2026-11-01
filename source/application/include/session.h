#ifndef GATEWAY_SESSION_H
#define GATEWAY_SESSION_H

#include <mosquitto.h>

typedef struct session_ctx {
    struct mosquitto *mosq;
    int connected;
    long long created_ms;
    long reconnect_attempt;
} session_ctx_t;

session_ctx_t *session_create(const char *client_id);
void session_destroy(session_ctx_t *s);

#endif /* GATEWAY_SESSION_H */
