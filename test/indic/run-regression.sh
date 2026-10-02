#!/usr/bin/env bash
# English pixel regression: the same English test books on stock CrossInk and
# on a CrossInk-Indic build, compared screen by screen.
#   run-regression.sh <stock-program> <indic-program> <out-dir>
set -u
H=$(cd "$(dirname "$0")" && pwd)
EP=$H/../epubs
SIM2=$H/../hindi/sim2.sh
OLD=$1; NEW=$2; REG=$3
mkdir -p "$REG"; fails=0; n=0
IN="1500:CONFIRM;3000:CONFIRM;4500:CONFIRM;9000:DOWN;12000:DOWN;15000:BACK;17500:QUIT"
SH="1200:boot;4300:books;8500:p1;11500:p2;14500:p3;17000:home"
for b in test_kerning_ligature test_reader_rendering_matrix font-prewarm-benchmark; do for f in 0 1; do
  for v in OLD NEW; do eval prog=\$$v
    "$SIM2" "$prog" "$REG/$b-f$f-$v" "$EP/$b.epub" "$b.epub" "{\"hyphenationEnabled\":1,\"fontFamily\":$f,\"language\":\"EN\"}" "$IN" "$SH"
  done
  for s in boot books p1 p2 p3 home; do
    n=$((n+1))
    d=$(magick compare -metric AE "$REG/$b-f$f-OLD-$s.png" "$REG/$b-f$f-NEW-$s.png" null: 2>&1 | cut -d' ' -f1)
    [ "$d" != "0" ] && { echo "DIFF $b f$f $s $d"; fails=$((fails+1)); }
  done
done; done
echo "regression: $n comparisons, $fails differ"
