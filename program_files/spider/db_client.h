#pragma once

#include "../database/database.h"
#include "link.h"

#include <mutex>
#include <regex>
#include <string>
#include <unordered_map>

class Client : public Database
{
public:
	Client(const Config& config);
	int findDoc(const std::string& url);
	int findWord(const std::string& word);
	void fillDatabase(const Link& link, const std::unordered_map <std::string, int>& word_counter);
protected:
	std::mutex mutex_;
};