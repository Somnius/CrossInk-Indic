// Tamil, Telugu, Kannada and Malayalam words hyphenate with their script's
// patterns whatever the book language is, and only between syllables.
#include <IndicScripts.h>
#include <Utf8.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "Epub/hyphenation/Hyphenator.h"

namespace {

std::vector<std::string> prefixes(const std::string& word) {
  std::vector<std::string> out;
  for (const auto& info : Hyphenator::breakOffsets(word, false)) out.push_back(word.substr(0, info.byteOffset));
  return out;
}

// Every break falls between syllables, with at least 3 codepoints each side.
void expectSyllableBreaks(const std::string& word) {
  const auto infos = Hyphenator::breakOffsets(word, false);
  ASSERT_FALSE(infos.empty()) << word;
  for (const auto& info : infos) {
    const auto* p = reinterpret_cast<const unsigned char*>(word.c_str());
    const auto* const split = p + info.byteOffset;
    uint32_t prev = 0;
    size_t before = 0;
    while (p < split) {
      prev = utf8NextCodepoint(&p);
      ++before;
    }
    const uint32_t next = utf8NextCodepoint(&p);
    size_t after = 1;
    while (utf8NextCodepoint(&p) != 0) ++after;
    EXPECT_TRUE(indic::syllableBreakAllowed(prev, next)) << word << " @" << info.byteOffset;
    EXPECT_GE(before, 3u) << word;
    EXPECT_GE(after, 3u) << word;
    EXPECT_TRUE(info.requiresInsertedHyphen) << word;
  }
}

}  // namespace

TEST(IndicHyphenation, UsesTheScriptWhateverTheBookLanguage) {
  Hyphenator::setPreferredLanguage("en");
  EXPECT_EQ(prefixes("உடுப்பியிலிருந்து"), (std::vector<std::string>{"உடுப்பி", "உடுப்பியி", "உடுப்பியிலி"}));
  Hyphenator::setPreferredLanguage("");
}

TEST(IndicHyphenation, BreaksOnlyBetweenSyllables) {
  Hyphenator::setPreferredLanguage("");
  for (const char* word : {"உடுப்பியிலிருந்து", "சுப்பிரமணிய", "తెరవఁబడలేదు", "కూర్చోవడమేమిటి", "ಆಳಿಕೊಂಡಿದ್ದರು", "ಅರಸರಾಗಿದ್ದರು",
                           "സൂചിപ്പിച്ചതും", "പഞ്ചുമേനവനും", "കോപിഷ്ഠന്റെ"}) {
    expectSyllableBreaks(word);
  }
}

TEST(IndicHyphenation, KeepsConjunctsWhole) {
  Hyphenator::setPreferredLanguage("");
  // No break may leave a virama at the end of a piece (that would split a conjunct).
  for (const char* word : {"ಪ್ರಾಂತ್ಯಕ್ಕೆ", "അദ്ധ്യായത്തിൽ", "కూర్చోవడమేమిటి"}) {
    for (const auto& prefix : prefixes(word)) {
      const auto* p = reinterpret_cast<const unsigned char*>(prefix.c_str());
      uint32_t last = 0;
      while (const uint32_t cp = utf8NextCodepoint(&p)) last = cp;
      EXPECT_FALSE(indic::isVirama(last)) << word << " -> " << prefix;
    }
  }
}

TEST(IndicHyphenation, DevanagariAndBengaliStayUnhyphenated) {
  Hyphenator::setPreferredLanguage("");
  EXPECT_TRUE(prefixes("धर्मशास्त्रविशारद").empty());
  EXPECT_TRUE(prefixes("আকাঙ্ক্ষাপূর্ণভাবে").empty());
}
