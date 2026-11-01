#ifndef GATEWAY_CONFIG_H
#define GATEWAY_CONFIG_H

typedef struct {
    char broker_host[128];
    int broker_port;
    char client_id[64];
    char topic[128];
    int sample_interval_ms;
    int watchdog_timeout_s;
    int keepalive_s;
    char log_file[256];
} gateway_config_t;

void config_load_defaults(gateway_config_t *cfg);

/* Simple "key=value" file, one per line, '#' comments. Missing file is
 * not an error -- defaults (optionally overridden by env) apply. */
int config_load_file(gateway_config_t *cfg, const char *path);

/* Overrides via GATEWAY_BROKER_HOST, GATEWAY_BROKER_PORT,
 * GATEWAY_SAMPLE_INTERVAL_MS, GATEWAY_WATCHDOG_TIMEOUT_S. */
void config_load_env(gateway_config_t *cfg);

#endif /* GATEWAY_CONFIG_H */
