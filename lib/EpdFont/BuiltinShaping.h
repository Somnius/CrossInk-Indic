#pragma once

#include <cstdint>
#include <string>

class ComplexShaper;

// Complex-script shaping for fonts compiled into the firmware. The layout font
// (GSUB/GPOS and the compiled plans, see scripts/shaping_blob.py) is a const
// array the CPU reads in place from flash, so it costs no RAM on any board.
// fontconvert.py emits one BuiltinShapingFace per font that shapes; regular
// and bold share one layout font (same glyph order and GSUB) while advances
// come from each font's own glyphs.
struct BuiltinShapingFace {
  const uint8_t* layout;
  uint32_t layoutLength;
  uint32_t layoutKey;       // FNV-1a of the layout font, | 1 (shares faces between sizes)
  uint32_t ppem26_6;        // size the glyphs were rasterised at
  const void* tokenGlyphs;  // const EpdGlyph*: the glyph-token glyphs, by glyph ID
  uint32_t tokenGlyphCount;
  ComplexShaper* shaper;  // created on first use
};

namespace builtinShaping {
// EpdFontData::shapeHandler for built-in fonts; ctx is a BuiltinShapingFace.
bool shape(void* ctx, const char* utf8, std::string* out);
}  // namespace builtinShaping
