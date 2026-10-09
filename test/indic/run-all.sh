#!/usr/bin/env bash
# Build every CrossInk-Indic variant for the simulator and screenshot it.
#   run-all.sh <out-dir> [full|read|all]
# Full-menu builds: whole suite on the X4 Pro profile, book + Settings on the
# others. Reading-only builds: book pages on the X4 Pro and X3 profiles.
# With STOCK=<stock CrossInk x4-pro-simulator program>, English screens of the
# reading-only builds (and of the hi/ta builds) are compared with stock pixel
# by pixel. FROM=<code> resumes the full-menu builds at that language.
set -u
H=$(cd "$(dirname "$0")" && pwd)
REPO=$(cd "$H/../.." && pwd)
OUT=$(mkdir -p "${1:?out dir}" && cd "$1" && pwd); WHAT=${2:-all}
LANGS="hi:hindi:HI mr:marathi:MAR ne:nepali:NE bn:bengali:BN as:assamese:AS pa:punjabi:PA gu:gujarati:GU or:odia:OR ta:tamil:TA te:telugu:TE kn:kannada:KN ml:malayalam:ML si:sinhala:SIN"
READS="devanagari:hindi bengali:bengali gurmukhi:punjabi gujarati:gujarati oriya:odia tamil:tamil telugu:telugu kannada:kannada malayalam:malayalam sinhala:sinhala"
build(){
  local log="$OUT/build-${2}-${3}.log"
  (cd "$REPO" && OUT=$OUT/bins scripts/build-indic.sh "$@" > "$log" 2>&1) || { echo "BUILD FAILED: $*"; tail -20 "$log"; return 1; }
}
if [ "$WHAT" != read ]; then
  started=${FROM:+no}
  for x in $LANGS; do IFS=: read -r code name ui <<< "$x"
    [ "${started:-yes}" = no ] && { [ "$code" = "$FROM" ] && started=yes || continue; }
    for env in x4-pro-simulator simulator simulator-X3 sticky-simulator x4-classic-simulator; do
      build lang "$code" "$env" || continue
      q=1; [ "$env" = x4-pro-simulator ] && q=
      echo "== $code $env: $(QUICK=$q "$H/run-lang.sh" "$REPO/.pio/build/$env/program" "$name" "$ui" "$OUT/$code/$env" | tr '\n' ' ')"
      if [ -n "${STOCK:-}" ] && [ "$env" = x4-pro-simulator ] && [[ " hi ta " == *" $code "* ]]; then
        echo "== $code english $("$H/run-regression.sh" "$STOCK" "$REPO/.pio/build/$env/program" "$OUT/$code/regression" | tail -1)"
      fi
    done
  done
fi
if [ "$WHAT" != full ]; then
  for x in $READS; do IFS=: read -r script name <<< "$x"
    for env in x4-pro-simulator simulator-X3; do
      build read "$script" "$env" || continue
      echo "== read-$script $env: $("$H/run-lang.sh" "$REPO/.pio/build/$env/program" "$name" EN "$OUT/read-$script/$env" | tr '\n' ' ')"
      if [ -n "${STOCK:-}" ] && [ "$env" = x4-pro-simulator ]; then
        echo "== read-$script english $("$H/run-regression.sh" "$STOCK" "$REPO/.pio/build/$env/program" "$OUT/read-$script/regression" | tail -1)"
      fi
    done
  done
fi
