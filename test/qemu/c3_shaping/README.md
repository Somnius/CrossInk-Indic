# Shaping on the ESP32-C3, in QEMU

The Xteink X3/X4 have no PSRAM, so complex-script shaping there takes paths
the desktop simulator cannot run. This firmware runs them in Espressif's QEMU
(`-machine esp32c3`, CrossInk's own partition table) and compares the result
with HarfBuzz's output for the Devanagari reference words in
`test/complex_shaper/ExpectedShaping.h`:

1. **Built-in fonts** (`BuiltinShaping`): the layout font is a `const` array
   the CPU reads in place from flash.
2. **SD-card fonts** (`FlashBlobCache`): the layout font is copied once into
   the unused `spiffs` partition and memory-mapped, as `SdCardFont` does. The
   second boot must find and reuse that copy.

Run `./run.sh` (set `QEMU` and `PIO` as needed). Expected output, per boot:

```
built-in devanagari_14_regular: 16/16 words match HarfBuzz
flash-mapped (SD font path): 16/16 words match HarfBuzz
flash-mapped, second shaper: 16/16 words match HarfBuzz
C3 SHAPING: PASS (0 failure(s))
```

The first boot also logs `Copying 96684-byte layout font to flash slot 0`; the
second does not. Heap lines show what shaping costs in internal RAM.

QEMU cannot stand in for the e-paper panel, SD card or buttons, so this checks
the shaping and flash-cache code, not the whole firmware.
