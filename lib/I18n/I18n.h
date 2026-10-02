#pragma once

#include <atomic>
#include <cstdint>

#include "I18nKeys.h"
/**
 * Internationalization (i18n) system for CrossPoint Reader
 */

class I18n {
 public:
  static I18n& getInstance();

  // Disable copy
  I18n(const I18n&) = delete;
  I18n& operator=(const I18n&) = delete;

  // Get localized string by ID
  const char* get(StrId id) const;

  const char* operator[](StrId id) const { return get(id); }
  // The English string, e.g. where the current language's script cannot draw.
  static const char* getEnglish(StrId id);

  Language getLanguage() const { return _language; }
  void setLanguage(Language lang);

  // Languages in scripts the stock UI fonts lack (the Indic scripts, Sinhala)
  // need a font that has them: built in for one script per CrossInk-Indic
  // build, or an SD-card font's UI fallback. While none can draw the current
  // language, get() serves English instead of boxes; getLanguage() still
  // reports the language the reader chose.
  //
  // scriptProbe(): a letter of the language's script (0: Latin/Cyrillic/... ).
  static uint32_t scriptProbe(Language lang);
  static bool needsScriptFont(Language lang) { return scriptProbe(lang) != 0; }
  // Whether every UI font size can draw a codepoint (set by main.cpp).
  using ScriptFontCheck = bool (*)(uint32_t codepoint);
  void setScriptFontCheck(ScriptFontCheck check) { _scriptFontCheck = check; }
  // Re-evaluates the current language's script against the UI fonts. Called
  // when the UI fonts change; setLanguage() calls it too.
  void refreshScriptFont();
  bool isShowingFallbackStrings() const {
    return !_scriptFontAvailable.load(std::memory_order_relaxed) && needsScriptFont(_language);
  }
  const char* getLanguageName(Language lang) const;
  static Language languageFromCode(const char* code);

  // Get all unique characters used in a specific language
  // Returns a sorted string of unique characters
  static const char* getCharacterSet(Language lang);

 private:
  I18n() : _language(Language::EN) {}

  Language _language;
  std::atomic<bool> _scriptFontAvailable{false};
  ScriptFontCheck _scriptFontCheck = nullptr;
};

// Convenience macros
#define tr(id) I18n::getInstance().get(StrId::id)
#define I18N I18n::getInstance()
