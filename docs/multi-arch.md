# Multi-arch builds

`source/application/Dockerfile` builds entirely from source against
each target's own `libmosquitto-dev` -- nothing downloads an
architecture-specific prebuilt binary -- so it needs no special
handling to cross-build. Verified (2026-10-30) via `docker buildx` with
QEMU user-mode emulation registered (`tonistiigi/binfmt`):

| Platform | Compiles | Runs under emulation |
|---|---|---|
| `linux/amd64` | yes | yes (native) |
| `linux/arm64` | yes | yes (e.g. an Apple Silicon Mac) |
| `linux/arm/v7` | yes | yes (32-bit -- a real BeagleBone Black) |

All three behave identically: connect, publish, and react the same way
when the broker goes away.

`docker compose up` already builds for whatever architecture you're
running it on -- no extra steps needed for the normal challenge path.
`scripts/build_multiarch.sh` is only for cross-building an image for a
*different* architecture than your dev machine (e.g. building on a
laptop to run on real RPi/BBB hardware). It needs the `docker-buildx`
plugin and `qemu-user`/`qemu-user-binfmt` (package names vary by
distro); if `docker buildx version` fails, install those first and
register the emulators once with:

```
docker run --privileged --rm tonistiigi/binfmt --install all
```
