#include "include/url_analyzer.h"

url_analyzer::url_analyzer(const std::string& src, const std::string& rt) : source(src), root(rt)
{ }

std::string url_analyzer::get_protocol() const {
	return root.substr(0, root.find("://"));
}

bool url_analyzer::is_valid_content() const {
	const std::string forbidden_descriptors = "#?=&@%";

	for (auto& ch : source)
		if (forbidden_descriptors.find(ch) != std::string::npos)
			return false;

	return true;
}

bool url_analyzer::is_root_contained() const {
	return source.find(root) == 0;
}

bool url_analyzer::is_file_extension_contained() const {
	const std::regex forbidden_pattern(R"(^([\w:]*)//([\w.]*)/([\w/]*)\.\w{0,10}$)");
	return std::regex_match(source, forbidden_pattern);
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