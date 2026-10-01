#include "BuiltinShaping.h"

#include <Logging.h>

#include <mutex>
#include <new>

#include "ComplexShaper.h"
#include "EpdFontData.h"

namespace {
// The layout font lives in flash for the firmware's lifetime: nothing to free.
void keepFlashBlob(void*) {}

bool loadFlashBlob(void* ctx, ComplexShaper::Blob* out) {
  const auto* face = static_cast<const BuiltinShapingFace*>(ctx);
  *out = ComplexShaper::Blob{face->layout, face->layoutLength, &keepFlashBlob};
  return true;
}

// This font's own advance for a glyph (12.4 pixels) in the shaper's 26.6.
int32_t glyphAdvance(void* ctx, const uint32_t glyph) {
  const auto* face = static_cast<const BuiltinShapingFace*>(ctx);
  if (glyph >= face->tokenGlyphCount) return -1;
  return static_cast<int32_t>(static_cast<const EpdGlyph*>(face->tokenGlyphs)[glyph].advanceX) * 4;
}
}  // namespace

bool builtinShaping::shape(void* ctx, const char* utf8, std::string* out) {
  auto* face = static_cast<BuiltinShapingFace*>(ctx);
  if (face == nullptr || out == nullptr) return false;
  // Layout (main loop) and drawing (render task) can both get here first.
  static std::mutex creation;
  std::unique_lock<std::mutex> lock(creation);
  if (face->shaper == nullptr) {
    // One small object per built-in font that has shaped text, kept for the
    // firmware's lifetime (ComplexShaper::releaseAll() frees its face and caches).
    auto* shaper = new (std::nothrow) ComplexShaper();
    if (shaper == nullptr) {
      LOG_ERR("SHAPE", "OOM creating built-in font shaper");
      return false;
    }
    shaper->setBlobSource(&loadFlashBlob, face, face->layoutKey);
    shaper->setScale(face->ppem26_6);
    shaper->setAdvanceSource(&glyphAdvance, face);
    face->shaper = shaper;
  }
  lock.unlock();  // the shaper serializes shaping itself
  return face->shaper->shape(utf8, *out);
}
