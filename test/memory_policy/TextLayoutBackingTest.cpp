#include <Arduino.h>
#include <Epub/ParsedText.h>
#include <Epub/hyphenation/Hyphenator.h>
#include <GfxRenderer.h>
#include <HalStorage.h>
#include <gtest/gtest.h>

#include <cstdlib>
#include <fstream>

// Hyphen dictionary availability is outside allocation policy. Exercise the
// production non-hyphenating layout, real bidi, TextBlock and serialization.
std::vector<Hyphenator::BreakInfo> Hyphenator::breakOffsets(const std::string&, bool) { return {}; }

struct TextLayoutBackingTest : testing::Test {
  void SetUp() override {
    fakeheap::reset();
    Storage.reset();
  }
  void TearDown() override {
    EXPECT_TRUE(fakeheap::live.empty());
    Storage.reset();
  }
  std::vector<uint8_t> layout(int font, int scenario, bool* result = nullptr) {
    BlockStyle style;
    if (scenario == 1) {
      style.directionDefined = true;
      style.isRtl = true;
    }
    if (scenario == 3) {
      style.marginLeft = 12;
      style.marginRight = 12;
    }
    ParsedText text(false, false, false, scenario == 4, scenario == 5, 0, style);
    const std::vector<std::string> words =
        scenario == 1 ? std::vector<std::string>{"שלום", "עולם", "עברית", "123"}
        : scenario == 2
            ? std::vector<std::string>{"日", "本", "語", "読", "書"}
            : std::vector<std::string>{"A", "paragraph", "with", "different", "word", "lengths", "and", "punctuation."};
    for (int i = 0; i < 400; ++i)
      text.addWord(words[i % words.size()], i % 3 ? EpdFontFamily::REGULAR : EpdFontFamily::BOLD, false, scenario == 2,
                   false, 0, i * 8);
    if (scenario == 2) text.setRubyGroupAt(0, 2, "にほん");
    FsFile output;
    EXPECT_TRUE(Storage.openFileForWrite("test", "layout", output));
    GfxRenderer renderer;
    bool ok = text.layoutAndExtractLines(renderer, font, 240,
                                         [&](std::shared_ptr<TextBlock> block, uint32_t offset, uint32_t) {
                                           EXPECT_TRUE(block->valid());
                                           EXPECT_TRUE(block->serialize(output));
                                           EXPECT_EQ(output.write(&offset, sizeof(offset)), sizeof(offset));
                                         });
    output.close();
    if (result)
      *result = ok;
    else
      EXPECT_TRUE(ok);
    return Storage.bytes("layout");
  }
};
TEST_F(TextLayoutBackingTest, SameSerializedLinesAcrossPoolsFontsAndReadingAids) {
  std::ofstream reference;
  if (const char* path = std::getenv("LAYOUT_REFERENCE_OUT")) reference.open(path, std::ios::binary);
  for (int font : {0, 2, 5})
    for (int scenario = 0; scenario < 6; ++scenario) {
      fakeheap::reset(false);
      const auto expected = layout(font, scenario);
      ASSERT_FALSE(expected.empty());
      EXPECT_TRUE(fakeheap::live.empty());
      if (reference) reference.write(reinterpret_cast<const char*>(expected.data()), expected.size());
      fakeheap::reset();
      EXPECT_EQ(layout(font, scenario), expected);
      EXPECT_TRUE(fakeheap::live.empty());
      fakeheap::reset();
      fakeheap::external.fail = 1000;
      EXPECT_EQ(layout(font, scenario), expected);
      EXPECT_TRUE(fakeheap::live.empty());
    }
}
#ifndef PSRAM_REFERENCE
TEST_F(TextLayoutBackingTest, AllocationFailureReturnsWithoutPartialLinesOrLeaks) {
  fakeheap::external.fail = 1000;
  fakeheap::internal.largest = 1;
  bool ok = true;
  auto output = layout(0, 0, &ok);
  EXPECT_FALSE(ok);
  EXPECT_TRUE(output.empty());
  EXPECT_TRUE(fakeheap::live.empty());
}
#endif

// Word spaces shrink (up to ParsedText::SPACE_SHRINK_PERCENT) to fill a line
// before a word moves down, in any alignment, without overflowing the line.
TEST_F(TextLayoutBackingTest, SpacesShrinkToFitOneMoreWord) {
  for (const CssTextAlign alignment : {CssTextAlign::Left, CssTextAlign::Justify, CssTextAlign::Center}) {
    BlockStyle style;
    style.alignment = alignment;
    // Font 5 in the stub: 11 px per letter, 8 px spaces. "aa bb cc" is 82 px
    // with natural spaces; at 79 px it fits only with each space 2 px narrower.
    ParsedText text(false, false, /*hyphenationEnabled=*/true, false, false, 0, style);
    for (const char* word : {"aa", "bb", "cc"}) text.addWord(word, EpdFontFamily::REGULAR, false, false, false, 0, 0);
    GfxRenderer renderer;
    std::vector<std::shared_ptr<TextBlock>> lines;
    ASSERT_TRUE(text.layoutAndExtractLines(renderer, 5, 79, [&](std::shared_ptr<TextBlock> block, uint32_t, uint32_t) {
      lines.push_back(std::move(block));
    }));
    ASSERT_EQ(lines.size(), 1U) << static_cast<int>(alignment);
    ASSERT_EQ(lines[0]->wordCount(), 3U);
    EXPECT_GE(lines[0]->wordXpos(0), 0);
    EXPECT_LE(lines[0]->wordXpos(2) + 22, 79) << static_cast<int>(alignment);
  }
}
