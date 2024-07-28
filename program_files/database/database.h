#pragma once

#include <pqxx/pqxx>
#include "../config/config.h"

class Database
{
public:
	Database(const Config& config);
	void createTable();

protected:
	pqxx::connection c;
};