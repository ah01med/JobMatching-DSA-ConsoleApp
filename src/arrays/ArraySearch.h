#pragma once
#include <string>


namespace arrayalgo {
// Case‑insensitive substring search using KMP
bool containsKeywordCI(const std::string& text, const std::string& pattern);
}