#ifndef GATEWAY_LOG_H
#define GATEWAY_LOG_H

/* Minimal thread-safe logger. Writes to stdout, and additionally to a
 * file if log_init() was given a path. */

void log_init(const char *path); /* path may be NULL for stdout-only */
void log_msg(const char *level, const char *fmt, ...);

#define LOG_INFO(...)  log_msg("INFO",  __VA_ARGS__)
#define LOG_WARN(...)  log_msg("WARN",  __VA_ARGS__)
#define LOG_ERROR(...) log_msg("ERROR", __VA_ARGS__)
#define LOG_DEBUG(...) log_msg("DEBUG", __VA_ARGS__)

#endif /* GATEWAY_LOG_H */
