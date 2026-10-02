#pragma once

#include <Epub/Page.h>
#include <Epub/ParsedText.h>
#include <Epub/hyphenation/Hyphenator.h>
#include <GfxRenderer.h>
#include <Utf8.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <string>

class TouchReaderPreviewModel {
 public:
  static constexpr size_t TEXT_CAPACITY = 8U * 1024U;
  static constexpr size_t WORD_CAPACITY = 256;
  static constexpr size_t LINE_CAPACITY = 128;

  bool capture(const Page& page, const GfxRenderer& renderer, const int fontId, const uint8_t lineHeightPercent,
               const int xOffset = 0, const int yOffset = 0) {
    clear();
    sourceXOffset = xOffset;
    sourceYOffset = yOffset;
    sourceLineHeightPixels =
        static_cast<int16_t>(std::max(1, (renderer.getLineHeight(fontId) * lineHeightPercent + 50) / 100));
    bool previousElementWasLine = false;
    bool joinNextLine = false;
    for (const auto& element : page.elements) {
      if (!element || element->getTag() != TAG_PageLine) {
        previousElementWasLine = false;
        joinNextLine = false;
        continue;
      }
      if (lineCount >= lines.size()) break;
      const auto& pageLine = static_cast<const PageLine&>(*element);
      const auto& block = pageLine.getBlock();
      if (!block) continue;
      if (wordCount + block->wordCount() > words.size()) break;

      size_t blockTextSize = 0;
      for (uint16_t i = 0; i < block->wordCount(); ++i) {
        blockTextSize += static_cast<size_t>(block->wordTextLen(i)) + 1U;
      }
      if (blockTextSize > text.size() - textSize) break;

      const bool continuesSplitWord = joinNextLine && lineCount > 0;
      Line& line = lines[lineCount++];
      line.x = pageLine.xPos;
      line.y = pageLine.yPos;
      line.sourceBlock = block;
      line.firstWord = wordCount;
      line.wordCount = block->wordCount();
      line.style = block->getBlockStyle();
      line.startsParagraph =
          !previousElementWasLine || lineCount == 1 || startsNewParagraph(lines[lineCount - 2], line);
      if (!hasBaseline) {
        firstLineY = line.y;
        hasBaseline = true;
      }

      for (uint16_t i = 0; i < block->wordCount(); ++i) {
        const uint16_t textLength = block->wordTextLen(i);
        if (i == 0 && continuesSplitWord && !line.startsParagraph && textSize > 1 && text[textSize - 2] == '-') {
          // The page split this word across lines ("ιν-" / "τσών"); the preview
          // reflows whole words, so join them back without the inserted hyphen.
          textSize -= 2;
          std::memcpy(text.data() + textSize, block->wordText(i), textLength);
          textSize += textLength;
          text[textSize++] = '\0';
          continue;
        }
        Word& word = words[wordCount++];
        word.textOffset = textSize;
        word.x = block->wordXpos(i);
        word.style = block->wordStyle(i);
        word.focusBoundary = block->focusBoundary(i);
        word.hasSpaceBefore = block->wordHasSpaceBefore(i);
        std::memcpy(text.data() + textSize, block->wordText(i), textLength);
        textSize += textLength;
        text[textSize++] = '\0';
        if (!word.hasSpaceBefore && i > 0 && wordCount >= line.firstWord + 2) {
          const Word& previous = words[wordCount - 2];
          const int attachedX = previous.x + wordAdvance(renderer, fontId, previous, previous.focusBoundary != 0) +
                                renderer.getKerning(fontId, lastCodepoint(wordText(previous)),
                                                    firstCodepoint(wordText(word)), previous.style);
          // Some blocks do not report every visible word gap. Recover one
          // only when the rendered source positions prove it was present.
          word.hasSpaceBefore = word.x > attachedX || block->guideDotXOffset(i - 1) > 0;
        }
      }
      line.wordCount = static_cast<uint16_t>(wordCount - line.firstWord);
      joinNextLine = block->wordCount() > 0 && block->wordEndsWithInsertedHyphen(block->wordCount() - 1);
      previousElementWasLine = true;
    }
    return hasBaseline && wordCount > 0;
  }

