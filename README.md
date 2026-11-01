# Operation Black Box

> Apex Raceway, its karts, its trackside BBQ, and its chronically
> flaky MQTT reconnects are entirely fictional. Any resemblance to an
> actual karting-and-BBQ team outing is purely coincidental, mildly
> suspicious, and absolutely not an admission that someone's lap timer
> froze mid-race.

You're the engineer who picked up [`customer_ticket/incident.md`](customer_ticket/incident.md).
A customer's gateway is misbehaving in the field. You have the source,
the docs, and a reproduction someone on the support side already
captured. Treat it like any other field-issue ticket that lands on
your desk.

## Suggested approach

- No prep needed, no specific hardware required.
- Set a timer for about 20-30 minutes when you start. When it goes
  off, just note for yourself (nobody collects this) which thread,
  which file, or which idea you'd gotten to.
- Look at it the way you'd look at a real customer issue: form a
  hypothesis, go check it, revise.
- There's no single "correct" answer expected in that time. What's
  interesting is where you started, what you ruled out, and what tools
  you reached for.

## Getting it running

```
docker compose up
```

That's the only command you need. It builds and starts the gateway
plus a broker that reproduces the customer's reported instability.
Gateway logs appear in your terminal; `docker compose logs -f gateway`
if you detach.

No Docker on your laptop? See [`docs/architecture.md`](docs/architecture.md)
for what the pieces are, and `scripts/run_native.sh` for building and
running it directly (needs gcc, cmake and a local mosquitto broker).

## Where things are

- [`customer_ticket/incident.md`](customer_ticket/incident.md) -- the ticket itself.
- `artifacts/` -- logs, CPU/memory sampling, and a support engineer's
  own notes from an earlier reproduction attempt.
- `source/application/` -- the gateway's source (C, CMake).
- `docs/` -- architecture and release notes, as filed for this release.
- `my_findings.md` -- a template if you want to jot your own notes down
  as you go. Entirely optional, and nobody reads it but you.

## At KD.be27

We'll compare notes: where people started, what turned out to be a
dead end, what tools got used, and what the full picture actually
looks like.
