#pragma once

#include <string>
#include <regex>
#include <unordered_set>
#include "se_encoder.hpp"
#include "../text_property_types/se_encoding.hpp"

class url_analyzer {
private:
	static const se_encoding internal_encoding;
	static const std::vector<std::regex> forbidden_patterns;

private:
	se_encoding external_encoding;

private:
	std::string get_protocol(const std::string& url) const {
		const std::string protocol_quantifier = "://";
		return url.substr(0, url.find(protocol_quantifier) + protocol_quantifier.size());
	}

	bool is_valid_content(const std::string& source) const {
		const std::string forbidden_descriptors = "#?=&@% ";

		for (auto& ch : source) {
			if (forbidden_descriptors.find(ch) != std::string::npos) {
				return false;
			}
		}

		return true;
	}

	bool is_valid_structure(const std::string& source) const {
		const std::regex valid_pattern = std::regex(R"(^https?://.*$)");
		return std::regex_match(source, valid_pattern);
	}

	bool is_file_extension_contained(const std::string& source) const {
		for (auto& ptrn : forbidden_patterns) {
			if (std::regex_match(source, ptrn)) {
				return true;
			}
		}

		return false;
	}

public:
	url_analyzer() = delete;
	url_analyzer(se_encoding enc) : external_encoding(enc) 
	{ }

	bool is_valid_url(std::string url) const {
		se_encoder::encode(url, external_encoding, internal_encoding);
		return is_valid_structure(url) &&
			   is_valid_content(url) &&
			   !is_file_extension_contained(url);
	}

	bool is_child(const std::string& url, const std::string& root) const {
		return url.find(root) == 0;
	}

	std::string convert_to_url(std::string path, std::string root) {
		se_encoder::encode(path, external_encoding, internal_encoding);
		se_encoder::encode(root, external_encoding, internal_encoding);

		if (path.find(get_protocol(root))) {
			if (path.find('/') || path.size() == 1) {
				path.clear();
			}

			path = root + path;
		}

		se_encoder::encode(path, internal_encoding, external_encoding);

		return path;
	}

	std::string get_root(std::string url) const {
		se_encoder::encode(url, external_encoding, internal_encoding);

		auto offset = get_protocol(url).size();
		auto rel_path_beg = url.find('/', offset);

		auto res = (rel_path_beg == std::string::npos) 
							? url
							: url.substr(0, rel_path_beg);

		se_encoder::encode(res, internal_encoding, external_encoding);

		return res;
	}
};

const se_encoding url_analyzer::internal_encoding = DEFAULT_ENCODING;

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
	std::regex(R"(.*\.zip$)"),
};