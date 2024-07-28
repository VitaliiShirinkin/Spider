#include "search_engine.h"
#include "../spider/text_processing.h"

Search::Search(const Config& config) : Database(config) { }

std::vector<std::string> Search::searchEngine(const std::string& text)
{
	std::string query = "SELECT d.url, sum(wd.word_counter) FROM docs d INNER JOIN words_docs wd ON d.id = wd.doc_id INNER JOIN words w ON w.id = wd.word_id WHERE w.word IN (";

	std::unordered_map<std::string, int> words = indexer(text);
	std::vector<std::string> result;
	std::vector<std::string> w;
	int i = 1;
	for (const auto& word : words)
	{
		if (i > 1)
		{
			query.append(",");
		}
		query.append("$").append(std::to_string(i));
		w.push_back(word.first);
		++i;
	}
	query.append(") GROUP BY d.url ORDER BY sum(wd.word_counter) DESC LIMIT 10 ");

	if (i > 1)
	{
		pqxx::work s_tx{ c };
		pqxx::result res;
		res = s_tx.exec_params(query, pqxx::prepare::make_dynamic_params(w));


		for (const auto& doc : res)
		{
			result.push_back(doc["url"].as<std::string>());
		}
	}
	return result;
}
