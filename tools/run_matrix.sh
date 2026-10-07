#!/usr/bin/env bash
set -euo pipefail

# Run all specs and collect results.
# Exit code = number of failed specs.

HARNESS="${1:-./build/harness}"
SPECS_DIR="${2:-specs/examples}"
OUT_DIR="${3:-reports/}"

"${HARNESS}" run --specs "${SPECS_DIR}" --out "${OUT_DIR}"
