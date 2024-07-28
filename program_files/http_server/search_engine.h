#pragma once

#include "../database/database.h"
#include "../spider/text_processing.h"

#include <string>
#include <vector>

class Search : public Database
{
public:
	Search(const Config& config);
	std::vector<std::string> searchEngine(const std::string& text);
};
