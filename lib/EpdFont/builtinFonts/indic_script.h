#pragma once

// CrossInk-Indic compiles one Indic script (or Myanmar) into the firmware. Build with
// -DCROSSINK_SCRIPT_<NAME> (see scripts/build-indic.sh); none means Devanagari.
// Fonts come from scripts/convert-indic-fonts.sh. INDIC_READ(size, style) and
// INDIC_UI(size) name the selected script's font data.

#if (defined(CROSSINK_SCRIPT_DEVANAGARI) + defined(CROSSINK_SCRIPT_BENGALI) + defined(CROSSINK_SCRIPT_GURMUKHI) + \
     defined(CROSSINK_SCRIPT_GUJARATI) + defined(CROSSINK_SCRIPT_ORIYA) + defined(CROSSINK_SCRIPT_TAMIL) +       \
     defined(CROSSINK_SCRIPT_TELUGU) + defined(CROSSINK_SCRIPT_KANNADA) + defined(CROSSINK_SCRIPT_MALAYALAM) +   \
     defined(CROSSINK_SCRIPT_SINHALA) + defined(CROSSINK_SCRIPT_MYANMAR)) > 1
#error "Define at most one CROSSINK_SCRIPT_* (scripts/build-indic.sh)"
#endif

#if defined(CROSSINK_SCRIPT_DEVANAGARI) || (!defined(CROSSINK_SCRIPT_BENGALI) && !defined(CROSSINK_SCRIPT_GURMUKHI) && !defined(CROSSINK_SCRIPT_GUJARATI) && !defined(CROSSINK_SCRIPT_ORIYA) && !defined(CROSSINK_SCRIPT_TAMIL) && !defined(CROSSINK_SCRIPT_TELUGU) && !defined(CROSSINK_SCRIPT_KANNADA) && !defined(CROSSINK_SCRIPT_MALAYALAM) && !defined(CROSSINK_SCRIPT_SINHALA) && !defined(CROSSINK_SCRIPT_MYANMAR))
#include <builtinFonts/devanagari_10_regular.h>
#include <builtinFonts/devanagari_10_bold.h>
#include <builtinFonts/devanagari_12_regular.h>
#include <builtinFonts/devanagari_12_bold.h>
#include <builtinFonts/devanagari_14_regular.h>
#include <builtinFonts/devanagari_14_bold.h>
#include <builtinFonts/devanagari_16_regular.h>
#include <builtinFonts/devanagari_16_bold.h>
#include <builtinFonts/devanagari_ui_8.h>
#include <builtinFonts/devanagari_ui_10.h>
#include <builtinFonts/devanagari_ui_12.h>
#define INDIC_READ(size, style) devanagari_##size##_##style
#define INDIC_UI(size) devanagari_ui_##size
#elif defined(CROSSINK_SCRIPT_BENGALI)
#include <builtinFonts/bengali_10_regular.h>
#include <builtinFonts/bengali_10_bold.h>
#include <builtinFonts/bengali_12_regular.h>
#include <builtinFonts/bengali_12_bold.h>
#include <builtinFonts/bengali_14_regular.h>
#include <builtinFonts/bengali_14_bold.h>
#include <builtinFonts/bengali_16_regular.h>
#include <builtinFonts/bengali_16_bold.h>
#include <builtinFonts/bengali_ui_8.h>
#include <builtinFonts/bengali_ui_10.h>
#include <builtinFonts/bengali_ui_12.h>
#define INDIC_READ(size, style) bengali_##size##_##style
#define INDIC_UI(size) bengali_ui_##size
#elif defined(CROSSINK_SCRIPT_GURMUKHI)
#include <builtinFonts/gurmukhi_10_regular.h>
#include <builtinFonts/gurmukhi_10_bold.h>
#include <builtinFonts/gurmukhi_12_regular.h>
#include <builtinFonts/gurmukhi_12_bold.h>
#include <builtinFonts/gurmukhi_14_regular.h>
#include <builtinFonts/gurmukhi_14_bold.h>
#include <builtinFonts/gurmukhi_16_regular.h>
#include <builtinFonts/gurmukhi_16_bold.h>
#include <builtinFonts/gurmukhi_ui_8.h>
#include <builtinFonts/gurmukhi_ui_10.h>
#include <builtinFonts/gurmukhi_ui_12.h>
#define INDIC_READ(size, style) gurmukhi_##size##_##style
#define INDIC_UI(size) gurmukhi_ui_##size
#elif defined(CROSSINK_SCRIPT_GUJARATI)
#include <builtinFonts/gujarati_10_regular.h>
#include <builtinFonts/gujarati_10_bold.h>
#include <builtinFonts/gujarati_12_regular.h>
#include <builtinFonts/gujarati_12_bold.h>
#include <builtinFonts/gujarati_14_regular.h>
#include <builtinFonts/gujarati_14_bold.h>
#include <builtinFonts/gujarati_16_regular.h>
#include <builtinFonts/gujarati_16_bold.h>
#include <builtinFonts/gujarati_ui_8.h>
#include <builtinFonts/gujarati_ui_10.h>
#include <builtinFonts/gujarati_ui_12.h>
#define INDIC_READ(size, style) gujarati_##size##_##style
#define INDIC_UI(size) gujarati_ui_##size
#elif defined(CROSSINK_SCRIPT_ORIYA)
#include <builtinFonts/oriya_10_regular.h>
#include <builtinFonts/oriya_10_bold.h>
#include <builtinFonts/oriya_12_regular.h>
#include <builtinFonts/oriya_12_bold.h>
#include <builtinFonts/oriya_14_regular.h>
#include <builtinFonts/oriya_14_bold.h>
#include <builtinFonts/oriya_16_regular.h>
#include <builtinFonts/oriya_16_bold.h>
#include <builtinFonts/oriya_ui_8.h>
#include <builtinFonts/oriya_ui_10.h>
#include <builtinFonts/oriya_ui_12.h>
#define INDIC_READ(size, style) oriya_##size##_##style
#define INDIC_UI(size) oriya_ui_##size
#elif defined(CROSSINK_SCRIPT_TAMIL)
#include <builtinFonts/tamil_10_regular.h>
#include <builtinFonts/tamil_10_bold.h>
#include <builtinFonts/tamil_12_regular.h>
#include <builtinFonts/tamil_12_bold.h>
#include <builtinFonts/tamil_14_regular.h>
#include <builtinFonts/tamil_14_bold.h>
#include <builtinFonts/tamil_16_regular.h>
#include <builtinFonts/tamil_16_bold.h>
#include <builtinFonts/tamil_ui_8.h>
#include <builtinFonts/tamil_ui_10.h>
#include <builtinFonts/tamil_ui_12.h>
#define INDIC_READ(size, style) tamil_##size##_##style
#define INDIC_UI(size) tamil_ui_##size
#elif defined(CROSSINK_SCRIPT_TELUGU)
#include <builtinFonts/telugu_10_regular.h>
#include <builtinFonts/telugu_10_bold.h>
#include <builtinFonts/telugu_12_regular.h>
#include <builtinFonts/telugu_12_bold.h>
#include <builtinFonts/telugu_14_regular.h>
#include <builtinFonts/telugu_14_bold.h>
#include <builtinFonts/telugu_16_regular.h>
#include <builtinFonts/telugu_16_bold.h>
#include <builtinFonts/telugu_ui_8.h>
#include <builtinFonts/telugu_ui_10.h>
#include <builtinFonts/telugu_ui_12.h>
#define INDIC_READ(size, style) telugu_##size##_##style
#define INDIC_UI(size) telugu_ui_##size
#elif defined(CROSSINK_SCRIPT_KANNADA)
#include <builtinFonts/kannada_10_regular.h>
#include <builtinFonts/kannada_10_bold.h>
#include <builtinFonts/kannada_12_regular.h>
#include <builtinFonts/kannada_12_bold.h>
#include <builtinFonts/kannada_14_regular.h>
#include <builtinFonts/kannada_14_bold.h>
#include <builtinFonts/kannada_16_regular.h>
#include <builtinFonts/kannada_16_bold.h>
#include <builtinFonts/kannada_ui_8.h>
#include <builtinFonts/kannada_ui_10.h>
#include <builtinFonts/kannada_ui_12.h>
#define INDIC_READ(size, style) kannada_##size##_##style
#define INDIC_UI(size) kannada_ui_##size
#elif defined(CROSSINK_SCRIPT_MALAYALAM)
#include <builtinFonts/malayalam_10_regular.h>
#include <builtinFonts/malayalam_10_bold.h>
#include <builtinFonts/malayalam_12_regular.h>
#include <builtinFonts/malayalam_12_bold.h>
#include <builtinFonts/malayalam_14_regular.h>
#include <builtinFonts/malayalam_14_bold.h>
#include <builtinFonts/malayalam_16_regular.h>
#include <builtinFonts/malayalam_16_bold.h>
#include <builtinFonts/malayalam_ui_8.h>
#include <builtinFonts/malayalam_ui_10.h>
#include <builtinFonts/malayalam_ui_12.h>
#define INDIC_READ(size, style) malayalam_##size##_##style
#define INDIC_UI(size) malayalam_ui_##size
#elif defined(CROSSINK_SCRIPT_SINHALA)
#include <builtinFonts/sinhala_10_regular.h>
#include <builtinFonts/sinhala_10_bold.h>
#include <builtinFonts/sinhala_12_regular.h>
#include <builtinFonts/sinhala_12_bold.h>
#include <builtinFonts/sinhala_14_regular.h>
#include <builtinFonts/sinhala_14_bold.h>
#include <builtinFonts/sinhala_16_regular.h>
#include <builtinFonts/sinhala_16_bold.h>
#include <builtinFonts/sinhala_ui_8.h>
#include <builtinFonts/sinhala_ui_10.h>
#include <builtinFonts/sinhala_ui_12.h>
#define INDIC_READ(size, style) sinhala_##size##_##style
#define INDIC_UI(size) sinhala_ui_##size
#elif defined(CROSSINK_SCRIPT_MYANMAR)
#include <builtinFonts/myanmar_10_regular.h>
#include <builtinFonts/myanmar_10_bold.h>
#include <builtinFonts/myanmar_12_regular.h>
#include <builtinFonts/myanmar_12_bold.h>
#include <builtinFonts/myanmar_14_regular.h>
#include <builtinFonts/myanmar_14_bold.h>
#include <builtinFonts/myanmar_16_regular.h>
#include <builtinFonts/myanmar_16_bold.h>
#include <builtinFonts/myanmar_ui_8.h>
#include <builtinFonts/myanmar_ui_10.h>
#include <builtinFonts/myanmar_ui_12.h>
#define INDIC_READ(size, style) myanmar_##size##_##style
#define INDIC_UI(size) myanmar_ui_##size
#endif