  void renderText(const GfxRenderer& renderer, const int fontId, const int xOffset, const int yOffset,
                  const int contentWidth, const uint8_t lineHeightPercent, const uint8_t wordSpacing,
                  const uint8_t paragraphAlignment, const bool focusReadingEnabled, const bool guideReadingEnabled,
                  const bool foregroundBlack) const {
    if (!valid()) return;
    const int currentLineHeight = std::max(1, (renderer.getLineHeight(fontId) * lineHeightPercent + 50) / 100);
    int y = firstLineY + yOffset;
    for (size_t paragraphStart = 0; paragraphStart < lineCount;) {
      size_t paragraphEnd = paragraphStart + 1;
      while (paragraphEnd < lineCount && !lines[paragraphEnd].startsParagraph) ++paragraphEnd;

      const Line& line = lines[paragraphStart];
      const uint16_t firstWord = line.firstWord;
      const uint16_t paragraphWordEnd = lines[paragraphEnd - 1].firstWord + lines[paragraphEnd - 1].wordCount;
      const int leftInset = std::max(0, static_cast<int>(line.style.leftInset()));
      const int rightInset = std::max(0, static_cast<int>(line.style.rightInset()));
      const int availableLeft = xOffset + leftInset;
      const int availableWidth = std::max(1, contentWidth - leftInset - rightInset);
      CssTextAlign alignment = paragraphAlignment < static_cast<uint8_t>(CssTextAlign::None)
                                   ? static_cast<CssTextAlign>(paragraphAlignment)
                                   : line.style.alignment;
      if (alignment == CssTextAlign::None) alignment = CssTextAlign::Justify;

      Position position{firstWord, 0};
      bool firstPreviewLine = true;
      while (position.word < paragraphWordEnd) {
        const int firstLineIndent = firstPreviewLine ? previewFirstLineIndent(renderer, fontId, line, alignment) : 0;
        const int lineWidthLimit = std::max(1, availableWidth - firstLineIndent);
        PreviewLine previewLine;
        const Position next = reflowLine(renderer, fontId, position, paragraphWordEnd, lineWidthLimit, wordSpacing,
                                         focusReadingEnabled, guideReadingEnabled, previewLine);
        renderReflowedLine(renderer, fontId, previewLine, y, availableLeft, availableWidth, firstLineIndent, alignment,
                           next.word >= paragraphWordEnd, wordSpacing, focusReadingEnabled, guideReadingEnabled,
                           foregroundBlack);
        position = next;
        firstPreviewLine = false;
        y += currentLineHeight;
      }

      if (paragraphEnd < lineCount) {
        const int sourceGap = lines[paragraphEnd].y - lines[paragraphEnd - 1].y - sourceLineHeightPixels;
        if (sourceGap > 0) y += sourceGap * currentLineHeight / std::max(1, static_cast<int>(sourceLineHeightPixels));
      }
      paragraphStart = paragraphEnd;
    }
  }

  // Keep the source blocks alive so unchanged settings use the reader's exact
  // rendering, including justification, ruby, bidi, and focus run positions.
  // TextBlock::render writes pixels through the renderer, so this must remain
  // a mutable reference despite cppcheck not seeing that dependency.
  // cppcheck-suppress constParameterReference
  void renderSource(GfxRenderer& renderer, const int fontId, const bool foregroundBlack) const {
    if (!valid()) return;
    for (size_t i = 0; i < lineCount; ++i) {
      const auto& line = lines[i];
      line.sourceBlock->render(renderer, fontId, sourceXOffset + line.x, sourceYOffset + line.y, foregroundBlack);
    }
  }

  bool valid() const { return hasBaseline && lineCount > 0 && wordCount > 0; }

 private:
  struct Word {
    uint16_t textOffset = 0;
    int16_t x = 0;
    EpdFontFamily::Style style = EpdFontFamily::REGULAR;
    uint8_t focusBoundary = 0;
    bool hasSpaceBefore = false;
  };

