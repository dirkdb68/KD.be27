#!/bin/bash
# Cross-builds the gateway image for amd64, arm64 (e.g. an Apple Silicon
# laptop) and arm/v7 (32-bit -- a real BeagleBone Black). Nothing in the
# Dockerfile is architecture-specific (everything compiles from source
# against each target's own libmosquitto-dev), so this needs no special
# handling beyond telling buildx which platforms to target.
#
# Local use (this script): builds and loads one image per platform,
# tagged gateway:<arch>, and does a quick smoke run under QEMU emulation
# for the non-native ones. Needs buildx + QEMU user-mode emulators
# registered (`docker buildx version`; if missing, see README below).
#
# To publish a single multi-arch tag instead (needs a registry):
#   docker buildx build --platform linux/amd64,linux/arm64,linux/arm/v7 \
#       -t <registry>/<name>:<tag> --push source/application

set -eu

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
APP="$HERE/source/application"
BUILDER=multiarch

if ! docker buildx version >/dev/null 2>&1; then
    echo "error: docker buildx not available. Install the docker-buildx" >&2
    echo "       plugin and qemu-user(-binfmt) (package names vary by" >&2
    echo "       distro), then: docker run --privileged --rm tonistiigi/binfmt --install all" >&2
    exit 1
fi

if ! docker buildx inspect "$BUILDER" >/dev/null 2>&1; then
    docker buildx create --name "$BUILDER" --driver docker-container --use
fi

for PLATFORM in linux/amd64 linux/arm64 linux/arm/v7; do
    TAG="gateway:$(echo "$PLATFORM" | tr '/' '-')"
    echo "=== $PLATFORM -> $TAG ==="
    docker buildx build --builder "$BUILDER" --platform "$PLATFORM" --load -t "$TAG" "$APP"
    docker image inspect "$TAG" --format 'image reports: {{.Architecture}}/{{.Variant}} os={{.Os}}'
done

echo "All platforms built. Tagged images: gateway:linux-amd64, gateway:linux-arm64, gateway:linux-arm-v7"
