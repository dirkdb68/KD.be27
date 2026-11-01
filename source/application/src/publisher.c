#include "publisher.h"
#include "watchdog.h"
#include "sensors.h"
#include "log.h"
#include "gateway.h"

#include <unistd.h>

/* Short pause before retrying a failed publish, so a backlog built up
 * during an outage doesn't hammer the client while it's disconnected.
 * Doesn't affect throughput while connected. */
#define PUBLISH_RETRY_BACKOFF_MS 50

/* Same rationale as mqtt_client.c's connect-failure throttling: during an
 * outage every queued reading fails to publish, and logging each one
 * would flood the log without adding information. */
static long long g_last_pub_fail_log_ms = 0;
static long g_pub_fail_since_log = 0;

void publisher_init(publisher_t *p, offline_queue_t *q, mqtt_client_t *mqtt) {
    p->queue = q;
    p->mqtt = mqtt;
    p->running = 1;
}

/* Drains the offline queue and publishes each reading. Also the only
 * place that feeds the watchdog: a successful publish is the clearest
 * proof the gateway is actually doing its job, end to end. */
void *publisher_thread(void *arg) {
    publisher_t *p = (publisher_t *)arg;
    char payload[256];

    while (p->running) {
        sensor_reading_t r;
        if (!offline_queue_pop(p->queue, &r, 500)) {
            continue; /* nothing queued right now */
        }

        int len = sensors_format_json(&r, payload, sizeof(payload));
        int rc = mqtt_client_publish(p->mqtt, p->mqtt->cfg->topic, payload, len);
        if (rc == 0) {
            watchdog_feed();
        } else {
            long long t = now_ms();
            g_pub_fail_since_log++;
            if (t - g_last_pub_fail_log_ms >= 1000) {
                LOG_WARN("publisher: publish failed (rc=%d), re-queuing reading (%ld failures in the last %lldms)",
                          rc, g_pub_fail_since_log, t - g_last_pub_fail_log_ms);
                g_last_pub_fail_log_ms = t;
                g_pub_fail_since_log = 0;
            }
            offline_queue_push_front(p->queue, &r);
            usleep(PUBLISH_RETRY_BACKOFF_MS * 1000);
        }
    }
    return NULL;
}
