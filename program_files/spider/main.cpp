#pragma comment(lib, "crypt32")

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

#include "http_utils.h"
#include "text_processing.h"
#include "db_client.h"

#include <functional>
#include <algorithm>

std::mutex mtx;
std::condition_variable cv;
std::queue<std::function<void()>> tasks;
bool exitThreadPool = false;

void threadPoolWorker() {
	std::unique_lock<std::mutex> lock(mtx);
	while (!exitThreadPool || !tasks.empty()) {
		if (tasks.empty()) {
			cv.wait(lock);
		}
		else {
			auto task = tasks.front();
			tasks.pop();
			lock.unlock();
			task();
			lock.lock();
		}
	}
}
void parseLink(const Link& link, Client& client, int depth, std::vector<std::string>& scrapped)
{
	try 
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		std::string html = getHtmlContent(link);

		if (html.size() == 0)
		{
			std::cout << "Failed to get HTML Content" << std::endl;
			return;
		}

		std::unordered_map<std::string, int> words = indexer(cleanTags(html));
		client.fillDatabase(link, words);

		if (depth > 1) 
		{
			std::vector<Link> links = assembleLinks(html, link);

			std::lock_guard<std::mutex> lock(mtx);

			size_t count = links.size();
			size_t index = 0;
			for (auto& subLink : links)
			{
				std::string new_url = stringulateLink(subLink);
				if (std::find(scrapped.begin(), scrapped.end(), new_url) == scrapped.end())
				{
					scrapped.push_back(new_url);
					tasks.push([subLink, &client, depth, &scrapped]() { parseLink(subLink, client, depth - 1, scrapped); });
				}
			}
			cv.notify_one();
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

}


int main()
{
	try
	{
		Config config("../../../../config/config.ini");
		Client client(config);
		int depth = atoi(config.getConfig("crawl_depth").c_str());
		std::vector<std::string> scrapped_urls;

		int numThreads = std::thread::hardware_concurrency();
		std::vector<std::thread> threadPool;
		for (int i = 0; i < numThreads; ++i) 
		{
			threadPool.emplace_back(threadPoolWorker);
		}

		Link link(prepareLink(config.getConfig("start_url")));
		scrapped_urls.push_back(config.getConfig("start_url"));

		{
			std::lock_guard<std::mutex> lock(mtx);
			tasks.push([link, &client, depth, &scrapped_urls]() { parseLink(link, client, depth, scrapped_urls); });
			cv.notify_one();
		}

		std::this_thread::sleep_for(std::chrono::seconds(2));

		{
			std::lock_guard<std::mutex> lock(mtx);
			exitThreadPool = true;
			cv.notify_all();
		}

		for (auto& t : threadPool) {
			t.join();
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}