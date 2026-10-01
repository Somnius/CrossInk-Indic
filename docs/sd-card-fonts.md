---
title: SD Card Fonts
nav_order: 4
---

# SD Card Fonts

CrossInk supports loading additional fonts from the SD card, including fonts
with extended Unicode coverage (CJK, Cyrillic, Greek, etc.).

## Installing Fonts

There are three ways to install fonts:

### Option 1: Download from device

1. Connect your CrossInk reader to Wi-Fi
2. Go to **Settings > Reader > Font Options > Manage Fonts**
3. Browse available font families and select to download
4. Downloaded fonts appear immediately in **Settings > Reader > Font Options > Font Family**

**Note**: To change the font sizes that are downloaded, change the option for `Download Font Size Range` _before_ downloading.

### Option 2: Upload via web browser

1. Start **File Transfer** and connect through **Join Network** or **Create Hotspot**
2. Open the web interface URL shown on the reader
3. Navigate to the **Fonts** tab
4. Upload `.cpfont` files using the upload form

### Option 3: Manual SD card copy (Fastest)

1.  Download font files from the
    [CrossInk Fonts](https://github.com/uxjulia/crossink-fonts/tree/main/cpfonts) repository.
    - Click the `.zip` file for the font you want then click on the download icon to download the raw file.
2.  Copy font family folders to one of two locations on your SD card:
    - `/.fonts/` — hidden directory (preferred; keeps the SD root tidy
      when mounted on a desktop)
    - `/fonts/` — visible directory (use this if your OS hides dot-files
      and you'd rather see the folder in your file manager)

    Both roots are always scanned at boot and the results are merged: a
    family installed in `/fonts/` shows up even when `/.fonts/` also
    exists, and vice versa. The two roots only collide if the same family
    name appears in both — in that case the copy in `/.fonts/` wins and
    the duplicate in `/fonts/` is ignored.

        SD Card Root/
        ├── .fonts/                     ← Hidden root (preferred)
        │   └── Literata/
        │       ├── Literata_12.cpfont
        │       ├── Literata_14.cpfont
        │       ├── Literata_16.cpfont
        │       └── Literata_18.cpfont
        └── fonts/                      ← Visible root (equally valid)
            └── Merriweather/
                ├── Merriweather_12.cpfont
                └── ...

3.  Insert the SD card and power on your CrossInk device

## Dictionary Fonts

EPUB books can use a different installed SD-card family for dictionary definitions.
This can be set globally or per-book via `Font Options`. If a
saved point size is no longer available, CrossInk chooses the closest file from
the dictionary family. If the device experiences low available RAM, you may see the
dictionary font fall back to your reader font. This is normal.

### Generating dictionary font families

Use the dictionary-specific builder to generate the complete family catalog with
the extra coverage used by dictionary definitions:

    python3 -m pip install -r lib/EpdFont/scripts/requirements.txt
    python3 lib/EpdFont/scripts/build-dictionary-fonts.py \
      --output-dir ./generated-dictionary-fonts \
      --clean \
      --jobs 2

The dictionary build includes the `reading` ranges and the built-in ranges, plus
IPA and phonetic-extension characters (`U+0250–U+02FF` and `U+1D00–U+1DBF`) and
combining-mark ranges (`U+1DC0–U+1DFF`, `U+20D0–U+20FF`, and
`U+FE20–U+FE2F`).

The default output is `../crossink-fonts/dictionary-fonts`. Use a separate
`--output-dir` for personal builds, because `--clean` removes the selected output
directory before generating the fonts. The output contains family folders and ZIP
archives; copy a family folder or unzip its archive into `/.fonts/` or `/fonts/`
on the SD card. Use `--only FamilyA,FamilyB` to generate selected families.

## Hindi and other Indic scripts

Devanagari (Hindi, Marathi, Nepali, Sanskrit) and the other Indic scripts need
**shaping**: vowel signs move around their consonants (कि), consonant clusters
join into conjuncts (क्ष, त्र, ज्ञ), ra + virama becomes a reph above the next
letter (धर्म), and marks sit where the font places them. A font alone is not
enough; without shaping, conjuncts fall apart with a visible virama (क्‌ष).

This build shapes them with the OpenType shaper from CrossPoint Reader
([#3787](https://github.com/crosspoint-reader/crosspoint-reader/pull/3787) by
@ssafayet, `lib/OtShaper`). It follows HarfBuzz's Indic and Universal Shaping
Engine rules, using the font's own OpenType tables. The engine handles all ten
Indic presets in the [preset table](#available-unicode-interval-presets) below; this build ships and has tested fonts for
**Devanagari** only.

- **Built in.** CrossInk-Indic compiles Noto Sans Devanagari into the firmware
  as the reading fonts' and UI fonts' Devanagari, so Hindi needs no SD-card
  font at all. The families below are an alternative with a serif Devanagari
  (Bitter Hindi) or a different Latin face.
- **Fonts.** Two ready families come with the release, as zips to copy into
  `/.fonts/` on the SD card. Then select one under
  **Settings > Reader > Font Family**:

  | Family | Latin | Devanagari |
  |---|---|---|
  | **Bitter Hindi** | Bitter | Noto Serif Devanagari |
  | **Lexend Hindi** | Lexend Deca | Noto Sans Devanagari |

  Italic styles keep the Latin italic and use upright Devanagari, as
  Devanagari has no italic tradition. Both families are built from
  `lib/EpdFont/scripts/sd-fonts.yaml` with `script_fallbacks`, a Latin face
  merged with a Devanagari one. All fonts are under the SIL Open Font License.
- **Book language.** Shaping follows the script, so Hindi text shapes the same
  in a book tagged `hi`, wrongly tagged `en`, or untagged. The book's
  `dc:language` only picks language-specific forms in fonts that have them
  (Marathi and Nepali letterforms, for example).
- **Line breaks.** Lines break between words and never inside an akshara
  (syllable). Hindi is customarily not hyphenated, so no Hindi hyphenation
  patterns are used.
- **Titles and the interface.** Book titles, file names, the table of contents
  and the status bar use the selected family as a size-matched UI fallback,
  as for CJK, which is why the families include the 8, 10 and 12 pt sizes.
- **Your own `.cpfont`.** Converting with a script's preset embeds the shaping
  data automatically when the font has an OpenType `GSUB` table. Indic fonts
  rarely include Latin letters, so add a Latin fallback, and build the UI
  sizes as well:

      python3 lib/EpdFont/scripts/fontconvert_sdcard.py \
        --regular MyHindi-Regular.ttf --bold MyHindi-Bold.ttf \
        --fallback-regular NotoSans-Regular.ttf --fallback-bold NotoSans-Bold.ttf \
        --intervals devanagari,latin-ext,punctuation \
        --sizes 8,10,12,14,16,18 \
        --name MyHindi --output-dir ./MyHindi/

  The shaping data comes from whichever face (the primary, or the first
  fallback whose ranges include the script) has the script's OpenType tables.
  Pass `--no-shaping` to leave it out. Firmware without shaping ignores the
  section, so the same files work there too, unshaped.

On the X3 and X4 (no PSRAM), the first time an Indic font is used its layout
tables (about 90-100 KB per style for Noto Devanagari) are copied once into
the unused `spiffs` area of the device's internal flash and read from there,
keeping the reader's RAM free. The converter warns when a font's tables exceed
the 128 KB that area holds per font. Opening a chapter for the first time takes
longer than a Latin one, because every word is shaped while the chapter is
laid out; page turns draw the shaped words stored with the chapter's cached
layout instead of shaping them again. If the X3/X4 runs short of memory while
laying out a chapter, the words it could not shape show unshaped for that
session and the chapter is laid out again the next time you open the book.

## Available Pre-Built Fonts

You can view pre-built fonts available for download at [Inky](https://inky.crossink.dev/#downloads).

## Converting Custom Fonts with CrossPoint's Font Builder

To convert your own TrueType/OpenType fonts use CrossPoint's [Font Builder](https://crosspointreader.com/fonts)

## Converting Custom Fonts with Python

### Prerequisites

    pip install freetype-py fonttools

### Single font (one style)

    python3 lib/EpdFont/scripts/fontconvert_sdcard.py \
      MyFont-Regular.ttf \
      --intervals latin-ext \
      --sizes 12,14,16,18 \
      --style regular \
      --name MyFont \
      --output-dir ./MyFont/

### Multi-style font

    python3 lib/EpdFont/scripts/fontconvert_sdcard.py \
      --regular MyFont-Regular.ttf \
      --bold MyFont-Bold.ttf \
      --italic MyFont-Italic.ttf \
      --bolditalic MyFont-BoldItalic.ttf \
      --intervals latin-ext \
      --sizes 12,14,16,18 \
      --name MyFont \
      --output-dir ./MyFont/

### Available Unicode interval presets

| Preset        | Coverage                                                                                                             |
| ------------- | -------------------------------------------------------------------------------------------------------------------- |
| `ascii`       | U+0020–U+007E (Basic Latin)                                                                                          |
| `latin1`      | U+0080–U+00FF (Latin-1 Supplement)                                                                                   |
| `latin-ext`   | European languages (Latin + Extended-A/B + punctuation + ligatures)                                                  |
| `greek`       | Greek + Extended Greek                                                                                               |
| `cyrillic`    | Cyrillic + Supplement                                                                                                |
| `hebrew`      | Hebrew + Alphabetic Presentation Forms                                                                               |
| `devanagari`, `bengali`, `gurmukhi`, `gujarati`, `oriya`, `tamil`, `telugu`, `kannada`, `malayalam`, `sinhala` | One Indic script's block + dandas + joiners; embeds OpenType shaping data (see [Hindi and other Indic scripts](#hindi-and-other-indic-scripts)) |
| `georgian`    | Georgian + Georgian Supplement                                                                                       |
| `armenian`    | Armenian                                                                                                             |
| `ethiopic`    | Ethiopic + Extended                                                                                                  |
| `vietnamese`  | Vietnamese subset (ơ/ư and combining marks)                                                                          |
| `punctuation` | General punctuation (U+2000–U+206F)                                                                                  |
| `cjk`         | CJK Unified Ideographs + Hiragana + Katakana + Fullwidth                                                             |
| `hangul`      | Korean Hangul syllables + Jamo + Compatibility Jamo                                                                  |
| `cherokee`    | Cherokee (historic + supplement block)                                                                               |
| `tifinagh`    | Tifinagh                                                                                                             |
| `symbols`     | Math, currency, arrows, box-drawing, misc symbols, dingbats                                                          |
| `reading`     | Literary fiction coverage: Latin, Greek, Cyrillic, math/symbol blocks, supplemental punctuation, and CJK quote marks |
| `builtin`     | Matches the firmware's built-in font conversion intervals                                                            |

Combine presets with commas: `--intervals latin-ext,greek,cyrillic`

You can also specify arbitrary Unicode ranges directly:
`--intervals latin-ext,(0x2100-0x214F)`

To list all presets with codepoint counts:

    python3 lib/EpdFont/scripts/fontconvert_sdcard.py --list-presets

### Additional options

`--force-autohint` — force FreeType's auto-hinter instead of the font's native hinting (useful when a font's built-in hints produce poor results at small sizes).

Install custom fonts via the web interface or manual SD card copy.
