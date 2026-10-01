# Hindi UI on CrossInk: test screenshots (Phase B)

CrossInk desktop simulator, Xteink X4 Pro profile (480×800), firmware `feat/hindi`. Menus come from `lib/I18n/translations/hindi.yaml` (machine-assisted, Android Hindi register, awaiting review by a native speaker). Devanagari in the UI is drawn by the selected Hindi SD font as the UI fallback and shaped like book text.

| File | What it shows |
|---|---|
| `B01-home-empty.png` | Home with no book: «कोई किताब खुली नहीं है», «फ़ाइलें ब्राउज़ करें», «हाल की किताबें», «फ़ाइल ट्रांसफ़र», «सेटिंग». |
| `B01-home-files.png` | File browser with a Devanagari file name. |
| `B01-home-book.png` | Home after reading: the "continue reading" card with the Hindi title, author and progress. |
| `B02-settings-display.png` | Settings > डिस्प्ले (Display) tab: every row and value in Hindi. |
| `B02-settings-reader.png` | Settings > रीडर (Reader) tab. |
| `B02-settings-controls.png` | Settings > कंट्रोल (Controls) tab. |
| `B02-settings-system.png` | Settings > सिस्टम (System) tab; brand names (KOReader, OPDS) stay Latin. |
| `B02-settings-popup.png` | A sub-page (पढ़ने के आंकड़े, Reading Stats). |
| `B03-device-page.png` | System > डिवाइस (Device): language row shows «हिन्दी (Hindi)», dates and times stay numeric. |
| `B04-picker-list.png` | Language picker (भाषा) with «हिन्दी (Hindi)» at the end of the list. |
| `B05-reader-menu-menu.png` | The in-book reader menu over a shaped Hindi page. |
| `B06-lexend-home.png`, `B06-lexend-reader-settings.png` | Same UI with the Lexend Hindi family (sans). |
| `B07-nofont-picker.png` | **No Hindi font installed**: the picker lists plain "Hindi" instead of boxes. |
| `B07-nofont-alert.png` | Choosing Hindi without a Hindi font explains (in English) what to install. |
| `B07-nofont-settings.png` | …and the menus stay in English (no ◆ boxes) until a Hindi font is the reader font. |
| `B08-no8pt-home.png`, `B08-no8pt-book.png` | Bitter Hindi installed **without the 8 pt file** (as the on-device download's default size range does): Hindi menus still work, the small UI text uses the nearest size. |
| `B09-font-preview-hindi.png` | Font Family screen in Hindi: Bitter Hindi previews a shaped Devanagari sample («श्रद्धा, ज्ञान और क्षमा»). |
| `B09-font-preview-builtin.png` | Previewing a built-in font that has no Devanagari falls back to the English sample instead of boxes. |

| `B10-device-simulator-home.png` | Hindi Home on the **Xteink X4** profile, with Hindi button-hint labels (चुनें, ऊपर, नीचे). |
| `B10-device-simulator-X3-home.png` | Same on the **Xteink X3** profile. |
| `B10-device-sticky-simulator-book.png` | Home with the book card on the **Seeed Sticky** profile. |
| `B10-device-x4-classic-simulator-book.png` | Same on the **Xteink X4 Classic** profile (button labels incl. पढ़ें). |
64 long messages (KOReader sync-server errors, firmware-update, crash and low-memory diagnostics, Nearby transfer failures, keyboard tips, USB Drive hints, Calibre setup steps) stay in English: CrossInk stores each language's strings in at most 32 KB, and Devanagari takes 3 bytes per letter.
