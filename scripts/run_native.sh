#!/bin/bash
# Native fallback for anyone who can't or doesn't want to use Docker.
# Needs: gcc, cmake, libmosquitto-dev, and a mosquitto broker reachable
# at the host/port below (your own, or `mosquitto -v` in another shell).
#
# Usage: scripts/run_native.sh [broker_host] [broker_port]

set -eu

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
APP="$HERE/source/application"
BROKER_HOST="${1:-127.0.0.1}"
BROKER_PORT="${2:-1883}"

for cmd in gcc cmake; do
    command -v "$cmd" >/dev/null 2>&1 || {
        echo "error: $cmd not found. Install build-essential/cmake (and libmosquitto-dev) first." >&2
        exit 1
    }
done

mkdir -p "$APP/build"
cmake -S "$APP" -B "$APP/build" -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build "$APP/build" -j"$(nproc 2>/dev/null || echo 2)"

CONF="$(mktemp)"
trap 'rm -f "$CONF"' EXIT
cat > "$CONF" <<EOF
broker_host=$BROKER_HOST
broker_port=$BROKER_PORT
EOF

echo "Starting against broker $BROKER_HOST:$BROKER_PORT (make sure one is running)."
exec "$APP/build/gateway" "$CONF"
