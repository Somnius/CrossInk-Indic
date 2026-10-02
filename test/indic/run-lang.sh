#!/usr/bin/env bash
# Screenshot one CrossInk-Indic variant in the simulator.
#
#   run-lang.sh <program> <language> <ui-code> <out-dir>
#   QUICK=1 run-lang.sh ...   only the first book pages and Settings (other device profiles)
#
# <language> picks books/<language>.epub; <ui-code> is the menu language
# ("TA", "MAR", ... or "EN" for a reading-only build, which skips the menu shots).
# Everything comes from the firmware's own fonts (no /.fonts on the card).
set -u
H=$(cd "$(dirname "$0")" && pwd)
P=$1; L=$2; CODE=$3; S=$4
SIM2=$H/../hindi/sim2.sh
BOOK=$H/books/$L.epub
mkdir -p "$S"
B="\"hyphenationEnabled\":1,\"language\":\"$CODE\""
OPEN="1500:CONFIRM;3000:CONFIRM;4500:CONFIRM"
n(){ "$SIM2" "$P" "$S/$1" "$BOOK" "$L.epub" "$2" "$3" "$4"; }
n L01-bitter "{$B,\"fontFamily\":1}" "$OPEN;9000:DOWN;11000:DOWN;13000:DOWN;15000:BACK;17000:QUIT" \
  "1200:home;4300:files;8500:p1;10500:p2;12500:p3;14500:p4;16500:home-book"
if [ -z "${QUICK:-}" ]; then
n L02-lexend "{$B,\"fontFamily\":0}" "$OPEN;9000:DOWN;11000:DOWN;13000:QUIT" "8500:p1;12500:p3"
n L03-size16 "{$B,\"fontFamily\":1,\"fontSize\":16}" "$OPEN;9000:DOWN;11000:DOWN;13000:QUIT" "8500:p1;12500:p3"
n L04-size10 "{$B,\"fontFamily\":0,\"fontSize\":10}" "$OPEN;9000:DOWN;11000:DOWN;13000:QUIT" "8500:p1;12500:p3"
fi
if [ "$CODE" != EN ]; then
  n L05-settings "{$B,\"fontFamily\":1}" \
    "1500:DOWN;2000:DOWN;2500:DOWN;3000:CONFIRM;4500:CONFIRM;6000:CONFIRM;7500:CONFIRM;9000:DOWN;9500:CONFIRM;11500:QUIT" \
    "4300:display;5800:reader;7300:controls;8800:system;11000:device"
  [ -n "${QUICK:-}" ] || n L06-reader-menu "{$B,\"fontFamily\":1}" "$OPEN;8000:CONFIRM;10000:QUIT" "9500:menu"
  [ -n "${QUICK:-}" ] || n L07-transfer "{$B,\"fontFamily\":1}" "1500:DOWN;2000:DOWN;2500:CONFIRM;5000:QUIT" "4500:screen"
fi
grep -lE "Guru Meditation|abort\(|Segmentation|ERROR.*[Ss]hap" "$S"/L*.log && echo "CRASH/SHAPING MARKERS FOUND" || echo "no crash markers"
ls "$S"/L*.png | wc -l
