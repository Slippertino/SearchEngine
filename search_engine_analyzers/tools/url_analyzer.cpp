#include "include/url_analyzer.h"

const std::vector<std::regex> url_analyzer::forbidden_patterns = {
	std::regex(R"(.*\.json$)"),
	std::regex(R"(.*\.css$)"),
	std::regex(R"(.*\.js$)"),
	std::regex(R"(.*\.xml$)"),
	std::regex(R"(.*\.png$)"),
	std::regex(R"(.*\.jpeg$)"),
	std::regex(R"(.*\.gif$)"),
	std::regex(R"(.*\.mp3$)"),
	std::regex(R"(.*\.mp4$)"),
	std::regex(R"(.*\.txt$)"),
};

url_analyzer::url_analyzer(const std::string& src, const std::string& rt) : source(src), root(rt)
{ }

std::string url_analyzer::get_protocol() const {
	return root.substr(0, root.find("://"));
}

bool url_analyzer::is_valid_content() const {
	const std::string forbidden_descriptors = "#?=&@% ";

	for (auto& ch : source)
		if (forbidden_descriptors.find(ch) != std::string::npos)
			return false;

	return true;
}

bool url_analyzer::is_root_contained() const {
	return source.find(root) == 0;
}

bool url_analyzer::is_file_extension_contained() const {
	for (auto& ptrn : forbidden_patterns)
		if (std::regex_match(source, ptrn))
			return true;

	return false;
}

bool url_analyzer::is_valid_url() const {
	return is_valid_content() &&
		   is_root_contained() &&
		   !is_file_extension_contained();
}

std::string url_analyzer::convert_to_url() {
	if (source.find(get_protocol())) {
		if (source.find('/') || source.size() == 1)
			source.clear();

		source = root + source;
	}

	if (source == root + "/")
		source.clear();

	return source;
}