#include "config.h"
#include "diagnostics.h"
#include "gateway.h"
#include "log.h"
#include "mqtt_client.h"
#include "offline_queue.h"
#include "publisher.h"
#include "sensors.h"
#include "watchdog.h"

#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

static gateway_config_t g_cfg;
static offline_queue_t g_queue;
static mqtt_client_t g_mqtt;
static publisher_t g_publisher;
static diagnostics_t g_diag;

int main(int argc, char **argv) {
    config_load_defaults(&g_cfg);
    if (argc > 1) {
        config_load_file(&g_cfg, argv[1]);
    } else {
        config_load_file(&g_cfg, "/etc/gateway/gateway.conf");
    }
    config_load_env(&g_cfg);

    log_init(NULL); /* container logs to stdout; no file by default */

    LOG_INFO("Apex Raceway Trackside Telemetry Gateway v%s starting", GATEWAY_VERSION);
    LOG_INFO("config: broker=%s:%d client_id=%s topic=%s sample_ms=%d watchdog_s=%d",
              g_cfg.broker_host, g_cfg.broker_port, g_cfg.client_id, g_cfg.topic,
              g_cfg.sample_interval_ms, g_cfg.watchdog_timeout_s);

    mosquitto_lib_init();

    sensors_init((unsigned int)getpid());
    offline_queue_init(&g_queue);
    mqtt_client_init(&g_mqtt, &g_cfg);
    publisher_init(&g_publisher, &g_queue, &g_mqtt);
    diagnostics_init(&g_diag, &g_queue, &g_mqtt);
    watchdog_init(g_cfg.watchdog_timeout_s);

    pthread_t t_reconnect, t_publisher, t_watchdog, t_diag;
    pthread_create(&t_reconnect, NULL, mqtt_client_reconnect_thread, &g_mqtt);
    pthread_create(&t_publisher, NULL, publisher_thread, &g_publisher);
    pthread_create(&t_watchdog, NULL, watchdog_thread, NULL);
    pthread_create(&t_diag, NULL, diagnostics_thread, &g_diag);

    /* Main thread: sample sensors and buffer every reading. Publishing
     * itself (and therefore network availability) is entirely the
     * publisher thread's problem -- this loop never blocks on it. */
    for (;;) {
        sensor_reading_t r;
        sensors_read(&r);

        struct timeval tv;
        gettimeofday(&tv, NULL);
        r.timestamp_ms = (long long)tv.tv_sec * 1000LL + tv.tv_usec / 1000;

        offline_queue_push(&g_queue, &r);
        usleep((useconds_t)g_cfg.sample_interval_ms * 1000);
    }

    return 0;
}
