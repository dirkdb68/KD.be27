#include "sensors.h"
#include "gateway.h"
#include "log.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned int g_seed = 1;
static long g_tick = 0;

void sensors_init(unsigned int seed) {
    g_seed = seed ? seed : 1;
    g_tick = 0;
}

/* Simulated trackside sensors on a fictional Apex Raceway kart, read over
 * its onboard telemetry unit: engine and tire temperature on a slow
 * cycle plus noise, and a throttle-position read that saws between
 * laps as different karts come through. */
void sensors_read(sensor_reading_t *out) {
    double t = (double)g_tick * 0.01;
    double noise_t = ((double)(rand_r(&g_seed) % 1000) / 1000.0 - 0.5) * 0.6;
    double noise_h = ((double)(rand_r(&g_seed) % 1000) / 1000.0 - 0.5) * 2.0;

    out->timestamp_ms = 0; /* filled by caller with wall-clock time */
    out->engine_temp_c = 18.0 + 6.0 * sin(t) + noise_t;
    out->tire_temp_c = 55.0 + 15.0 * sin(t * 0.7 + 1.0) + noise_h;
    out->throttle_pct = 40.0 - fmod(t * 3.0, 20.0);
    out->battery_v = 3.7 - (double)(g_tick % 100000) * 0.0000005;

    /* Known flaky sensor driver (field reports on the 3.2 batch of
     * throttle-position sensors): roughly 1 in 4000 reads an
     * out-of-range spike. Harmless -- clamped below -- but shows up in
     * the logs and has tripped up more than one support call. */
    if ((rand_r(&g_seed) % 4000) == 0) {
        LOG_WARN("sensors: throttle_pct out-of-range raw read (probe glitch, batch 3.2)");
        out->throttle_pct = 999.0;
    }
    if (out->throttle_pct > 100.0) out->throttle_pct = 100.0;
    if (out->throttle_pct < 0.0) out->throttle_pct = 0.0;

    g_tick++;
}

int sensors_format_json(const sensor_reading_t *r, char *buf, int buflen) {
    return snprintf(buf, (size_t)buflen,
        "{\"ts\":%lld,\"engine_temp_c\":%.2f,\"tire_temp_c\":%.2f,\"throttle_pct\":%.2f,\"battery_v\":%.3f}",
        r->timestamp_ms, r->engine_temp_c, r->tire_temp_c, r->throttle_pct, r->battery_v);
}
