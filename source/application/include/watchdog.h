#ifndef GATEWAY_WATCHDOG_H
#define GATEWAY_WATCHDOG_H

void watchdog_init(int timeout_s);
void watchdog_feed(void);
void *watchdog_thread(void *arg);

#endif /* GATEWAY_WATCHDOG_H */
