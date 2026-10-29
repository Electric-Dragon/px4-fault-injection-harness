#!/usr/bin/env bash
set -euo pipefail

# Usage: ./docker/run-sitl.sh [px4-tag-or-sha] [sih-target]
# Defaults: v1.16.1, sihsim_quadx (use sihsim_airplane for fixed-wing).
# Only pins are accepted: a release tag or a full 40-char commit SHA. Branch names are
# refused — a branch is not a pin (jmavsim vanished from `main` between 2026-07 and 2026-10).
#
# Env overrides:
#   PX4_IMAGE      container image (default: px4io/px4-dev-simulation-jammy)
#   HARNESS_PORT   host UDP port the harness listens on (default: 14556 → udpin://:14556)
#
# The PX4 checkout and build live in a named volume per ref (px4-<ref>), so only the first
# run per ref clones and builds from scratch. Force a clean rebuild: docker volume rm px4-<ref>

PX4_REF="${1:-v1.16.1}"
SIM_TARGET="${2:-sihsim_quadx}"
PX4_IMAGE="${PX4_IMAGE:-px4io/px4-dev-simulation-jammy}"
HARNESS_PORT="${HARNESS_PORT:-14556}"
PX4_REPO="https://github.com/PX4/PX4-Autopilot.git"

# Host LAN IPv4 (not host.docker.internal — it can resolve IPv6-only)
HOST_IP="$(ipconfig getifaddr en0 2>/dev/null || hostname -I 2>/dev/null | awk '{print $1}' || true)"
if [[ -z "${HOST_IP}" ]]; then
    echo "error: could not detect host LAN IPv4 (tried en0). Is the Mac on a network?" >&2
    exit 1
fi

if [[ "${PX4_REF}" =~ ^[0-9a-f]{40}$ ]]; then
    REF_KIND="sha"
    echo "warning: SHA pin has no tag, so PX4 will report version v0.0.0 — record the SHA instead." >&2
elif git ls-remote --exit-code --tags "${PX4_REPO}" "refs/tags/${PX4_REF}" >/dev/null 2>&1; then
    REF_KIND="tag"
else
    echo "error: '${PX4_REF}' is not a PX4 release tag or a full commit SHA. Branches are not pins." >&2
    exit 1
fi

PX4_VOLUME="px4-${PX4_REF}"

echo "Starting PX4 SITL (SIH): ref=${PX4_REF} target=${SIM_TARGET} image=${PX4_IMAGE} volume=${PX4_VOLUME}"
echo "MAVLink → ${HOST_IP}:${HARNESS_PORT}   (harness: --url udpin://:${HARNESS_PORT})"

docker run --rm -it \
    -e PX4_REF="${PX4_REF}" \
    -e REF_KIND="${REF_KIND}" \
    -e PX4_REPO="${PX4_REPO}" \
    -e SIM_TARGET="${SIM_TARGET}" \
    -e HOST_IP="${HOST_IP}" \
    -e HARNESS_PORT="${HARNESS_PORT}" \
    -v "${PX4_VOLUME}:/src/PX4" \
    "${PX4_IMAGE}" \
    bash -c '
        set -euo pipefail

        # The marker is written only after a complete clone. A clone that died halfway
        # leaves a .git but no marker, so the volume is wiped and cloned again.
        MARKER=/src/PX4/.harness-clone-complete
        if [ -f "${MARKER}" ]; then
            echo "Reusing PX4 checkout in volume (incremental build)"
        else
            find /src/PX4 -mindepth 1 -delete
            # Clone the tag itself (not a branch) so `git describe` finds it and PX4 reports
            # its real version — reports embed it.
            if [ "${REF_KIND}" = tag ]; then
                git clone --depth 1 --branch "${PX4_REF}" "${PX4_REPO}" /src/PX4
            else
                git init -q /src/PX4
                git -C /src/PX4 remote add origin "${PX4_REPO}"
                git -C /src/PX4 fetch --depth 1 origin "${PX4_REF}"
                git -C /src/PX4 checkout -q FETCH_HEAD
            fi
            touch "${MARKER}"
        fi
        cd /src/PX4

        # SITL MAVLink defaults to the container'"'"'s localhost. Once PX4 is up, add one
        # instance aimed at the harness on the host.
        (
            until build/px4_sitl_default/bin/px4-mavlink status >/dev/null 2>&1; do sleep 2; done
            build/px4_sitl_default/bin/px4-mavlink start -x -u 14558 -r 4000000 -m onboard \
                -t "${HOST_IP}" -o "${HARNESS_PORT}"
        ) &

        make px4_sitl "${SIM_TARGET}"
    '
