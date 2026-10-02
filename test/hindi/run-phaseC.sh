#!/usr/bin/env bash
# CrossInk-Indic: everything from the firmware's own fonts (no /.fonts on the card).
set -u
H=$(cd "$(dirname "$0")" && pwd)
REPO=$(cd "$H/../.." && pwd)
BOOKS=$H/books; cd "$H"
P=${P:-$REPO/.pio/build/x4-pro-simulator/program}  # CrossInk-Indic (built-in Hindi)
S=$H/shotsC; mkdir -p "$S"
B='"hyphenationEnabled":1,"language":"HI"'
n(){ ./sim2.sh "$P" "$S/$1" "$2" "$3" "$4" "$5" "$6"; }
n C01-home $BOOKS/premchand-hi.epub "पंच परमेश्वर.epub" "{$B,\"fontFamily\":1}" "1500:CONFIRM;3000:CONFIRM;4500:CONFIRM;9000:DOWN;12000:BACK;14000:QUIT" "1200:empty;4300:files;8500:bitter-p1;11500:bitter-p2;13500:book"
n C02-lexend $BOOKS/premchand-hi.epub "पंच परमेश्वर.epub" "{$B,\"fontFamily\":0}" "1500:CONFIRM;3000:CONFIRM;4500:CONFIRM;9000:DOWN;11000:QUIT" "8500:p1;10500:p2"
n C03-size16 $BOOKS/premchand-hi.epub "पंच परमेश्वर.epub" "{$B,\"fontFamily\":1,\"fontSize\":16}" "1500:CONFIRM;3000:CONFIRM;4500:CONFIRM;9000:DOWN;11000:QUIT" "10500:p2"
n C04-size10 $BOOKS/premchand-hi.epub "पंच परमेश्वर.epub" "{$B,\"fontFamily\":0,\"fontSize\":10}" "1500:CONFIRM;3000:CONFIRM;4500:CONFIRM;9000:DOWN;11000:QUIT" "10500:p2"
n C05-settings $BOOKS/premchand-hi.epub "a.epub" "{$B,\"fontFamily\":1}" "1500:DOWN;2000:DOWN;2500:DOWN;3000:CONFIRM;4500:CONFIRM;6000:CONFIRM;7500:CONFIRM;9000:DOWN;9500:CONFIRM;11500:QUIT" "4300:display;5800:reader;7300:controls;8800:system;11000:device"
n C06-reader-menu $BOOKS/premchand-hi.epub "पंच परमेश्वर.epub" "{$B,\"fontFamily\":1}" "1500:CONFIRM;3000:CONFIRM;4500:CONFIRM;8000:CONFIRM;10000:QUIT" "9500:menu"
n C07-transfer $BOOKS/premchand-hi.epub "a.epub" "{$B,\"fontFamily\":1}" "1500:DOWN;2000:DOWN;2500:CONFIRM;5000:QUIT" "4500:screen"
n C08-styles $BOOKS/hindi-styles.epub "शैली.epub" "{$B,\"fontFamily\":1}" "1500:CONFIRM;3000:CONFIRM;4500:CONFIRM;9000:QUIT" "8500:page"
grep -lE "Guru Meditation|abort\(|Segmentation" "$S"/C*.log && echo "CRASH MARKERS FOUND" || echo "no crash markers"
ls "$S"/C*.png | wc -l
