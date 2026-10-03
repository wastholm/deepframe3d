#!/bin/bash

set -eu

for s in 16 22 24 32 48 64 128 256; do
    convert -scale ${s}x${s} icons/deepframe3d.svg icons/deepframe3d-${s}.png
done
