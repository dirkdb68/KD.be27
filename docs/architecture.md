# Trackside Telemetry Gateway -- architecture (release 3.4)

Four threads, one process, no external dependencies beyond an MQTT
broker:

```
 main thread            reconnect thread          publisher thread        watchdog thread
 (sampling)             (mqtt_client.c)           (publisher.c)           (watchdog.c)
     |                        |                         |                      |
 sensors_read()          keeps a live                drains the            exits the process
 every sample_          mosquitto handle            offline queue,          if nothing has
 interval_ms,           in current_session,         publishes each         fed it within
 pushes onto the        reconnecting with           reading, feeds         watchdog_timeout_s
 offline queue          no fixed poll interval       the watchdog on
                        when the broker drops        a successful
                                                      publish
```

## Modules

- **sensors.c** -- simulated kart telemetry (engine/tire temperature,
  throttle position, battery voltage). Not networked; just generates
  readings on a timer.
- **offline_queue.c** -- an unbounded FIFO. Added in 3.4 (see release
  notes) so a reading is never dropped, however long the broker is
  unreachable.
- **session.c** -- a small struct wrapping one `mosquitto` handle plus
  its connection metadata (attempt number, timestamps).
- **mqtt_client.c** -- owns reconnecting to the broker and keeps
  `current_session` pointing at whichever session is currently live.
- **publisher.c** -- the only consumer of the offline queue; the only
  thread that calls `mqtt_client_publish()`.
- **watchdog.c** -- a simple "have I been fed recently" timer. If not,
  it logs an error and exits the process; the surrounding supervisor
  (systemd in the field, Docker's restart policy here) brings it back.
- **diagnostics.c** -- a periodic one-line health report (queue depth,
  connection state, RSS) written to the same log, every 5 seconds.

## Note

This document describes the pieces and how they're wired together --
it is release documentation, not a verified design review, and nobody
has re-checked it against the current source for this ticket yet.
