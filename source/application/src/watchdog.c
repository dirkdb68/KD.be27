#include "watchdog.h"
#include "log.h"
#include "gateway.h"

#include <stdatomic.h>
#include <stdlib.h>
#include <unistd.h>

static _Atomic long long g_last_feed_ms = 0;
static int g_timeout_s = 20;

void watchdog_init(int timeout_s) {
    g_timeout_s = timeout_s;
    g_last_feed_ms = now_ms();
}

void watchdog_feed(void) {
    g_last_feed_ms = now_ms();
}

/*
 * Restarts the process if nothing has fed the watchdog within the
 * configured timeout. Fed from the publisher's successful-publish path
 * (see publisher.c) -- that's intentional: a stalled network link is
 * exactly the failure mode we want to recover from, and the container/
 * systemd restart policy brings us back cleanly.
 */
void *watchdog_thread(void *arg) {
    (void)arg;
    while (1) {
        sleep(1);
        long long since = now_ms() - g_last_feed_ms;
        if (since > (long long)g_timeout_s * 1000) {
            LOG_ERROR("watchdog: no heartbeat for %lld ms (timeout=%ds) -- restarting", since, g_timeout_s);
            exit(42);
        }
    }
    return NULL;
}
