#!/bin/bash
# CrossInk-Indic: built-in fonts for the Indic scripts (and Sinhala, Myanmar).
#
# For each script, from Noto Sans <Script> regular/bold:
#   <script>_layout.h                 the layout font the shaper reads in place
#                                     from flash (shared by every size and by
#                                     regular/bold: same glyph order and GSUB)
#   <script>_<size>_{regular,bold}.h  reading fallback fonts, 10-16 pt, 2-bit,
#                                     compressed in groups of at most 16 KB
#   <script>_ui_<size>.h              UI fallback fonts, 8/10/12 pt, 1-bit,
#                                     uncompressed (menus never inflate glyphs)
# The glyphs the shaper emits (conjuncts, half forms, reph, ...) are added by
# glyph ID (--shaping-font). A firmware build compiles one script in
# (CROSSINK_SCRIPT_<NAME>, see builtinFonts/indic_script.h).
#
# usage: convert-indic-fonts.sh [script ...]   (default: all)
set -e
cd "$(dirname "$0")"

# name:FontDir:first codepoint of the script's 128-codepoint block[:last codepoint]
ALL_SCRIPTS=(
  devanagari:NotoSansDevanagari:0x0900
  bengali:NotoSansBengali:0x0980
  gurmukhi:NotoSansGurmukhi:0x0A00
  gujarati:NotoSansGujarati:0x0A80
  oriya:NotoSansOriya:0x0B00
  tamil:NotoSansTamil:0x0B80
  telugu:NotoSansTelugu:0x0C00
  kannada:NotoSansKannada:0x0C80
  malayalam:NotoSansMalayalam:0x0D00
  sinhala:NotoSansSinhala:0x0D80
  myanmar:NotoSansMyanmar:0x1000:0x109F
)
READING_SIZES=(10 12 14 16)
UI_SIZES=(8 10 12)
READING_RENDER_ARGS=(--2bit --compress --pnum --darken-aa --zopfli --max-group-bytes 16384)
OUT=../builtinFonts

wanted=("$@")
for w in "${wanted[@]}"; do
  [[ " ${ALL_SCRIPTS[*]} " =~ " $w:" ]] || { echo "unknown script $w" >&2; exit 2; }
done
# Write each header through a temp file, so a failed run never leaves a truncated one.
gen() { local out=$1; shift; "$@" > "$out.tmp" && mv "$out.tmp" "$out"; }
for entry in "${ALL_SCRIPTS[@]}"; do
  IFS=: read -r name dir first last <<< "$entry"
  if [[ ${#wanted[@]} -gt 0 && ! " ${wanted[*]} " =~ " ${name} " ]]; then continue; fi
  src="../builtinFonts/source/$dir"
  regular="$src/$dir-Regular.ttf"
  [[ -n "$last" ]] || last=$(printf '0x%04X' $(( first + 0x7F )))
  # The script's block, the dandas several scripts share, ZWNJ/ZWJ (so text
  # drawn unshaped still finds them), the dotted circle the shaper shows for a
  # stray sign.
  ranges=("$first,$last" 0x0964,0x0965 0x200C,0x200D 0x25CC,0x25CC)
  range_args=()
  for r in "${ranges[@]}"; do range_args+=(--additional-intervals "$r"); done
  symbol="${name}Layout"
  shaping_args=(--shaping-scripts "$name" --shaping-layout-symbol "$symbol"
                --shaping-layout-header "${name}_layout.h" --shaping-layout-font "$regular")

  gen "$OUT/${name}_layout.h" python gen_builtin_shaping_layout.py "$regular" "$symbol" --scripts "$name"
  echo "Generated $OUT/${name}_layout.h"

  for size in "${READING_SIZES[@]}"; do
    for style in Regular Bold; do
      lower=$(echo "$style" | tr '[:upper:]' '[:lower:]')
      face="$src/$dir-$style.ttf"
      gen "$OUT/${name}_${size}_${lower}.h" python fontconvert.py "${name}_${size}_${lower}" "$size" "$face" \
        --no-default-intervals "${range_args[@]}" --shaping-font "$face" "${shaping_args[@]}" "${READING_RENDER_ARGS[@]}"
      echo "Generated $OUT/${name}_${size}_${lower}.h"
    done
  done

  for size in "${UI_SIZES[@]}"; do
    gen "$OUT/${name}_ui_${size}.h" python fontconvert.py "${name}_ui_${size}" "$size" "$regular" \
      --no-default-intervals "${range_args[@]}" --shaping-font "$regular" "${shaping_args[@]}"
    echo "Generated $OUT/${name}_ui_${size}.h"
  done
done