  struct Line {
    std::shared_ptr<TextBlock> sourceBlock;
    int16_t x = 0;
    int16_t y = 0;
    uint16_t firstWord = 0;
    uint16_t wordCount = 0;
    BlockStyle style{};
    bool startsParagraph = true;
  };

  std::array<char, TEXT_CAPACITY> text{};
  std::array<Word, WORD_CAPACITY> words{};
  std::array<Line, LINE_CAPACITY> lines{};
  uint16_t textSize = 0;
  uint16_t wordCount = 0;
  uint16_t lineCount = 0;
  int sourceXOffset = 0;
  int sourceYOffset = 0;
  int16_t firstLineY = 0;
  int16_t sourceLineHeightPixels = 1;
  bool hasBaseline = false;

  static constexpr char GUIDE_DOT_UTF8[] = "\xc2\xb7";
  static constexpr uint32_t GUIDE_DOT_CODEPOINT = 0x00B7;

  const char* wordText(const Word& word) const { return text.data() + word.textOffset; }

  bool startsNewParagraph(const Line& previous, const Line& current) const {
    if (current.y <= previous.y ||
        current.y - previous.y > sourceLineHeightPixels + std::max<int>(2, sourceLineHeightPixels / 3)) {
      return true;
    }
    return previous.style.leftInset() != current.style.leftInset() ||
           previous.style.rightInset() != current.style.rightInset() ||
           previous.style.alignment != current.style.alignment ||
           previous.style.textIndent != current.style.textIndent ||
           previous.style.textIndentDefined != current.style.textIndentDefined ||
           previous.style.isRtl != current.style.isRtl;
  }

  int previewFirstLineIndent(const GfxRenderer& renderer, const int fontId, const Line& line,
                             const CssTextAlign alignment) const {
    const bool naturalAlignment = alignment == CssTextAlign::Justify || alignment == CssTextAlign::Left;
    if (!naturalAlignment || line.wordCount == 0 || words[line.firstWord].x <= 0) return 0;
    if (line.style.textIndentDefined) return std::max(0, static_cast<int>(line.style.textIndent));
    return renderer.getSpaceWidth(fontId, EpdFontFamily::REGULAR) * 3;
  }

  // Where the reflow stands: a word, and how far into it (a word split at the end
  // of the previous preview line continues from its split point).
  struct Position {
    uint16_t word = 0;
    uint16_t offset = 0;
  };
  // One drawn piece of a preview line: a whole word, the rest of a split word,
  // or a word's start up to a hyphenation point.
  struct Piece {
    uint16_t word = 0;
    uint16_t start = 0;
    uint16_t end = 0;
    bool hyphen = false;
    int16_t width = 0;
  };
  struct PreviewLine {
    static constexpr size_t MAX_PIECES = 96;
    std::array<Piece, MAX_PIECES> pieces{};
    size_t count = 0;
    int width = 0;
  };

  bool wholeWord(const Piece& piece) const {
    return piece.start == 0 && !piece.hyphen && piece.end == std::strlen(wordText(words[piece.word]));
  }

  std::string pieceText(const Piece& piece) const {
    std::string value(wordText(words[piece.word]) + piece.start, piece.end - piece.start);
    if (piece.hyphen) value.push_back('-');
    return value;
  }

  int pieceAdvance(const GfxRenderer& renderer, const int fontId, const Piece& piece, const bool focusEnabled) const {
    const Word& word = words[piece.word];
    if (wholeWord(piece)) return wordAdvance(renderer, fontId, word, focusEnabled);
    return renderer.getTextAdvanceX(fontId, pieceText(piece).c_str(), word.style);
  }

