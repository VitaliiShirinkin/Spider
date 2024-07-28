#include "config.h"

Config::Config(const std::string& file)
{
	Config::readFile(file);
}

std::string Config::getConfig(const std::string& key) const
{
	if (config_data.empty())
	{
		throw std::runtime_error("No configs found");
	} 
	return (config_data.at(key));
}

void Config::readFile(const std::string& file)
{
	std::ifstream read(file);
	if (!read.is_open())
	{
		throw std::runtime_error("Failed to read file");
	}

	for (std::string temp{}; std::getline(read, temp);)
	{
		if (temp.find('=') != std::string::npos) 
		{
			std::istringstream iss{ temp };
			std::string id, value;
			std::getline(iss, id, '=');
			std::getline(iss, value);
			config_data[id] = value;
		}
	}

	read.close();
}