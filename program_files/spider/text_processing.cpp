#include "text_processing.h"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/locale/conversion.hpp>
#include <boost/locale/generator.hpp>

#include <sstream>
#include <regex>

std::string cleanRegex(const std::string& text, const std::string& reg)
{
	std::string replacement = " ";
	std::string result = boost::replace_all_regex_copy(text, boost::regex(reg), replacement);
	return result;
}

std::string lowerCase(const std::string& text)
{
	boost::locale::generator gen;
	std::locale loc = gen("");
	std::locale::global(loc);

	std::string result = boost::locale::to_lower(boost::locale::normalize(text, boost::locale::norm_nfc));
	return result;
}

std::unordered_map<std::string, int> indexer(const std::string& text)
{
	std::string reg = "[^A-Za-zР-пр-џ_]";
	std::string processed_text = lowerCase(cleanRegex(text, reg));

	std::unordered_map<std::string, int> result;
	
	std::istringstream iss{ processed_text };
	std::string word;
	
	while (std::getline(iss, word, ' '))
	{
		if (!word.empty())
		{
			size_t length = word.length();
			if (length > 2 && length < 33)
			{
				if (result.find(word) == result.end())
				{
					result[word] = 1;
				}
				else
				{
					++result[word];
				}
			}
		}
	}
	return result;
}

std::string cleanTags(const std::string& text)
{
	std::regex reg("<.*?>");
	std::string replacement = "";
	std::string result = std::regex_replace(text, reg, replacement);
	return result;
}