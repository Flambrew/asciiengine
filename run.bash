#!/bin/bash

OUT_PATH="logs/log-$(date +"%Y%m%d-%H%M%S")"

OUT_DIR=$(dirname "$OUT_PATH")
if [ ! -d "$OUT_DIR" ]; then
    mkdir -p "$OUT_DIR"
fi

EXEC=0
while [ -f "$OUT_PATH-$EXEC.txt" ]; do
    ((EXEC++))
done
OUT_PATH="$OUT_PATH-$EXEC.txt"

make > "$OUT_PATH"
if [ $? -eq 0 ]; then
    ./out
    make clean > "$OUT_PATH"
else
    echo "Compilation failed."
fi
