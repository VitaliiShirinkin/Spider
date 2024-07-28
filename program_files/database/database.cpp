#include "database.h"

Database::Database(const Config& config) : c("host=" + config.getConfig("db_host") +
											 " port=" + config.getConfig("db_port") +
											 " dbname=" + config.getConfig("db_name") +
											 " user=" + config.getConfig("db_user") +
											 " password=" + config.getConfig("db_password"))
{
	createTable();
}

void Database::createTable()
{
	pqxx::work ct{ c };

	ct.exec("CREATE TABLE IF NOT EXISTS words (id serial primary key, word varchar(32) not null); ");
	ct.exec("CREATE TABLE IF NOT EXISTS docs (id serial primary key, url text not null); ");
	ct.exec("CREATE TABLE IF NOT EXISTS words_docs (id serial primary key, word_id integer not null references words(id), doc_id integer not null references docs(id), word_counter integer not null); ");
	ct.commit();
}