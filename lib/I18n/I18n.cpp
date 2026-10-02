#include "I18n.h"

#include <cstddef>
#include <cstring>

#include "I18nStrings.h"

using namespace i18n_strings;

namespace {
// The top bit of an offset means "this string is the English one, at this
// offset in the English blob". Languages too big for 16-bit offsets use 32.
const char* resolveString(const LangStrings& lang, const size_t index) {
  if (lang.wideOffsets != nullptr) {
    const uint32_t off = lang.wideOffsets[index];
    if (off & 0x80000000u) return STRINGS_EN_DATA + (off & 0x7FFFFFFFu);
    return lang.data + off;
  }
  const uint16_t off = lang.offsets[index];
  if (off & 0x8000) return STRINGS_EN_DATA + (off & 0x7FFF);
  return lang.data + off;
}

bool isBuiltinLanguage(const Language language) {
  const auto raw = static_cast<uint8_t>(language);
  for (const uint8_t builtin : SORTED_LANGUAGE_INDICES) {
    if (builtin == raw) return true;
  }
  return false;
}
}  // namespace

I18n& I18n::getInstance() {
  static I18n instance;
  return instance;
}

const char* I18n::get(StrId id) const {
  const auto index = static_cast<size_t>(id);
  if (index >= static_cast<size_t>(StrId::_COUNT)) {
    return "???";
  }

  // Use generated helper function - no hardcoded switch needed!
  const LangStrings lang = getLanguageStrings(isShowingFallbackStrings() ? Language::EN : _language);

  return resolveString(lang, index);
}

const char* I18n::getEnglish(StrId id) {
  const auto index = static_cast<size_t>(id);
  if (index >= static_cast<size_t>(StrId::_COUNT)) return "???";
  return resolveString(getLanguageStrings(Language::EN), index);
}

void I18n::setLanguage(Language lang) {
  if (lang >= Language::_COUNT) {
    return;
  }
  // Keep persisted settings untouched, but make every runtime language-dependent
  // behavior agree with the English string fallback in reduced-language builds.
  _language = isBuiltinLanguage(lang) ? lang : Language::EN;
  refreshScriptFont();
}

namespace {
// Language code -> a consonant of its script, and the English name the
// language picker shows while no font can draw the native one.
struct ScriptLanguage {
  const char* code;
  uint32_t probe;
  const char* englishName;
};
constexpr ScriptLanguage kScriptLanguages[] = {
    {"HI", 0x0915, "Hindi"},    {"MAR", 0x0915, "Marathi"}, {"NE", 0x0915, "Nepali"},   {"BN", 0x0995, "Bengali"},
    {"AS", 0x0995, "Assamese"}, {"PA", 0x0A15, "Punjabi"},  {"GU", 0x0A95, "Gujarati"}, {"OR", 0x0B15, "Odia"},
    {"TA", 0x0B95, "Tamil"},    {"TE", 0x0C15, "Telugu"},   {"KN", 0x0C95, "Kannada"},  {"ML", 0x0D15, "Malayalam"},
    {"SIN", 0x0D9A, "Sinhala"},
};

const ScriptLanguage* scriptLanguage(const Language lang) {
  const auto index = static_cast<size_t>(lang);
  if (index >= static_cast<size_t>(Language::_COUNT)) return nullptr;
  for (const auto& entry : kScriptLanguages) {
    if (strcmp(LANGUAGE_CODES[index], entry.code) == 0) return &entry;
  }
  return nullptr;
}
}  // namespace

uint32_t I18n::scriptProbe(const Language lang) {
  const ScriptLanguage* entry = scriptLanguage(lang);
  return entry ? entry->probe : 0;
}

void I18n::refreshScriptFont() {
  const uint32_t probe = scriptProbe(_language);
  _scriptFontAvailable.store(probe == 0 || (_scriptFontCheck != nullptr && _scriptFontCheck(probe)),
                             std::memory_order_relaxed);
}

const char* I18n::getLanguageName(Language lang) const {
  const auto index = static_cast<size_t>(lang);
  if (index >= static_cast<size_t>(Language::_COUNT)) {
    return "???";
  }
  // Without a font for its script the native name would draw as boxes.
  if (const ScriptLanguage* entry = scriptLanguage(lang)) {
    if (_scriptFontCheck == nullptr || !_scriptFontCheck(entry->probe)) return entry->englishName;
  }
  return LANGUAGE_NAMES[index];
}

Language I18n::languageFromCode(const char* code) {
  for (uint8_t i = 0; i < getLanguageCount(); i++) {
    if (strcmp(code, LANGUAGE_CODES[i]) == 0) return static_cast<Language>(i);
  }
  return Language::EN;
}

// Generate character set for a specific language
const char* I18n::getCharacterSet(Language lang) {
  const auto langIndex = static_cast<size_t>(lang);
  if (langIndex >= static_cast<size_t>(Language::_COUNT)) {
    lang = Language::EN;  // Fallback to first language
  }

  return CHARACTER_SETS[static_cast<size_t>(lang)];
}
