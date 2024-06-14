#!/bin/bash

OUT_PATH="logs/$(date +"%Y%m%d-%H%M%S")"
OUT_MAIN="logs/latest.log"
> "$OUT_MAIN"

OUT_DIR=$(dirname "$OUT_MAIN")
if [ ! -d "$OUT_DIR" ]; then
    mkdir -p "$OUT_DIR"
fi

EXEC=0
while [ -f "$OUT_PATH-$EXEC.log" ]; do
    ((EXEC++))
done
OUT_PATH="$OUT_PATH-$EXEC.log"

make >> "$OUT_MAIN" 2>&1
if [ $? -eq 0 ]; then
    echo "//----====<OUTPUT>====----\\\\" >> "$OUT_MAIN"
    if [ "$1" = "-d" ] || [ "$1" = "-debug" ] || [ "$2" = "-d" ] || [ "$2" = "-debug" ]; then
        gdb ./out | tee -a "$OUT_MAIN"
    else
        ./out | tee -a "$OUT_MAIN"
    fi
    echo "\\\\----====<OUTPUT>====----//" >> "$OUT_MAIN"
    make clean >> "$OUT_MAIN" 2>&1
else
    echo "compilation failed." | tee -a "$OUT_MAIN"
fi

if [ "$1" = "-l" ] || [ "$1" = "-log" ] || [ "$2" = "-l" ] || [ "$2" = "-log" ]; then
    cp "$OUT_MAIN" "$OUT_PATH"
fi