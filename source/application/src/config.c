#include "config.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void config_load_defaults(gateway_config_t *cfg) {
    memset(cfg, 0, sizeof(*cfg));
    strncpy(cfg->broker_host, "mosquitto", sizeof(cfg->broker_host) - 1);
    cfg->broker_port = 1883;
    strncpy(cfg->client_id, "apexraceway-gateway", sizeof(cfg->client_id) - 1);
    strncpy(cfg->topic, "apexraceway/track1/telemetry", sizeof(cfg->topic) - 1);
    cfg->sample_interval_ms = 250;
    cfg->watchdog_timeout_s = 20;
    cfg->keepalive_s = 10;
    strncpy(cfg->log_file, "/var/log/gateway/gateway.log", sizeof(cfg->log_file) - 1);
}

static void trim(char *s) {
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\n' || s[n - 1] == '\r' || s[n - 1] == ' ' || s[n - 1] == '\t')) {
        s[--n] = '\0';
    }
}

int config_load_file(gateway_config_t *cfg, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        LOG_WARN("config: %s not found, using defaults/env", path);
        return -1;
    }

    char line[512];
    while (fgets(line, sizeof(line), f)) {
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '#' || *p == '\n' || *p == '\0') continue;

        char *eq = strchr(p, '=');
        if (!eq) continue;
        *eq = '\0';
        char *key = p;
        char *val = eq + 1;
        trim(key);
        trim(val);

        if (strcmp(key, "broker_host") == 0) {
            strncpy(cfg->broker_host, val, sizeof(cfg->broker_host) - 1);
        } else if (strcmp(key, "broker_port") == 0) {
            cfg->broker_port = atoi(val);
        } else if (strcmp(key, "client_id") == 0) {
            strncpy(cfg->client_id, val, sizeof(cfg->client_id) - 1);
        } else if (strcmp(key, "topic") == 0) {
            strncpy(cfg->topic, val, sizeof(cfg->topic) - 1);
        } else if (strcmp(key, "sample_interval_ms") == 0) {
            cfg->sample_interval_ms = atoi(val);
        } else if (strcmp(key, "watchdog_timeout_s") == 0) {
            cfg->watchdog_timeout_s = atoi(val);
        } else if (strcmp(key, "keepalive_s") == 0) {
            cfg->keepalive_s = atoi(val);
        } else if (strcmp(key, "log_file") == 0) {
            strncpy(cfg->log_file, val, sizeof(cfg->log_file) - 1);
        } else {
            LOG_WARN("config: unknown key '%s' in %s, ignored", key, path);
        }
    }
    fclose(f);
    return 0;
}

void config_load_env(gateway_config_t *cfg) {
    const char *v;
    if ((v = getenv("GATEWAY_BROKER_HOST"))) strncpy(cfg->broker_host, v, sizeof(cfg->broker_host) - 1);
    if ((v = getenv("GATEWAY_BROKER_PORT"))) cfg->broker_port = atoi(v);
    if ((v = getenv("GATEWAY_SAMPLE_INTERVAL_MS"))) cfg->sample_interval_ms = atoi(v);
    if ((v = getenv("GATEWAY_WATCHDOG_TIMEOUT_S"))) cfg->watchdog_timeout_s = atoi(v);
}
