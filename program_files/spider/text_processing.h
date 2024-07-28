#pragma once

#include <string>
#include <unordered_map>

std::string cleanRegex(const std::string& text, const std::string& reg);
std::string lowerCase(const std::string& text);
std::unordered_map<std::string, int> indexer(const std::string& text);
std::string cleanTags(const std::string& text);