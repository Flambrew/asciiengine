#!/bin/bash

OUT_PATH="logs/$(date +"%Y%m%d-%H%M%S")"

OUT_DIR=$(dirname "$OUT_PATH")
if [ ! -d "$OUT_DIR" ]; then
    mkdir -p "$OUT_DIR"
fi

EXEC=0
while [ -f "$OUT_PATH-$EXEC.log" ]; do
    ((EXEC++))
done
OUT_PATH="$OUT_PATH-$EXEC.log"

make >> "$OUT_PATH" 2>&1
if [ $? -eq 0 ]; then
    echo "//----====<OUTPUT>====----\\\\" >> "$OUT_PATH"
    if [ "$1" = "gdb" ]; then
        gdb ./out | tee -a "$OUT_PATH"
    else
        ./out | tee -a "$OUT_PATH"
    fi
    echo "\\\\----====<OUTPUT>====----//" >> "$OUT_PATH"
    make clean >> "$OUT_PATH" 2>&1
else
    echo "compilation failed." | tee -a "$OUT_PATH"
fi
