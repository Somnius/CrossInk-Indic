#!/usr/bin/env bash
# Build the C3 shaping test firmware and boot it twice in Espressif's QEMU.
# QEMU: https://github.com/espressif/qemu/releases (qemu-riscv32-softmmu-*);
# set QEMU=/path/to/qemu-system-riscv32 (it needs libslirp at runtime).
set -euo pipefail
cd "$(dirname "$0")"
QEMU=${QEMU:-qemu-system-riscv32}
PIO=${PIO:-pio}
# esptool: PlatformIO's copy unless ESPTOOL is set.
ESPTOOL=${ESPTOOL:-"python3 $(ls -d "${PLATFORMIO_CORE_DIR:-$HOME/.platformio}"/packages/tool-esptoolpy 2>/dev/null)/esptool.py"}
"$PIO" run
B=.pio/build/c3-shaping
$ESPTOOL --chip esp32c3 merge_bin --fill-flash-size 16MB -o "$B/flash.bin" \
  0x0 "$B/bootloader.bin" 0x8000 "$B/partitions.bin" 0x10000 "$B/firmware.bin" >/dev/null
status=0
for boot in 1 2; do
  echo "--- boot $boot"
  # The flash image persists between boots: the second one must reuse the
  # layout font the first copied into the spiffs partition.
  log=$(timeout 240 "$QEMU" -nographic -machine esp32c3 -drive file="$B/flash.bin",if=mtd,format=raw -icount 3 \
    | sed -n '/=== C3/,/C3 SHAPING/p' || true)
  echo "$log"
  grep -q "C3 SHAPING: PASS" <<< "$log" || status=1
done
exit $status
