#!/usr/bin/env bash
set -euo pipefail
cmake --build build -j"$(nproc)"
CTEST_BIN="${CTEST_BIN:-/usr/bin/ctest}"
if [[ ! -x "${CTEST_BIN}" ]]; then
    CTEST_BIN="ctest"
fi
"${CTEST_BIN}" --test-dir build --output-on-failure
