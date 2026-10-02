#!/usr/bin/env bash
# sim2.sh <program> <out-prefix> <epub> <book-filename> <settings-json> <input-script> <shots "ms:name;...">
# Shot names are suffixes: "1200:home" writes <out-prefix>-home.png.
set -e
PROG=$1; OUT=$2; BOOK=$3; NAME=$4; SETTINGS=$5; INPUT=$6; SHOTS=$7
D=$(mktemp -d); mkdir -p "$D/fs_/books" "$D/fs_/.crosspoint"; cp "$BOOK" "$D/fs_/books/$NAME"
echo "$SETTINGS" > "$D/fs_/.crosspoint/crossink-settings.json"
if [ -d "${SDFONTS:-/nonexistent}" ]; then mkdir -p "$D/fs_/.fonts" && cp -r "$SDFONTS"/* "$D/fs_/.fonts/"; fi
S=$(echo "$SHOTS" | tr ';' '\n' | sed -E "s|^([0-9]+):(.*)$|\1:${OUT}-\2.bmp|" | paste -sd';')
cd "$D"
SDL_VIDEODRIVER=dummy CROSSPOINT_SIM_SCREENSHOTS="$S" CROSSPOINT_SIM_INPUT_SCRIPT="$INPUT" timeout 60 "$PROG" > "${OUT}.log" 2>&1 || true
for f in "${OUT}"-*.bmp; do [ -f "$f" ] && magick "$f" "${f%.bmp}.png" && rm "$f"; done
cd /; rm -r "$D"