  // Lays out one preview line from `from` as the reader does (ParsedText's
  // hyphenated breaker): whole words while they fit, then the widest
  // hyphenated start of the next word, with word spaces allowed to shrink by
  // ParsedText::SPACE_SHRINK_PERCENT.
  Position reflowLine(const GfxRenderer& renderer, const int fontId, const Position from,
                      const uint16_t paragraphWordEnd, const int availableWidth, const uint8_t wordSpacing,
                      const bool focusEnabled, const bool guideReadingEnabled, PreviewLine& out) const {
    out.count = 0;
    out.width = 0;
    int shrinkAllowance = 0;
    Position at = from;
    while (at.word < paragraphWordEnd && out.count < PreviewLine::MAX_PIECES) {
      const Word& word = words[at.word];
      const char* value = wordText(word);
      const uint16_t length = static_cast<uint16_t>(std::strlen(value));
      int gap = 0;
      int gapShrink = 0;
      if (out.count > 0) {
        gap = wordGap(renderer, fontId, words[at.word - 1], word, wordSpacing, guideReadingEnabled);
        if (word.hasSpaceBefore) gapShrink = gap * ParsedText::SPACE_SHRINK_PERCENT / 100;
      }
      Piece piece{at.word, at.offset, length, false, 0};
      piece.width = static_cast<int16_t>(pieceAdvance(renderer, fontId, piece, focusEnabled));
      const int limit = availableWidth + shrinkAllowance + gapShrink;
      if (out.count == 0 || out.width + gap + piece.width <= limit) {
        out.pieces[out.count++] = piece;
        out.width += gap + piece.width;
        shrinkAllowance += gapShrink;
        at = Position{static_cast<uint16_t>(at.word + 1), 0};
        if (out.count == 1 && out.width > availableWidth && !splitToFit(renderer, fontId, out, availableWidth)) {
          continue;
        }
        if (out.count == 1 && out.pieces[0].end < length) return Position{out.pieces[0].word, out.pieces[0].end};
        continue;
      }
      // Overflow: put the widest hyphenated start of this word on the line.
      const int room = limit - out.width - gap;
      if (room > 0) {
        Piece best{};
        bool found = false;
        for (const auto& info : Hyphenator::breakOffsets(std::string(value), false)) {
          if (info.byteOffset <= at.offset || info.byteOffset >= length) continue;
          Piece candidate{at.word, at.offset, static_cast<uint16_t>(info.byteOffset), info.requiresInsertedHyphen, 0};
          candidate.width = static_cast<int16_t>(pieceAdvance(renderer, fontId, candidate, false));
          if (candidate.width <= room && (!found || candidate.width > best.width)) {
            best = candidate;
            found = true;
          }
        }
        if (found) {
          out.pieces[out.count++] = best;
          out.width += gap + best.width;
          return Position{at.word, best.end};
        }
      }
      break;
    }
    return at;
  }

  // A first piece wider than the line: split it at its widest fitting
  // hyphenation point, as the reader would. False when no point fits.
  bool splitToFit(const GfxRenderer& renderer, const int fontId, PreviewLine& out, const int availableWidth) const {
    Piece& piece = out.pieces[0];
    const char* value = wordText(words[piece.word]);
    Piece best{};
    bool found = false;
    for (const auto& info : Hyphenator::breakOffsets(std::string(value), true)) {
      if (info.byteOffset <= piece.start || info.byteOffset >= piece.end) continue;
      Piece candidate{piece.word, piece.start, static_cast<uint16_t>(info.byteOffset), info.requiresInsertedHyphen, 0};
      candidate.width = static_cast<int16_t>(pieceAdvance(renderer, fontId, candidate, false));
      if (candidate.width <= availableWidth && (!found || candidate.width > best.width)) {
        best = candidate;
        found = true;
      }
    }
    if (!found) return false;
    piece = best;
    out.width = best.width;
    return true;
  }

