// Built-in fonts shape Devanagari from a layout font compiled into flash
// (BuiltinShaping). These run the real generated font data: the UI script
// font and the shared reading fallbacks.
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "../complex_shaper/ExpectedShaping.h"
#include "ComplexShaper.h"
#include "EpdFont.h"
#include "EpdFontData.h"
#include "ShapingTokens.h"
#include "Utf8.h"
#include "builtinFonts/devanagari_14_bold.h"
#include "builtinFonts/devanagari_14_regular.h"
#include "builtinFonts/devanagari_ui_12.h"
#include "builtinFonts/inter_12_bold.h"

namespace {

std::vector<uint32_t> glyphIds(const std::string& tokens) {
  std::vector<uint32_t> ids;
  const auto* p = reinterpret_cast<const unsigned char*>(tokens.c_str());
  while (const uint32_t cp = utf8NextCodepoint(&p)) {
    if (shaping::isGlyphToken(cp)) ids.push_back(cp - shaping::GLYPH_TOKEN_BASE);
  }
  return ids;
}

const ShapingFixture& devanagariFixture() {
  for (const auto& fixture : kShapingFixtures) {
    if (std::string(fixture.script) == "Devanagari") return fixture;
  }
  return kShapingFixtures[0];
}

void expectHarfBuzzGlyphs(const EpdFontData& font) {
  ASSERT_NE(font.shapeHandler, nullptr);
  ASSERT_NE(font.shapeCtx, nullptr);
  const EpdFont epdFont(&font);
  const ShapingFixture& fixture = devanagariFixture();
  for (size_t w = 0; w < fixture.count; w++) {
    const ExpectedShaping& expected = fixture.words[w];
    ComplexShaper::setDocumentLanguage(expected.language);
    std::string out;
    ASSERT_TRUE(font.shapeHandler(font.shapeCtx, expected.utf8, &out)) << expected.utf8;
    std::vector<uint32_t> want;
    for (uint8_t i = 0; i < expected.count; i++) want.push_back(expected.glyphs[i].gid);
    EXPECT_EQ(glyphIds(out), want) << expected.utf8 << " (" << expected.language << ")";
    for (const uint32_t gid : glyphIds(out)) {
      EXPECT_NE(epdFont.findGlyph(shaping::GLYPH_TOKEN_BASE + gid), nullptr) << "missing glyph " << gid;
    }
  }
  ComplexShaper::setDocumentLanguage("");
}

}  // namespace

TEST(BuiltinShaping, UiFontMatchesHarfBuzzGlyphs) { expectHarfBuzzGlyphs(devanagari_ui_12); }

TEST(BuiltinShaping, ReadingFallbacksMatchHarfBuzzGlyphs) {
  expectHarfBuzzGlyphs(devanagari_14_regular);
  expectHarfBuzzGlyphs(devanagari_14_bold);  // regular's layout, bold's own glyphs
}

TEST(BuiltinShaping, BoldUiFontLeavesDevanagariToRegular) {
  EXPECT_EQ(inter_12_bold.shapeHandler, nullptr);
  const EpdFont bold(&inter_12_bold);
  EXPECT_EQ(bold.findGlyph(0x0915), nullptr);
}

TEST(BuiltinShaping, BoldAdvancesComeFromTheBoldGlyphs) {
  std::string regular;
  std::string bold;
  ASSERT_TRUE(devanagari_14_regular.shapeHandler(devanagari_14_regular.shapeCtx, "क्षत्रिय", &regular));
  ASSERT_TRUE(devanagari_14_bold.shapeHandler(devanagari_14_bold.shapeCtx, "क्षत्रिय", &bold));
  EXPECT_EQ(glyphIds(regular), glyphIds(bold));
  EXPECT_NE(regular, bold) << "bold must advance by its own (wider) glyphs";
}
