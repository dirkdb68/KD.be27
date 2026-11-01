# Venue broker: runs mosquitto under scripts/track_network_sim.sh, which
# makes it reachable/unreachable on a seeded, deterministic schedule for the
# whole lifetime of the container, so every bench run sees the same
# connectivity pattern.

FROM debian:12-slim

RUN apt-get update \
    && apt-get install -y --no-install-recommends mosquitto procps \
    && rm -rf /var/lib/apt/lists/*

COPY docker/broker/mosquitto_venue.conf /etc/mosquitto-venue/mosquitto_venue.conf
COPY scripts/track_network_sim.sh /usr/local/bin/track_network_sim.sh
RUN chmod +x /usr/local/bin/track_network_sim.sh

# VENUE_NET_SEED/VENUE_NET_CYCLES can be overridden at `docker compose run`/env
# level; VENUE_NET_CYCLES=0 (the default here) means "run forever".
ENV VENUE_NET_SEED=42
ENV VENUE_NET_CYCLES=0

ENTRYPOINT ["/bin/sh", "-c", "exec /usr/local/bin/track_network_sim.sh \"$VENUE_NET_SEED\" \"$VENUE_NET_CYCLES\""]
