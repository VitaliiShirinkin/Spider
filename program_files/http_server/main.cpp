#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

#include "http_connection.h"
#include "search_engine.h"
#include "../config/config.h"

#include <Windows.h>


void httpServer(tcp::acceptor& acceptor, tcp::socket& socket, Search& search)
{
	acceptor.async_accept(socket,
		[&](beast::error_code ec)
		{
			if (!ec)
				std::make_shared<HttpConnection>(std::move(socket), search)->start();
			httpServer(acceptor, socket, search);
		});
}


int main(int argc, char* argv[])
{
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	try
	{
		Config config("../../../../config/config.ini");
		Search search(config);
		auto const address = net::ip::make_address("0.0.0.0");
		unsigned short port = atoi(config.getConfig("port").c_str());

		net::io_context ioc{1};

		tcp::acceptor acceptor{ioc, { address, port }};
		tcp::socket socket{ioc};
		httpServer(acceptor, socket, search);

		std::cout << "Open browser and connect to http://localhost:8080 to see the web server operating" << std::endl;

		ioc.run();
	}
	catch (std::exception const& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}