#include "session.h"
#include "gateway.h"

#include <stdlib.h>

session_ctx_t *session_create(const char *client_id) {
    session_ctx_t *s = malloc(sizeof(session_ctx_t));
    if (!s) return NULL;
    s->mosq = mosquitto_new(client_id, true, NULL);
    s->connected = 0;
    s->created_ms = now_ms();
    s->reconnect_attempt = 0;
    return s;
}

/*
 * Tears down an MQTT session.
 *
 * NOTE (rel. 3.4 / GW-512): this used to leak the whole session_ctx_t on
 * every reconnect. Fixed by destroying the mosquitto handle here instead
 * of leaving it to the caller.
 *
 * TODO: audit callers for thread-safety. This is invoked from the
 * reconnect loop at the moment a new session takes over -- the publisher
 * thread may still hold a pointer to the session being replaced.
 */
void session_destroy(session_ctx_t *s) {
    if (!s) return;
    if (s->mosq) {
        mosquitto_destroy(s->mosq);
    }
}
