// QEMU (ESP32-C3) check of complex-script shaping on a board without PSRAM.
//
// 1. Built-in fonts: the layout font is a const array the CPU reads in place
//    from flash (BuiltinShaping), as CrossInk-Indic's built-in Devanagari does.
// 2. SD-card fonts: the layout font is copied once into the unused `spiffs`
//    partition and memory-mapped (FlashBlobCache), as SdCardFont does on the
//    X3/X4. The bytes come from the same array here instead of an SD card.
//
// Both are compared with HarfBuzz's output for the Devanagari reference words
// (test/complex_shaper/ExpectedShaping.h). Prints "C3 SHAPING: PASS" or FAIL.
#include <Arduino.h>
#include <esp_heap_caps.h>
#include <esp_memory_utils.h>
#include <esp_partition.h>

#include <cstring>
#include <string>
#include <vector>

#include "AllScriptsExpected.h"
#include "ComplexShaper.h"
#include "EpdFont.h"
#include "ExpectedShaping.h"
#include "FlashBlobCache.h"
#include "Fnv1a.h"
#include "FontDecompressor.h"
#include "ShapingTokens.h"
#include "Utf8.h"
#include "builtinFonts/bengali_14_regular.h"
#include "builtinFonts/devanagari_14_regular.h"
#include "builtinFonts/gujarati_14_regular.h"
#include "builtinFonts/gurmukhi_14_regular.h"
#include "builtinFonts/kannada_14_regular.h"
#include "builtinFonts/malayalam_14_regular.h"
#include "builtinFonts/myanmar_14_regular.h"
#include "builtinFonts/oriya_14_regular.h"
#include "builtinFonts/sinhala_14_regular.h"
#include "builtinFonts/tamil_14_regular.h"
#include "builtinFonts/telugu_14_regular.h"

namespace {

int gFailures = 0;

struct Glyph {
  uint32_t gid;
  int advance12_4;
  int dx;
  int dy;
};

std::vector<Glyph> decode(const std::string& tokens) {
  std::vector<Glyph> glyphs;
  shaping::PendingGlyph pending;
  const auto* p = reinterpret_cast<const unsigned char*>(tokens.c_str());
  while (const uint32_t cp = utf8NextCodepoint(&p)) {
    if (pending.consume(cp)) continue;
    if (shaping::isGlyphToken(cp)) {
      glyphs.push_back(
          {cp - shaping::GLYPH_TOKEN_BASE, static_cast<int>(pending.advanceOr(-1)), pending.dx, pending.dy});
      pending.reset();
    }
  }
  return glyphs;
}

const ShapingFixture& devanagari() {
  for (const auto& fixture : kShapingFixtures) {
    if (strcmp(fixture.script, "Devanagari") == 0) return fixture;
  }
  return kShapingFixtures[0];
}

void heap(const char* label) {
  Serial.printf("[HEAP] %-28s free=%u largest=%u min=%u\n", label, heap_caps_get_free_size(MALLOC_CAP_8BIT),
                heap_caps_get_largest_free_block(MALLOC_CAP_8BIT), heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT));
}

// fullCompare: also positions (only meaningful at the fixture's ppem).
void check(const char* label, bool (*shape)(void*, const char*, std::string*), void* ctx, const bool fullCompare) {
  const ShapingFixture& fixture = devanagari();
  int ok = 0;
  const uint32_t start = micros();
  for (size_t w = 0; w < fixture.count; w++) {
    const ExpectedShaping& want = fixture.words[w];
    ComplexShaper::setDocumentLanguage(want.language);
    std::string out;
    if (!shape(ctx, want.utf8, &out)) {
      Serial.printf("FAIL %s: could not shape word %u\n", label, static_cast<unsigned>(w));
      gFailures++;
      continue;
    }
    const auto got = decode(out);
    bool same = got.size() == want.count;
    for (size_t i = 0; same && i < got.size(); i++) {
      const ExpectedGlyph& g = want.glyphs[i];
      same = got[i].gid == g.gid &&
             (!fullCompare || (got[i].advance12_4 == g.advance12_4 && got[i].dx == g.dx && got[i].dy == g.dy));
    }
    if (same) {
      ok++;
    } else {
      Serial.printf("FAIL %s: word %u differs from HarfBuzz\n", label, static_cast<unsigned>(w));
      gFailures++;
    }
  }
  ComplexShaper::setDocumentLanguage("");
  Serial.printf("%s: %d/%u words match HarfBuzz (%lu us)\n", label, ok, static_cast<unsigned>(fixture.count),
                static_cast<unsigned long>(micros() - start));
}

