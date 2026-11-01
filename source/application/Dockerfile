# Apex Raceway Trackside Telemetry Gateway v3.4 -- multi-stage build.
#
# Build stage has the full toolchain (gcc, cmake, libmosquitto-dev).
# Runtime stage only ships libmosquitto1 and the compiled binary, so the
# image you actually run is small and doesn't leak build tools into the
# container participants poke around in.

FROM debian:12-slim AS build

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential cmake libmosquitto-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY CMakeLists.txt ./
COPY include ./include
COPY src ./src

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    && cmake --build build -j"$(nproc)"

# ---------------------------------------------------------------------------

FROM debian:12-slim AS runtime

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        libmosquitto1 gdb strace \
    && rm -rf /var/lib/apt/lists/*

COPY --from=build /src/build/gateway /usr/local/bin/gateway
RUN mkdir -p /etc/gateway /var/log/gateway

ENTRYPOINT ["/usr/local/bin/gateway"]
CMD ["/etc/gateway/gateway.conf"]
