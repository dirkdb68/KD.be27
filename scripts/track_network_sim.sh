#!/bin/bash
# Bench simulation of the venue network: makes the MQTT broker reachable and
# unreachable on a seeded schedule, so every bench run sees the same pattern
# of connectivity loss as reported from the track.
#
# Usage: track_network_sim.sh <seed> <cycles> <broker_start_cmd> <broker_stop_cmd>
#
# cycles <= 0 means "run forever" (the seeded sequence just keeps extending);
# that's what the Docker broker container uses. A positive cycle count is for
# one-off, bounded bench runs.
#
# Default start/stop commands manage mosquitto directly via its config, so
# this works unmodified inside the Docker broker container (see
# docker-compose.yml).

set -u

SEED="${1:-42}"
CYCLES="${2:-12}"
START_CMD="${3:-mosquitto -c /etc/mosquitto-venue/mosquitto_venue.conf -d}"
STOP_CMD="${4:-pkill -f mosquitto-venue}"

state=$SEED

# Mutates the global $state and sets $RANGE_RESULT. Not called via $(...):
# that would run in a subshell and the update to $state would be lost.
range() {
    local lo=$1 hi=$2
    state=$(( (1103515245 * state + 12345) % 2147483648 ))
    RANGE_RESULT=$(( lo + state % (hi - lo + 1) ))
}

log() {
    echo "$(date '+%Y-%m-%d %H:%M:%S.%3N') [venue-net] $*"
}

log "starting venue network simulation: seed=$SEED cycles=$CYCLES"
eval "$START_CMD"
log "broker reachable"

i=0
while :; do
    i=$((i + 1))
    if [ "$CYCLES" -gt 0 ] && [ "$i" -gt "$CYCLES" ]; then
        break
    fi

    range 5 20; up_s=$RANGE_RESULT
    range 3 25; down_s=$RANGE_RESULT

    if [ "$CYCLES" -gt 0 ]; then
        label="$i/$CYCLES"
    else
        label="$i"
    fi

    sleep "$up_s"
    log "cycle $label: broker unreachable (was reachable ${up_s}s)"
    eval "$STOP_CMD"

    sleep "$down_s"
    log "cycle $label: broker reachable again (was unreachable ${down_s}s)"
    eval "$START_CMD"
done

log "simulation finished, broker left reachable"
