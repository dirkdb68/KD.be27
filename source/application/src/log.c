#include "log.h"

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

static FILE *g_logfile = NULL;
static pthread_mutex_t g_log_lock = PTHREAD_MUTEX_INITIALIZER;

void log_init(const char *path) {
    if (path) {
        g_logfile = fopen(path, "a");
        if (!g_logfile) {
            fprintf(stderr, "log_init: could not open %s, logging to stdout only\n", path);
        }
    }
}

void log_msg(const char *level, const char *fmt, ...) {
    struct timespec tsnow;
    clock_gettime(CLOCK_REALTIME, &tsnow);
    struct tm tmv;
    localtime_r(&tsnow.tv_sec, &tmv);

    char ts[32];
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", &tmv);

    char line[512];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(line, sizeof(line), fmt, ap);
    va_end(ap);

    pthread_mutex_lock(&g_log_lock);
    fprintf(stdout, "%s.%03ld [%-5s] %s\n", ts, tsnow.tv_nsec / 1000000, level, line);
    fflush(stdout);
    if (g_logfile) {
        fprintf(g_logfile, "%s.%03ld [%-5s] %s\n", ts, tsnow.tv_nsec / 1000000, level, line);
        fflush(g_logfile);
    }
    pthread_mutex_unlock(&g_log_lock);
}
