#include "mqtt_client.h"
#include "log.h"
#include "gateway.h"

#include <stdlib.h>

static void on_connect_cb(struct mosquitto *mosq, void *userdata, int rc) {
    (void)mosq;
    (void)userdata;
    if (rc == 0) {
        LOG_INFO("mqtt: connect callback OK");
    } else {
        LOG_WARN("mqtt: connect callback rc=%d", rc);
    }
}

static void on_disconnect_cb(struct mosquitto *mosq, void *userdata, int rc) {
    (void)mosq;
    mqtt_client_t *mc = (mqtt_client_t *)userdata;
    LOG_WARN("mqtt: disconnected (rc=%d)", rc);
    if (mc->current_session) {
        mc->current_session->connected = 0;
    }
}

void mqtt_client_init(mqtt_client_t *mc, gateway_config_t *cfg) {
    mc->cfg = cfg;
    mc->running = 1;
    mc->current_session = NULL;
    pthread_mutex_init(&mc->session_lock, NULL);
}

/*
 * Connect-failure logging is rate-limited to one line per second, with a
 * count of the attempts in between -- field units have small flash and a
 * flaky venue network shouldn't be able to fill it with log lines.
 */
static long long g_last_connect_fail_log_ms = 0;
static long g_connect_fail_since_log = 0;

static session_ctx_t *try_connect(mqtt_client_t *mc, long attempt) {
    session_ctx_t *s = session_create(mc->cfg->client_id);
    if (!s || !s->mosq) {
        LOG_ERROR("mqtt: could not allocate mosquitto handle");
        free(s);
        return NULL;
    }
    s->reconnect_attempt = attempt;

    mosquitto_connect_callback_set(s->mosq, on_connect_cb);
    mosquitto_disconnect_callback_set(s->mosq, on_disconnect_cb);
    mosquitto_user_data_set(s->mosq, mc);

    int rc = mosquitto_connect(s->mosq, mc->cfg->broker_host, mc->cfg->broker_port, mc->cfg->keepalive_s);
    if (rc != MOSQ_ERR_SUCCESS) {
        long long t = now_ms();
        g_connect_fail_since_log++;
        if (t - g_last_connect_fail_log_ms >= 1000) {
            LOG_WARN("mqtt: connect() failed: %s (%ld attempts in the last %lldms)",
                      mosquitto_strerror(rc), g_connect_fail_since_log, t - g_last_connect_fail_log_ms);
            g_last_connect_fail_log_ms = t;
            g_connect_fail_since_log = 0;
        }
        mosquitto_destroy(s->mosq);
        free(s);
        return NULL;
    }
    s->connected = 1;
    return s;
}

/*
 * Reconnect loop: keeps mc->current_session pointing at a live mosquitto
 * handle for the lifetime of the process.
 */
void *mqtt_client_reconnect_thread(void *arg) {
    mqtt_client_t *mc = (mqtt_client_t *)arg;
    long attempt = 0;

    while (mc->running) {
        session_ctx_t *cur = mc->current_session;

        if (!cur || !cur->connected) {
            session_ctx_t *fresh = try_connect(mc, ++attempt);
            if (!fresh) {
                /* Broker not reachable right now. 3.4 reacts to broker
                 * availability as fast as possible rather than polling on
                 * a fixed interval -- see docs/release_notes.md. */
                continue;
            }

            /* Swap in the new session and tear down the old one. The
             * session_lock only needs to protect publish() against a NULL
             * session pointer, so the swap itself doesn't take it. */
            session_ctx_t *old = mc->current_session;
            mc->current_session = fresh;
            LOG_INFO("mqtt: session swapped in (attempt=%ld)", fresh->reconnect_attempt);
            if (old) {
                session_destroy(old);
            }
            continue;
        }

        int rc = mosquitto_loop(cur->mosq, 100, 1);
        if (rc != MOSQ_ERR_SUCCESS) {
            LOG_WARN("mqtt: loop() rc=%d (%s)", rc, mosquitto_strerror(rc));
            cur->connected = 0;
        }
    }
    return NULL;
}

int mqtt_client_publish(mqtt_client_t *mc, const char *topic, const char *payload, int len) {
    int rc;
    pthread_mutex_lock(&mc->session_lock);
    session_ctx_t *s = mc->current_session;
    if (!s || !s->connected || !s->mosq) {
        pthread_mutex_unlock(&mc->session_lock);
        return -1;
    }
    rc = mosquitto_publish(s->mosq, NULL, topic, len, payload, 0, false);
    pthread_mutex_unlock(&mc->session_lock);
    return rc;
}
