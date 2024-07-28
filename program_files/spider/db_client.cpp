#include "db_client.h"
#include "http_utils.h"

Client::Client(const Config& config) : Database(config)
{
	c.prepare("insert_word", "INSERT INTO words (word) VALUES ($1) RETURNING id;");
	c.prepare("insert_doc", "INSERT INTO docs (url) VALUES ($1) RETURNING id;");
	c.prepare("insert_word_doc", "INSERT INTO words_docs (word_id, doc_id, word_counter) VALUES ($1, $2, $3) RETURNING id;");
	c.prepare("doc_finder", "SELECT id FROM docs WHERE url=$1");
	c.prepare("word_finder", "SELECT id FROM words WHERE word=$1");
	c.prepare("delete_word_doc", "DELETE FROM words_docs WHERE doc_id=$1");
}

int Client::findDoc(const std::string& url)
{
	pqxx::work tx{ c };
	pqxx::result res = tx.exec_prepared("doc_finder", url);
	tx.commit();

	if (!res.empty())
	{
		return res[0][0].as<int>();
	}

	return 0;
}

int Client::findWord(const std::string& word)
{
	pqxx::work tx{ c };
	pqxx::result res = tx.exec_prepared("word_finder", word);
	tx.commit();

	if (!res.empty())
	{
		return res[0][0].as<int>();
	}

	return 0;
}

void Client::fillDatabase(const Link& link, const std::unordered_map <std::string, int>& word_counter)
{
	std::lock_guard<std::mutex> lock(mutex_);

	std::string url = stringulateLink(link);
	int doc_id = findDoc(url);

	if (doc_id > 0)
	{
		pqxx::work wd_tx{ c };
		wd_tx.exec_prepared("delete_word_doc", doc_id);
		wd_tx.commit();
	}
	else
	{
		pqxx::work d_tx{ c };
		pqxx::result res = d_tx.exec_prepared("insert_doc", url);
		d_tx.commit();
		if (!res.empty())
		{
			doc_id = res[0][0].as<int>();
		}
		else
		{
			doc_id = 0;
		}
	}

	if (doc_id > 0)
	{
		for (auto w : word_counter)
		{
			int word_id = findWord(w.first);
			pqxx::work w_tx{ c };
			if (word_id == 0)
			{
				pqxx::result res = w_tx.exec_prepared("insert_word", w.first);
				word_id = res[0][0].as<int>();
			}
			w_tx.exec_prepared("insert_word_doc", word_id, doc_id, w.second);
			w_tx.commit();
		}
	}
}
