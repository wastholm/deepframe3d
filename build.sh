#!/bin/sh
set -e
mkdir -p build
cd build
cmake .. "$@"
make -j$(nproc 2>/dev/null || echo 1)
