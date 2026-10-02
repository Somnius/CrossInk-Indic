# Hindi test harness

Simulator checks used for the Hindi builds. Each script drives the CrossInk
desktop simulator through a scripted input sequence, saves screenshots and
checks the logs for crashes. Run them from anywhere; paths are relative to
this folder and the repository.

| Script | Build it expects (`pio run -e x4-pro-simulator`) | What it checks |
|---|---|---|
| `run-phaseA.sh` | an SD-font reading build (`feat/hindi-reading`), via `NEW=` | Hindi books with the SD fonts (tagged `hi`, wrongly `en`, untagged), UI file names; then a pixel regression against stock CrossInk (`OLD=`, a stock simulator build) on English test books |
| `run-phaseB.sh` | an SD-font Hindi UI build (`feat/hindi`), via `P=` | Hindi menus with the SD fonts, the no-font fallback to English |
| `run-phaseC.sh` | CrossInk-Indic (this branch), via `P=` | Built-in Hindi, no fonts on the card: books, sizes, both reading fonts, every Settings tab, reader menu, File Transfer |

Phases A and B need the SD fonts unpacked into `fonts-sd/` (the
`BitterHindi-sd-font.zip` / `LexendHindi-sd-font.zip` release assets).
Screenshots go to `shots/`, `shotsB/` and `shotsC/`.

`sim2.sh` runs one simulator session: a fresh card with one book, a settings
file, an input script (`ms:KEY;...`, keys `CONFIRM`, `DOWN`, `UP`, `BACK`,
`HOME`, `QUIT`, `TAP:x,y` with 0-1 coordinates) and screenshot times.

`books/` holds the test EPUBs: `make_epub.py` builds the Premchand ones from
`texts/` (public-domain stories from 1916 and 1930, plus a Devanagari test
page), `make_styles_epub.py` the bold/italic/superscript one.

The ESP32-C3 checks are in `../qemu/c3_shaping`.
