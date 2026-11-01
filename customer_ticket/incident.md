# Support ticket #AR-4471

**Customer:** Apex Raceway
**Product:** Trackside Telemetry Gateway, release 3.4
**Priority:** Medium impact / High urgency
**Reported by:** Track operations (via the regional reseller)

## Summary

Since upgrading to release 3.4, the trackside gateway that feeds lap
telemetry (engine temp, tire temp, throttle position, kart battery) to
the live timing board intermittently:

- loses connectivity to the venue's MQTT broker,
- runs at very high CPU while this happens,
- occasionally restarts the gateway service entirely.

## Frequency

Roughly 1 to 3 times per operating day, no obvious pattern the track's
own IT contact could identify. It does not correlate with any single
kart, sensor, or time of day that they've noticed.

## Impact

The live timing board freezes or shows stale data during the outage.
Lap telemetry is not lost (the gateway buffers it and sends it once the
connection is back), but during an event this is visible to spectators
and annoying for the race director.

## What the customer has already tried

- Rebooting the gateway unit: temporarily "fixes" it, recurs later the
  same day.
- Swapping the network cable/switch port: no change, so they no longer
  suspect their own network hardware.
- Running it overnight with nobody on track: still happens, so it is
  not specific to high kart traffic.

## Attached

- `artifacts/logs/gateway.log` -- a reproduction captured on a bench
  unit against a flaky broker connection (release notes below).
- `artifacts/sysstat/pidstat.log` -- CPU/memory sampling taken during
  the same reproduction.
- `artifacts/support_notes/support_engineer_journal.txt` -- a support
  engineer's own notes while poking at this before escalating.
- `docs/architecture.md`, `docs/release_notes.md` -- what we have on
  file for this release.
- `source/application/` -- the gateway's source, as shipped in 3.4.

## Ask

Find out what's actually going on, and tell us what you'd suggest. A
quick fix for the symptom is fine as a starting point, but we'd like to
understand the real cause before release 3.5.
