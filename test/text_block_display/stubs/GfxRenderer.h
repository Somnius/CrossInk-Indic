#pragma once

// Minimal renderer for TextBlockDisplayTest: records every string drawText
// receives (a laid-out word's display form when it has one) and measures with
// fixed metrics, so the test sees exactly what TextBlock hands the renderer.
#include <EpdFontFamily.h>
#include <Utf8.h>

#include <cstdint>
#include <string>
#include <vector>

// CrossInk declares the paragraph direction next to the renderer (GfxRenderer.h).
namespace BidiUtils {
enum class BidiBaseDir : signed char { AUTO = -1, LTR = 0, RTL = 1 };
}  // namespace BidiUtils

class GfxRenderer {
 public:
  struct LaidOutText {
    const char* text;
    const char* display;
  };

  mutable std::vector<std::string> drawnTexts;

  bool isFontCacheScanning() const { return false; }
  int getFontAscenderSize(int) const { return 12; }
  int getSpaceWidth(int, EpdFontFamily::Style = EpdFontFamily::REGULAR) const { return 4; }
  void fillRect(int, int, int, int, bool) const {}
  void drawLine(int, int, int, int, int, bool) const {}

  void drawText(int, int, int, const char* text, bool = true, EpdFontFamily::Style = EpdFontFamily::REGULAR,
                BidiUtils::BidiBaseDir = BidiUtils::BidiBaseDir::AUTO) const {
    drawnTexts.emplace_back(text);
  }
  void drawText(int font, int x, int y, const LaidOutText& text, bool black = true,
                EpdFontFamily::Style style = EpdFontFamily::REGULAR,
                BidiUtils::BidiBaseDir baseDir = BidiUtils::BidiBaseDir::AUTO) const {
    drawText(font, x, y, text.display ? text.display : text.text, black, style, baseDir);
  }

  // Fixture metrics: every codepoint is 8 px wide.
  int getTextAdvanceX(int, const char* text, EpdFontFamily::Style, uint32_t = 0) const {
    int width = 0;
    while (utf8NextCodepoint(reinterpret_cast<const uint8_t**>(&text))) width += 8;
    return width;
  }
  int getTextAdvanceX(int font, const LaidOutText& text, EpdFontFamily::Style style, uint32_t next = 0) const {
    return getTextAdvanceX(font, text.display ? text.display : text.text, style, next);
  }
  int getTextWidth(int font, const char* text, EpdFontFamily::Style style = EpdFontFamily::REGULAR,
                   BidiUtils::BidiBaseDir = BidiUtils::BidiBaseDir::AUTO) const {
    return getTextAdvanceX(font, text, style);
  }
};
