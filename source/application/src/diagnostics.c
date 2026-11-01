#include "diagnostics.h"
#include "gateway.h"
#include "log.h"

#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

void diagnostics_init(diagnostics_t *d, offline_queue_t *q, mqtt_client_t *mqtt) {
    d->queue = q;
    d->mqtt = mqtt;
    d->started_ms = now_ms();
}

/* Periodic self-reported health line: queue depth, connection state and
 * RSS. Cheap on-box substitute for a real metrics pipeline (Apex
 * Raceway's trackside gateways rarely have one). */
void *diagnostics_thread(void *arg) {
    diagnostics_t *d = (diagnostics_t *)arg;

    while (1) {
        sleep(5);

        struct rusage ru;
        getrusage(RUSAGE_SELF, &ru);

        long qsize = offline_queue_size(d->queue);
        int connected = d->mqtt->current_session && d->mqtt->current_session->connected;
        long long uptime_s = (now_ms() - d->started_ms) / 1000;

        LOG_INFO("diag: uptime=%llds queue=%ld connected=%d rss_kb=%ld",
                  uptime_s, qsize, connected, ru.ru_maxrss);
    }
    return NULL;
}
