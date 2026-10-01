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

  // Scripts the built-in UI fonts lack (Devanagari) need an SD-card font. While
  // none is loaded, get() serves English for those languages instead of boxes;
  // getLanguage() still reports the language the reader chose.
  static bool needsScriptFont(Language lang);
  // Written when UI fonts load (main loop), read while rendering (render task).
  void setScriptFontAvailable(bool available) { _scriptFontAvailable.store(available, std::memory_order_relaxed); }
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
};

// Convenience macros
#define tr(id) I18n::getInstance().get(StrId::id)
#define I18N I18n::getInstance()
