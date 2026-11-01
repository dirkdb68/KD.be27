# Trackside Telemetry Gateway -- release notes

## 3.4 (current)

- **AGS-2231 -- offline buffering.** Previously, a reading sampled while
  the gateway was disconnected from the broker was simply lost. 3.4
  adds an in-memory queue: every reading is buffered and replayed once
  the connection is back, so a short outage no longer means a gap in
  the lap telemetry.
- **GW-512 -- reconnect session leak fixed.** An earlier release leaked
  the whole per-connection session object on every reconnect. This is
  now freed as part of tearing down the old session when a new one
  takes over.
- Reconnect logic now reacts to the broker coming back as fast as
  possible, instead of polling on a fixed interval -- customers asked
  for less lag after a brief network blip.

## 3.3

- Initial release of the current gateway architecture (separate
  sampling, reconnect and publish threads, watchdog-supervised).
- Known issue: a reading sampled during a broker outage was dropped,
  not buffered (fixed in 3.4, see AGS-2231).
- Known issue: the per-reconnect session object was never freed (fixed
  in 3.4, see GW-512).

## Known issues (open)

- Field reports of occasional CPU spikes and service restarts
  correlating with network instability at some venues. Under
  investigation -- this ticket.
