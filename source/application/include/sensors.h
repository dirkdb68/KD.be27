#ifndef GATEWAY_SENSORS_H
#define GATEWAY_SENSORS_H

typedef struct {
    long long timestamp_ms;
    double engine_temp_c;
    double tire_temp_c;
    double throttle_pct;
    double battery_v;
} sensor_reading_t;

void sensors_init(unsigned int seed);
void sensors_read(sensor_reading_t *out);
int sensors_format_json(const sensor_reading_t *r, char *buf, int buflen);

#endif /* GATEWAY_SENSORS_H */
