# Hindi on CrossInk: test screenshots

All screenshots come from the CrossInk desktop simulator for the Xteink X4 Pro (480×800, same reader code as the device), unless a file says "device". Each phase keeps its successful tests here for posts and docs.

| File | What it shows |
|---|---|
| `00-stock-p1.png` | **Before.** Official CrossInk (upstream `b25beb1`, v1.6.0 + X4 Classic fix) opening the Hindi test book: every Devanagari character is a ◆ box, because no built-in font has Devanagari and there is no shaping. |
| `00-stock-home.png` | Before: Home screen with the Hindi test book's title and author as boxes. |
| `01-stock-sdfont-p1.png` | **Before, with a Devanagari font but no shaping.** Same stock firmware, with Noto Sans Devanagari installed as an SD-card font: the letters exist but nothing is shaped. Conjuncts break apart with a visible virama (क्षत्रिय → क्‌ष‌त्‌रि‌य), the इ-matra lands after its consonant instead of before (कि, स्थिति, हिन्दी), reph shows as a full र् in front (धर्म, कार्य), and the mixed Hindi/English line collides. This is why Hindi needs a shaping engine, not just a font. |
| `02-shaped-sans-p1.png` | **First shaped render.** The ported OpenType shaper (CrossPoint #3787) with a Noto Sans Devanagari-only SD font: conjuncts (क्ष, त्र, ज्ञ, द्ध, ह्म, ङ्क, द्य, द्म, स्त्र), reph (धर्म, कार्य, पूर्व), the इ-matra before its consonant (कि, शक्ति, स्थिति), nukta and chandrabindu are all right. The mixed line still shows boxes only because Noto Sans Devanagari has no Latin letters, which is why the release fonts merge a Latin face with a Devanagari one. |
| `03-bitter-hi-p1.png` | **Phase A test page, "Bitter Hindi" SD font** (Bitter + Noto Serif Devanagari), book tagged `hi`: every torture-test line shaped, Latin and Devanagari mixed on one line, the chapter title shaped in the status bar. |
| `03-bitter-hi-p2.png`, `-p3.png` | Premchand's «पंच परमेश्वर» (1916, public domain) in Bitter Hindi, justified, with the Devanagari danda (।) and conjuncts in running text. |
| `03-bitter-hi-home.png` | Home after reading: title and author in Devanagari, reading progress. |
| `03-bitter-hi-books.png` | File browser with a Devanagari file name («पंच परमेश्वर.epub»), drawn through the UI fallback in the SD font. |
| `04-lexend-hi-p1.png`, `-p3.png` | Same pages in **"Lexend Hindi"** (Lexend Deca + Noto Sans Devanagari). |
| `05-bitter-en-tagged-p1.png`, `-p2.png` | **Book wrongly tagged `en`** (common in the wild): Devanagari still shapes exactly as in the `hi` book, because shaping follows the script, not the tag. |
| `06-lexend-untagged-p2.png` | **Book with no language tag**: Premchand's «पूस की रात» (1930, public domain), shaped correctly in Lexend Hindi. |
| `07-ui-bitter-books.png` | File browser with a long Devanagari file name («पंच परमेश्वर — प्रेमचंद की कहानी.epub») drawn in full in the Hindi UI fallback. Names too long for the row are cut with "…" at a whole syllable, never inside a conjunct. |
| `07-ui-bitter-home.png` | Home screen "continue reading" card with the Devanagari title «हिन्दी परीक्षण पुस्तक» and author «मुंशी प्रेमचंद», shaped and measured in the Hindi UI fallback. |
| `08-ui-mixed-file-list.png` | File browser with Latin and Devanagari names side by side, every name measured correctly (Devanagari names no longer cut short). |
| `09-ui-truncation-books.png` | A Devanagari file name too long for its row, cut with "…" at a whole syllable (no dangling virama or split conjunct). |
| `10-device-simulator-p1.png` | Same test page on the **Xteink X4** simulator profile (ESP32-C3 device class). |
| `10-device-simulator-X3-p1.png` | Same test page on the **Xteink X3** profile (528×792 panel). |
| `10-device-sticky-simulator-p2.png` | Premchand in Bitter Hindi on the **Seeed Sticky** profile. |
| `10-device-x4-classic-simulator-home.png` | Home with the Devanagari title on the **Xteink X4 Classic** profile. |

## Regression check (non-Indic text)

The Hindi build and the stock build drew pixel-identical screens (ImageMagick `compare -metric AE` = 0) for 3 English test books × 2 built-in fonts × 6 screens (boot, file list, three pages, Home), and for 2 books × 5 screens with a Latin-only SD-card font. Adding shaping did not change anything for non-Indic readers.

## Device notes

Every device profile (X3, X4, X4 Pro, X4 Classic, Sticky) drew the same shaped text in its simulator. The simulator always keeps the shaping tables in RAM. On the real X3/X4 (ESP32-C3, no PSRAM) they are instead copied once into the unused `spiffs` flash partition and memory-mapped (CrossPoint's `FlashBlobCache`). That path is covered by host tests but, as in the upstream PR, not yet by a real device, so the X3/X4 build is marked untested on hardware. If it ever fails, text falls back to unshaped letters; it cannot touch the firmware partitions.
