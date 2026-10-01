# CrossInk-Indic: test screenshots (built-in Hindi)

CrossInk desktop simulator, Xteink X4 Pro profile (480×800), firmware `CrossInk-Indic` with **no fonts on the SD card**: every Devanagari glyph here comes from the firmware's built-in Noto Sans Devanagari, shaped from the layout font compiled into flash.

| File | What it shows |
|---|---|
| `C01-home-empty.png` | Home in Hindi with an empty SD card (no `/.fonts`). |
| `C01-home-files.png` | File browser with a Devanagari file name. |
| `C01-home-bitter-p1.png` | Devanagari test page in the built-in **Bitter** reading font: conjuncts, reph, vowel signs, nukta, chandrabindu, mixed Hindi/English. |
| `C01-home-bitter-p2.png` | Premchand's «पंच परमेश्वर» in built-in Bitter (14 pt). |
| `C01-home-book.png` | Home "continue reading" card with the Hindi title and author. |
| `C02-lexend-p1.png`, `C02-lexend-p2.png` | Same pages in the built-in **Lexend Deca** reading font. |
| `C03-size16-p2.png` | Built-in Bitter at **16 pt**. |
| `C04-size10-p2.png` | Built-in Lexend Deca at **10 pt**. |
| `C05-settings-display.png`, `-reader`, `-controls`, `-system` | Every Settings tab in Hindi. |
| `C05-settings-device.png` | System > डिवाइस, language «हिन्दी (Hindi)». |
| `C06-reader-menu-menu.png` | The in-book menu over a Hindi page. |
| `C07-transfer-screen.png` | **File Transfer** in Hindi: network screens stay Hindi here (the SD-font build has to show English there). |
| `C08-styles-page.png` | Hindi in regular, **bold**, *italic* (upright Devanagari), bold italic, superscript and subscript, and mixed with English italic, all from the built-in fonts. |
| `C10-device-*.png` | The same built-in Hindi on the X4, X3, Sticky and X4 Classic simulator profiles. |

## ESP32-C3 (X3/X4) in QEMU

`qemu-c3-shaping.log`: the shaping test firmware (`test/qemu/c3_shaping`) booted twice in Espressif's QEMU (`-machine esp32c3`, CrossInk's partition table, no PSRAM).

- **Built-in path:** layout font read in place from flash. 16/16 Devanagari reference words match HarfBuzz; the face costs 228 bytes of heap.
- **SD-card path:** layout font copied into the `spiffs` partition and memory-mapped (FlashBlobCache). 16/16 match HarfBuzz, including advances and mark offsets.
- **Second boot:** reuses the flash copy (no "Copying ..." line).
- **Heap:** shaping takes about 11 KB at its peak.
- **Drawing:** the built-in Devanagari font inflates in groups of at most 16 KB. All 125 shaped glyphs of a Premchand paragraph load both prewarmed and on demand, and free heap stays above 266 KB.
