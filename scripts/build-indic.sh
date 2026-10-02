#!/usr/bin/env bash
# Build CrossInk-Indic firmware variants.
#
#   scripts/build-indic.sh read <script> [env ...]   English menus, <script> built in
#   scripts/build-indic.sh lang <code>   [env ...]   English + <code> menus, its script built in
#
# <script>: devanagari bengali gurmukhi gujarati oriya tamil telugu kannada malayalam sinhala
# <code>:   hi mr ne bn as pa gu or ta te kn ml si
# envs default to: default x4-pro sticky x4-classic (or a *-simulator env).
# Set CROSSINK_INDIC_VERSION (e.g. 1.6.0-indic.2) to stamp release builds; the
# variant is appended (1.6.0-indic.2-ta, 1.6.0-indic.2-read-tamil) so the
# firmware-<device>-v<version>.bin names end in the variant the Wi-Fi updater
# looks for. Output bins are copied to $OUT (default: build-indic/).
set -euo pipefail
cd "$(dirname "$0")/.."

declare -A LANG_SCRIPT=(
  [hi]=devanagari [mr]=devanagari [ne]=devanagari [bn]=bengali [as]=bengali [pa]=gurmukhi
  [gu]=gujarati [or]=oriya [ta]=tamil [te]=telugu [kn]=kannada [ml]=malayalam [si]=sinhala
)
# Translation codes that differ from the build code (MR is an Xtensa register
# macro, SI was taken).
declare -A I18N_CODE=([mr]=mar [si]=sin)
mode=${1:?usage: build-indic.sh read <script>|lang <code> [env ...]}
what=${2:?missing script or language code}
shift 2
envs=("$@")
[[ ${#envs[@]} -eq 0 ]] && envs=(default x4-pro sticky x4-classic)

case "$mode" in
  read) script=$what; variant="read-$script"; langs=en
        case " devanagari bengali gurmukhi gujarati oriya tamil telugu kannada malayalam sinhala " in
          *" $script "*) ;;
          *) echo "unknown script $script" >&2; exit 2 ;;
        esac ;;
  lang) script=${LANG_SCRIPT[$what]:?unknown language code $what}; variant=$what; langs="en,${I18N_CODE[$what]:-$what}" ;;
  *) echo "mode must be read or lang" >&2; exit 2 ;;
esac
SCRIPT_MACRO="CROSSINK_SCRIPT_$(echo "$script" | tr '[:lower:]' '[:upper:]')"

export CROSSINK_I18N_LANGS=$langs
export PLATFORMIO_BUILD_FLAGS="-D$SCRIPT_MACRO -DCROSSINK_FIRMWARE_VARIANT=\\\"$variant\\\""
if [[ -n "${CROSSINK_INDIC_VERSION:-}" ]]; then
  export CROSSINK_RELEASE_VERSION="${CROSSINK_INDIC_VERSION#v}-$variant"
fi
OUT=${OUT:-build-indic}
mkdir -p "$OUT"
PIO=${PIO:-pio}
# gen_i18n.py writes lib/I18n/I18nStrings.* into the source tree, so two
# variant builds from one checkout must not overlap.
mkdir -p .pio
exec 9> .pio/build-indic.lock
flock 9

declare -A DEVICE=([default]=x3-x4 [x4-pro]=x4-pro [sticky]=sticky [x4-classic]=x4-classic)
for env in "${envs[@]}"; do
  echo "=== $env: script=$script langs=$langs variant=$variant"
  rm -f .pio/build/"$env"/firmware-*.bin
  nice -n 10 "$PIO" run -e "$env" -j "${JOBS:-12}"
  if [[ "$env" != *simulator* ]]; then
    dev=${DEVICE[$env]:?no device name for env $env}
    if [[ -n "${CROSSINK_RELEASE_VERSION:-}" ]]; then
      name="firmware-$dev-v$CROSSINK_RELEASE_VERSION.bin"
      cp .pio/build/"$env"/"$name" "$OUT/$name"
    else
      cp .pio/build/"$env"/firmware.bin "$OUT/firmware-$dev-$variant.bin"
    fi
    echo "$env $variant $(wc -c < .pio/build/"$env"/firmware.bin)" >> "$OUT/sizes.txt"
  fi
done