// Built-in fonts of every script: glyph ids against HarfBuzz (gen_all_scripts.py).
struct BuiltinScript {
  const char* name;
  const EpdFontData* font;
  const uint8_t* layout;
  const ScriptWord* words;
  size_t wordCount;
  const char* paragraph;
};

#define BUILTIN_SCRIPT(s) \
  {#s, &s##_14_regular, s##Layout, k_##s##_words, sizeof(k_##s##_words) / sizeof(k_##s##_words[0]), k_##s##_paragraph},
const BuiltinScript kBuiltinScripts[] = {ALL_SCRIPTS_EXPECTED(BUILTIN_SCRIPT)};
#undef BUILTIN_SCRIPT

void checkWords(const BuiltinScript& script) {
  int ok = 0;
  const uint32_t start = micros();
  for (size_t w = 0; w < script.wordCount; w++) {
    const ScriptWord& want = script.words[w];
    std::string out;
    if (!script.font->shapeHandler(script.font->shapeCtx, want.utf8, &out)) {
      Serial.printf("FAIL %s: could not shape word %u\n", script.name, static_cast<unsigned>(w));
      gFailures++;
      continue;
    }
    const auto got = decode(out);
    bool same = got.size() == want.count;
    for (size_t i = 0; same && i < got.size(); i++) same = got[i].gid == want.gids[i];
    if (same) {
      ok++;
    } else {
      Serial.printf("FAIL %s: word %u differs from HarfBuzz\n", script.name, static_cast<unsigned>(w));
      gFailures++;
    }
  }
  Serial.printf("built-in %-10s %2d/%u words match HarfBuzz (%lu us), layout in flash: %s\n", script.name, ok,
                static_cast<unsigned>(script.wordCount), static_cast<unsigned long>(micros() - start),
                esp_ptr_in_drom(script.layout) ? "yes" : "no");
  if (!esp_ptr_in_drom(script.layout)) gFailures++;
}

// Glyph bitmaps of a shaped paragraph, prewarmed as the reader does, then on demand.
void checkBitmaps(const BuiltinScript& script) {
  const EpdFontData* data = script.font;
  std::string shaped;
  data->shapeHandler(data->shapeCtx, script.paragraph, &shaped);
  FontDecompressor decompressor;
  decompressor.init();
  uint32_t largestGroup = 0;
  for (uint16_t g = 0; g < data->groupCount; g++) {
    if (data->groups[g].uncompressedSize > largestGroup) largestGroup = data->groups[g].uncompressedSize;
  }
  const EpdFont font(data);
  for (int pass = 0; pass < 2; pass++) {
    decompressor.clearCache();
    if (pass == 0) decompressor.prewarmCache(data, shaped.c_str());
    int glyphs = 0;
    int missing = 0;
    const auto* p = reinterpret_cast<const unsigned char*>(shaped.c_str());
    while (const uint32_t cp = utf8NextCodepoint(&p)) {
      if (!shaping::isGlyphToken(cp)) continue;
      const EpdGlyph* glyph = font.findGlyph(cp);
      glyphs++;
      if (glyph == nullptr ||
          (glyph->dataLength > 0 &&
           decompressor.getBitmap(data, glyph, static_cast<uint32_t>(glyph - data->glyph)) == nullptr)) {
        missing++;
      }
    }
    Serial.printf("bitmaps %-10s %s: %d glyphs, %d missing (%u groups, largest %lu bytes)\n", script.name,
                  pass == 0 ? "prewarmed" : "on demand", glyphs, missing, data->groupCount,
                  static_cast<unsigned long>(largestGroup));
    if (missing != 0 || glyphs == 0) gFailures++;
  }
  decompressor.clearCache();
}

// --- SD-card path: FlashBlobCache, as SdCardFont::loadShapingBlob ---
const uint8_t* const kLayout = devanagariLayout;
constexpr uint32_t kLayoutLength = sizeof(devanagariLayout);

bool readLayout(void*, const uint32_t offset, uint8_t* buf, const uint32_t length) {
  if (offset + length > kLayoutLength) return false;
  memcpy(buf, kLayout + offset, length);
  return true;
}

// As in a .cpfont shaping section: FNV-1a of the layout font, | 1.
uint32_t layoutKey() { return fnv1a::hash(kLayout, kLayoutLength) | 1u; }

bool loadMapped(void*, ComplexShaper::Blob* out) {
  const uint8_t* mapped = FlashBlobCache::acquire(layoutKey(), kLayoutLength, readLayout, nullptr);
  if (mapped == nullptr) {
    Serial.println("FAIL flash cache: FlashBlobCache::acquire returned nullptr");
    gFailures++;
    return false;
  }
  Serial.printf("flash cache: layout mapped at %p (copy in RAM? %s)\n", mapped, esp_ptr_in_dram(mapped) ? "yes" : "no");
  if (memcmp(mapped, kLayout, kLayoutLength) != 0) {
    Serial.println("FAIL flash cache: mapped bytes differ from the source");
    gFailures++;
  }
  *out = ComplexShaper::Blob{mapped, kLayoutLength, &FlashBlobCache::release};
  return true;
}

bool shapeMapped(void* ctx, const char* utf8, std::string* out) {
  return static_cast<ComplexShaper*>(ctx)->shape(utf8, *out);
}

}  // namespace

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("\n=== C3 shaping test (no PSRAM) ===");
  heap("start");

  // 1. Built-in font data of every script, layouts read in place from flash.
  check("built-in devanagari fixture", devanagari_14_regular.shapeHandler, devanagari_14_regular.shapeCtx, false);
  for (const BuiltinScript& script : kBuiltinScripts) checkWords(script);
  heap("after built-in");

  // 2. SD-card path through FlashBlobCache, at the fixture's size. Same layout
  // content as the built-in font: drop that face first so this one is built
  // from the flash cache instead of being shared.
  ComplexShaper::releaseAll();
  if (const esp_partition_t* part =
          esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, nullptr)) {
    uint32_t entry[4] = {};
    esp_partition_read(part, 0, entry, sizeof(entry));
    Serial.printf("flash cache directory slot 0 at boot: magic=%08lx key=%08lx len=%lu (partition at 0x%lx)\n",
                  static_cast<unsigned long>(entry[0]), static_cast<unsigned long>(entry[1]),
                  static_cast<unsigned long>(entry[2]), static_cast<unsigned long>(part->address));
    const void* mapped = nullptr;
    esp_partition_mmap_handle_t handle;
    if (esp_partition_mmap(part, 0, 65536, ESP_PARTITION_MMAP_DATA, &mapped, &handle) == ESP_OK) {
      uint32_t viaMap[4];
      memcpy(viaMap, mapped, sizeof(viaMap));
      Serial.printf("same bytes through mmap:              magic=%08lx key=%08lx len=%lu\n",
                    static_cast<unsigned long>(viaMap[0]), static_cast<unsigned long>(viaMap[1]),
                    static_cast<unsigned long>(viaMap[2]));
      esp_partition_munmap(handle);
    }
  }
  {
    ComplexShaper shaper;
    shaper.setBlobSource(loadMapped, nullptr, layoutKey());
    shaper.setScale(kFixturePpem26_6);
    check("flash-mapped (SD font path)", shapeMapped, &shaper, true);
    heap("after flash-mapped");
    // Second shaper, same content key: must reuse the mapping, not copy again.
    ComplexShaper again;
    again.setBlobSource(loadMapped, nullptr, layoutKey());
    again.setScale(kFixturePpem26_6);
    check("flash-mapped, second shaper", shapeMapped, &again, true);
  }
  ComplexShaper::releaseAll();
  heap("after releaseAll");

  // 3. Drawing: glyph bitmaps of a shaped paragraph from each compressed
  // built-in font, prewarmed as the reader does and on demand.
  for (const BuiltinScript& script : kBuiltinScripts) checkBitmaps(script);
  ComplexShaper::releaseAll();
  heap("after bitmaps");

  Serial.printf("C3 SHAPING: %s (%d failure(s))\n", gFailures == 0 ? "PASS" : "FAIL", gFailures);
}

void loop() { delay(1000); }