  void renderReflowedLine(const GfxRenderer& renderer, const int fontId, const PreviewLine& line, const int y,
                          const int availableLeft, const int availableWidth, const int firstLineIndent,
                          const CssTextAlign alignment, const bool isLastLine, const uint8_t wordSpacing,
                          const bool focusEnabled, const bool guideReadingEnabled, const bool foregroundBlack) const {
    if (line.count == 0) return;
    int spaceCount = 0;
    for (size_t i = 1; i < line.count; ++i) spaceCount += words[line.pieces[i].word].hasSpaceBefore;

    // Same spacing as ParsedText::extractLine: justified lines spread their
    // spare space; a line filled past its natural width shrinks its gaps.
    const int spare = availableWidth - firstLineIndent - line.width;
    int justifyExtra = 0;
    if (spaceCount > 0 && spare != 0 && ((alignment == CssTextAlign::Justify && !isLastLine) || spare < 0)) {
      justifyExtra = spare > 0 ? spare / spaceCount : -((-spare + spaceCount - 1) / spaceCount);
    }
    const int drawnWidth = line.width + (justifyExtra < 0 ? justifyExtra * spaceCount : 0);
    int targetLeft = availableLeft;
    if (alignment == CssTextAlign::Center) {
      targetLeft += std::max(0, (availableWidth - drawnWidth) / 2);
    } else if (alignment == CssTextAlign::Right) {
      targetLeft += std::max(0, availableWidth - drawnWidth);
    } else {
      targetLeft += firstLineIndent;
    }

    int wordX = targetLeft;
    for (size_t i = 0; i < line.count; ++i) {
      const Piece& piece = line.pieces[i];
      const Word& word = words[piece.word];
      if (i > 0) {
        const Word& previous = words[line.pieces[i - 1].word];
        const int gap = wordGap(renderer, fontId, previous, word, wordSpacing, guideReadingEnabled);
        if (guideReadingEnabled && word.hasSpaceBefore) {
          const int extra = wordSpacingExtra(wordSpacing);
          const int firstGap =
              renderer.getSpaceAdvance(fontId, lastCodepoint(wordText(previous)), GUIDE_DOT_CODEPOINT, previous.style);
          renderer.drawText(fontId, wordX + firstGap + extra / 2, y, GUIDE_DOT_UTF8, foregroundBlack,
                            EpdFontFamily::REGULAR);
        }
        wordX += gap + (word.hasSpaceBefore ? justifyExtra : 0);
      }
      if (wholeWord(piece)) {
        drawWord(renderer, fontId, wordX, y, word, focusEnabled, foregroundBlack);
      } else {
        renderer.drawText(fontId, wordX, y, pieceText(piece).c_str(), foregroundBlack, word.style);
      }
      wordX += piece.width;
    }
  }

  static uint32_t firstCodepoint(const char* value) {
    const auto* cursor = reinterpret_cast<const unsigned char*>(value);
    while (true) {
      const uint32_t codepoint = utf8NextCodepoint(&cursor);
      if (codepoint != 0x00AD) return codepoint;
    }
  }

  static uint32_t lastCodepoint(const char* value) {
    const size_t length = std::strlen(value);
    if (length == 0) return 0;
    size_t offset = length - 1;
    while (offset > 0 && (static_cast<uint8_t>(value[offset]) & 0xC0) == 0x80) --offset;
    const auto* cursor = reinterpret_cast<const unsigned char*>(value + offset);
    return utf8NextCodepoint(&cursor);
  }

  static bool isFocusWordCharacter(const uint32_t codepoint) {
    if (codepoint < 128) {
      return ((codepoint | 0x20) >= 'a' && (codepoint | 0x20) <= 'z') || codepoint == '\'';
    }
    if (codepoint >= 0x2000 && codepoint <= 0x2BFF) return codepoint == 0x2018 || codepoint == 0x2019;
    if (codepoint >= 0x00A1 && codepoint <= 0x00BF)
      return codepoint == 0x00AA || codepoint == 0x00B5 || codepoint == 0x00BA;
    if (codepoint >= 0x2E00 && codepoint <= 0x2E7F) return false;
    return codepoint != 0x02D7 && codepoint != 0xFE63 && codepoint != 0xFF0D;
  }

