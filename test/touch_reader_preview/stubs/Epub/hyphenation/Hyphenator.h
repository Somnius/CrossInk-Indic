#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <vector>

// Break points come from a table the tests fill in.
class Hyphenator {
 public:
  struct BreakInfo {
    size_t byteOffset;
    bool requiresInsertedHyphen;
  };

  static std::map<std::string, std::vector<BreakInfo>>& table() {
    static std::map<std::string, std::vector<BreakInfo>> breaks;
    return breaks;
  }

  static std::vector<BreakInfo> breakOffsets(const std::string& word, bool) {
    const auto it = table().find(word);
    return it == table().end() ? std::vector<BreakInfo>{} : it->second;
  }
};
