#pragma once

#include <string>
#include <regex>
#include <unordered_set>

class url_analyzer
{
private:
	std::string source;
	std::string root;

private:
	std::string get_protocol() const;

	bool is_valid_content() const;
	bool is_root_contained() const;
	bool is_file_extension_contained() const;

public:
	url_analyzer() = delete;
	url_analyzer(const std::string& src, const std::string& rt);

	bool is_valid_url() const;
	std::string convert_to_url();
};