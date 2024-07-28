#pragma once

#include <exception>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

class Config
{
public:
	Config() {};
	Config(const std::string& file);
	std::string getConfig(const std::string& key) const;	

protected:
	std::unordered_map<std::string, std::string> config_data;
	void readFile(const std::string& file);
};