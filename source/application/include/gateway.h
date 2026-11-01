#ifndef GATEWAY_H
#define GATEWAY_H

#include <time.h>

#define GATEWAY_VERSION "3.4.0"

/* Monotonic milliseconds, used for all internal timing (not wall clock). */
static inline long long now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
}

#endif /* GATEWAY_H */
