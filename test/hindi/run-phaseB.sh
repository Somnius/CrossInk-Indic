#!/usr/bin/env bash
# Phase B (Hindi UI) simulator screenshots.
set -u
H=$(cd "$(dirname "$0")" && pwd)
REPO=$(cd "$H/../.." && pwd)
BOOKS=$H/books; cd "$H"
P=${P:-$REPO/.pio/build/x4-pro-simulator/program}  # an SD-font Hindi UI build (feat/hindi)
S=$H/shotsB; mkdir -p "$S"
HI='{"hyphenationEnabled":1,"fontFamily":1,"language":"HI","sdFontFamilyName":"Bitter Hindi"}'
LX='{"hyphenationEnabled":1,"fontFamily":0,"language":"HI","sdFontFamilyName":"Lexend Hindi"}'
TOSYS="1500:DOWN;2000:DOWN;2500:DOWN;3000:CONFIRM;4500:CONFIRM;5000:CONFIRM;5500:CONFIRM"
TODEV="$TOSYS;6500:DOWN;7000:CONFIRM"
TOLANG="$TODEV;8500:DOWN;9000:DOWN;9500:DOWN;10000:CONFIRM"
f(){ SDFONTS=$H/fonts-sd ./sim2.sh "$P" "$S/$1" $BOOKS/premchand-hi.epub "$2" "$3" "$4" "$5"; }
n(){ ./sim2.sh "$P" "$S/$1" $BOOKS/premchand-hi.epub "$2" "$3" "$4" "$5"; }
f B01-home "पंच परमेश्वर.epub" "$HI" "1500:CONFIRM;3000:CONFIRM;4500:CONFIRM;8000:BACK;10000:QUIT" "1200:empty;4300:files;9500:book"
f B02-settings "a.epub" "$HI" "1500:DOWN;2000:DOWN;2500:DOWN;3000:CONFIRM;4500:CONFIRM;6000:CONFIRM;7500:CONFIRM;9000:DOWN;9500:DOWN;10000:DOWN;10500:CONFIRM;12500:QUIT" "4300:display;5800:reader;7300:controls;8800:system;12000:popup"
f B03-device "a.epub" "$HI" "$TODEV;9000:QUIT" "8500:page"
f B04-picker "a.epub" "$HI" "$TOLANG;12000:QUIT" "11500:list"
f B05-reader-menu "पंच परमेश्वर.epub" "$HI" "1500:CONFIRM;3000:CONFIRM;4500:CONFIRM;8000:CONFIRM;10000:QUIT" "9500:menu"
f B06-lexend "पंच परमेश्वर.epub" "$LX" "1500:DOWN;2000:DOWN;2500:DOWN;3000:CONFIRM;4500:CONFIRM;6000:QUIT" "1200:home;5500:reader-settings"
n B07-nofont "a.epub" '{"fontFamily":1,"language":"EN"}' "$TOLANG;12000:UP;14000:CONFIRM;17000:BACK;19000:QUIT" "13500:picker;16500:alert;18500:settings"
grep -lE "Guru Meditation|abort\(|Segmentation" "$S"/B*.log && echo "CRASH MARKERS FOUND" || echo "no crash markers"
ls "$S"/B*.png | wc -l
