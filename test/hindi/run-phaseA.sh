#!/usr/bin/env bash
# Phase A simulator suite: Hindi screenshots + pixel regression against stock.
set -u
H=$(cd "$(dirname "$0")" && pwd)
REPO=$(cd "$H/../.." && pwd)
BOOKS=$H/books
NEW=${NEW:-$REPO/.pio/build/x4-pro-simulator/program}  # build to test (an SD-font build: feat/hindi-reading)
OLD=${OLD:?set OLD to a stock CrossInk x4-pro-simulator program for the regression}
REG=${REG:-$H/regression}
EP=$REPO/test/epubs
S=$H/shots; cd "$H"
IN="1500:CONFIRM;3000:CONFIRM;4500:CONFIRM;9000:DOWN;12000:DOWN;15000:BACK;17000:QUIT"
SH="4300:books;8500:p1;11500:p2;14500:p3;16500:home"
run(){ SDFONTS=$H/fonts-sd ./sim2.sh $NEW "$S/$1" "$2" "$3" "{\"hyphenationEnabled\":1,\"fontFamily\":1,\"language\":\"EN\",\"sdFontFamilyName\":\"$4\"}" "$IN" "$SH"; }
run 03-bitter-hi $BOOKS/premchand-hi.epub "पंच परमेश्वर.epub" "Bitter Hindi"
run 04-lexend-hi $BOOKS/premchand-hi.epub "पंच परमेश्वर.epub" "Lexend Hindi"
run 05-bitter-en-tagged $BOOKS/premchand-en.epub "panch-parmeshwar-en.epub" "Bitter Hindi"
run 06-lexend-untagged $BOOKS/premchand-nolang.epub "पूस की रात.epub" "Lexend Hindi"
SDFONTS=$H/fonts-sd ./sim2.sh $NEW "$S/07-ui-bitter" $BOOKS/premchand-hi.epub "पंच परमेश्वर — प्रेमचंद की कहानी.epub" '{"fontFamily":1,"language":"EN","sdFontFamilyName":"Bitter Hindi"}' "1500:CONFIRM;3000:CONFIRM;4500:CONFIRM;8000:BACK;11000:QUIT" "4300:books;10000:home"
grep -lE "Guru Meditation|abort\(|Segmentation|assert" "$S"/0[3-7]*.log && echo "CRASH MARKERS FOUND"
# regression
mkdir -p "$REG"; fails=0; n=0
IN2="1500:CONFIRM;3000:CONFIRM;4500:CONFIRM;9000:DOWN;12000:DOWN;15000:BACK;17500:QUIT"; SH2="1200:boot;4300:books;8500:p1;11500:p2;14500:p3;17000:home"
for b in test_kerning_ligature test_reader_rendering_matrix font-prewarm-benchmark; do for f in 0 1; do
  for v in OLD NEW; do eval prog=\$$v; ./sim2.sh $prog "$REG/$b-f$f-$v" "$EP/$b.epub" "$b.epub" "{\"hyphenationEnabled\":1,\"fontFamily\":$f,\"language\":\"EN\"}" "$IN2" "$SH2"; done
  for s in boot books p1 p2 p3 home; do n=$((n+1)); d=$(magick compare -metric AE "$REG/$b-f$f-OLD-$s.png" "$REG/$b-f$f-NEW-$s.png" null: 2>&1 | cut -d' ' -f1); [ "$d" != "0" ] && { echo "DIFF $b f$f $s $d"; fails=$((fails+1)); }; done
done; done
echo "regression: $n comparisons, $fails differ"
