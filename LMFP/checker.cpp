#include <QString>
#include <regex>

bool checkUrl(const QString &url) {
	std::regex pattern("(https?://.*)");
	std::string urlStr = url.toStdString();
    return std::regex_match(urlStr, pattern);
}

bool checkExtensions(const std::string &ext) {
	std::regex pattern("^([\\w|]+)$");
	return std::regex_match(ext, pattern);
}