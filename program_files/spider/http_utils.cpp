#include "http_utils.h"

#include <regex>
#include <iostream>

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <openssl/ssl.h>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ip = boost::asio::ip;
namespace ssl = boost::asio::ssl;

using tcp = boost::asio::ip::tcp;

bool isText(const boost::beast::multi_buffer::const_buffers_type& b)
{
	for (auto itr = b.begin(); itr != b.end(); itr++)
	{
		for (int i = 0; i < (*itr).size(); i++)
		{
			if (*((const char*)(*itr).data() + i) == 0)
			{
				return false;
			}
		}
	}

	return true;
}

std::string getHtmlContent(const Link& link)
{

	std::string result;

	try
	{
		std::string host = link.hostName;
		std::string query = link.query;

		net::io_context ioc;

		if (link.protocol == ProtocolType::HTTPS)
		{

			ssl::context ctx(ssl::context::tlsv13_client);
			ctx.set_default_verify_paths();

			beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);
			stream.set_verify_mode(ssl::verify_none);

			stream.set_verify_callback([](bool preverified, ssl::verify_context& ctx) {
				return true; // Accept any certificate
				});


			if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
				beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
				throw beast::system_error{ec};
			}

			ip::tcp::resolver resolver(ioc);
			get_lowest_layer(stream).connect(resolver.resolve({ host, "https" }));
			get_lowest_layer(stream).expires_after(std::chrono::seconds(30));


			http::request<http::empty_body> req{http::verb::get, query, 11};
			req.set(http::field::host, host);
			req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

			stream.handshake(ssl::stream_base::client);
			http::write(stream, req);

			beast::flat_buffer buffer;
			http::response<http::dynamic_body> res;
			http::read(stream, buffer, res);

			if (isText(res.body().data()))
			{
				result = buffers_to_string(res.body().data());
			}
			else
			{
				std::cout << "This is not a text link, bailing out..." << std::endl;
			}

			beast::error_code ec;
			stream.shutdown(ec);
			if (ec == net::error::eof) {
				ec = {};
			}

			if (ec) {
				throw beast::system_error{ec};
			}
		}
		else
		{
			tcp::resolver resolver(ioc);
			beast::tcp_stream stream(ioc);

			auto const results = resolver.resolve(host, "http");

			stream.connect(results);

			http::request<http::string_body> req{http::verb::get, query, 11};
			req.set(http::field::host, host);
			req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);


			http::write(stream, req);

			beast::flat_buffer buffer;

			http::response<http::dynamic_body> res;


			http::read(stream, buffer, res);

			if (isText(res.body().data()))
			{
				result = buffers_to_string(res.body().data());
			}
			else
			{
				std::cout << "This is not a text link, bailing out..." << std::endl;
			}

			beast::error_code ec;
			stream.socket().shutdown(tcp::socket::shutdown_both, ec);

			if (ec && ec != beast::errc::not_connected)
				throw beast::system_error{ec};

		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return result;
}

Link prepareLink(const std::string& url)
{
	Link link;

	std::string protocol, hostName, query, temp;

	std::istringstream iss{ url };
	std::getline(iss, protocol, '/');
	if (protocol == "http:")
	{
		link.protocol = ProtocolType::HTTP;
	}
	else if (protocol == "https:")
	{
		link.protocol = ProtocolType::HTTPS;
	}

	std::getline(iss, temp, '/');
	std::getline(iss, temp);
	if (temp.find('/') != std::string::npos)
	{
		std::istringstream iss{ temp };
		std::getline(iss, hostName, '/');
		link.hostName = hostName;
		std::getline(iss, query);
		link.query = '/' + query;
	}
	else
	{
		link.hostName = temp;
	}

	return link;
}

std::vector<Link> assembleLinks(const std::string& html, const Link& link)
{
	static const std::regex reg("<a href=\"(.*?)\"", std::regex_constants::icase);
	std::vector<std::string> urls;
	std::copy(std::sregex_token_iterator(html.begin(), html.end(), reg, 1), std::sregex_token_iterator(), std::back_inserter(urls));

	std::vector<Link> links;
	for (auto& url : urls)
	{
		if (url.find('/') != std::string::npos)
		{
			if (url[0] == '/')
			{
				links.push_back({ link.protocol, link.hostName, url });
			}
			else
			{
				Link temp = prepareLink(url);
				links.push_back(temp);
			}
		}
	}
	
	return links;
}

std::string stringulateLink(const Link& link)
{
	std::string url;

	if (link.protocol == ProtocolType::HTTP)
	{
		url = "http://";
	}
	else if (link.protocol == ProtocolType::HTTPS)
	{
		url = "https://";
	}
	url = url + link.hostName + link.query;

	return url;
}

