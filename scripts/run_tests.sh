#!/usr/bin/env bash
set -euo pipefail
cmake --build build -j"$(nproc)"
ctest --test-dir build --output-on-failure