  uint8_t resolvedFocusBoundary(const Word& word, const bool enabled) const {
    if (!enabled || (word.style & EpdFontFamily::BOLD) != 0) return 0;
    if (word.focusBoundary != 0) return word.focusBoundary;
    const char* value = wordText(word);
    const auto* cursor = reinterpret_cast<const unsigned char*>(value);
    size_t characters = 0;
    while (*cursor != '\0') {
      const auto* const start = cursor;
      if (!isFocusWordCharacter(utf8NextCodepoint(&cursor)) || cursor <= start) break;
      ++characters;
    }
    if (characters == 0) return 0;
    const size_t boldCharacters = std::clamp<size_t>((characters * 43) / 100, 1, 9);
    if (boldCharacters >= characters) return 0;
    cursor = reinterpret_cast<const unsigned char*>(value);
    for (size_t i = 0; i < boldCharacters; ++i) utf8NextCodepoint(&cursor);
    return static_cast<uint8_t>(std::min<size_t>(cursor - reinterpret_cast<const unsigned char*>(value), UINT8_MAX));
  }

  int wordAdvance(const GfxRenderer& renderer, const int fontId, const Word& word, const bool focusEnabled) const {
    const char* value = wordText(word);
    const uint8_t boundary = resolvedFocusBoundary(word, focusEnabled);
    if (boundary == 0 || boundary >= std::strlen(value)) return renderer.getTextAdvanceX(fontId, value, word.style);
    char prefix[40];
    const size_t length = std::min<size_t>({static_cast<size_t>(boundary), sizeof(prefix) - 1, std::strlen(value)});
    std::memcpy(prefix, value, length);
    prefix[length] = '\0';
    const auto boldStyle = static_cast<EpdFontFamily::Style>(word.style | EpdFontFamily::BOLD);
    return renderer.getTextAdvanceX(fontId, prefix, boldStyle, firstCodepoint(value + length)) +
           renderer.getTextAdvanceX(fontId, value + length, word.style);
  }

  static int wordSpacingExtra(const uint8_t wordSpacing) { return std::min<uint8_t>(wordSpacing, 4) * 10; }

  int wordGap(const GfxRenderer& renderer, const int fontId, const Word& left, const Word& right,
              const uint8_t wordSpacing, const bool guideReadingEnabled) const {
    const uint32_t leftCodepoint = lastCodepoint(wordText(left));
    const uint32_t rightCodepoint = firstCodepoint(wordText(right));
    if (!right.hasSpaceBefore) return renderer.getKerning(fontId, leftCodepoint, rightCodepoint, left.style);
    const int extra = wordSpacingExtra(wordSpacing);
    if (!guideReadingEnabled) {
      return renderer.getSpaceAdvance(fontId, leftCodepoint, rightCodepoint, left.style) + extra;
    }
    return renderer.getSpaceAdvance(fontId, leftCodepoint, GUIDE_DOT_CODEPOINT, left.style) +
           renderer.getTextAdvanceX(fontId, GUIDE_DOT_UTF8, EpdFontFamily::REGULAR) +
           renderer.getSpaceAdvance(fontId, GUIDE_DOT_CODEPOINT, rightCodepoint, EpdFontFamily::REGULAR) + extra;
  }

  void drawWord(const GfxRenderer& renderer, const int fontId, const int x, const int y, const Word& word,
                const bool focusEnabled, const bool foregroundBlack) const {
    const char* value = wordText(word);
    const uint8_t boundary = resolvedFocusBoundary(word, focusEnabled);
    if (boundary == 0 || boundary >= std::strlen(value)) {
      renderer.drawText(fontId, x, y, value, foregroundBlack, word.style);
      return;
    }
    char prefix[40];
    const size_t length = std::min<size_t>({static_cast<size_t>(boundary), sizeof(prefix) - 1, std::strlen(value)});
    std::memcpy(prefix, value, length);
    prefix[length] = '\0';
    const auto boldStyle = static_cast<EpdFontFamily::Style>(word.style | EpdFontFamily::BOLD);
    renderer.drawText(fontId, x, y, prefix, foregroundBlack, boldStyle);
    renderer.drawText(fontId, x + renderer.getTextAdvanceX(fontId, prefix, boldStyle, firstCodepoint(value + length)),
                      y, value + length, foregroundBlack, word.style);
  }

  void clear() {
    for (size_t i = 0; i < lineCount; ++i) lines[i].sourceBlock.reset();
    textSize = 0;
    wordCount = 0;
    lineCount = 0;
    firstLineY = 0;
    sourceLineHeightPixels = 1;
    hasBaseline = false;
  }
};
