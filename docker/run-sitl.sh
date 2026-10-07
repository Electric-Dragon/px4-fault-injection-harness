#!/usr/bin/env bash
set -euo pipefail

# Usage: ./docker/run-sitl.sh [px4-ref] [airframe]
# Defaults: main, jmavsim

PX4_REF="${1:-main}"
AIRFRAME="${2:-jmavsim}"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# Detect host LAN IP (not host.docker.internal — can resolve IPv6-only)
HOST_IP="$(ipconfig getifaddr en0 2>/dev/null || hostname -I 2>/dev/null | awk '{print $1}')"

echo "Starting PX4 SITL: ref=${PX4_REF} airframe=${AIRFRAME} host=${HOST_IP}"

docker run --rm -it \
    -e HEADLESS=1 \
    -e PX4_SIM_HOST_ADDR="${HOST_IP}" \
    -p 14556:14556/udp \
    px4io/px4-dev-simulation-jammy \
    bash -c "
        git clone --depth 1 --branch ${PX4_REF} https://github.com/PX4/PX4-Autopilot.git /src/PX4 && \
        cd /src/PX4 && \
        make px4_sitl ${AIRFRAME}
    "
