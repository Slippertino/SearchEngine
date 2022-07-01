#pragma once

#include <string>
#include <algorithm>
#include "se_encoder.hpp"

class text_parser {
private:
	static const std::string delimeters;

private:
	se_encoding text_encoding;

private:
	void skip_delimeters(std::string& text) const {
		for (auto i = 0; i < text.size(); ++i)
			if (delimeters.find(text[i]) == std::string::npos) {
				text = text.substr(i);
				return;
			}

		text.clear();
	}

	std::string get_word(std::string& text) const {
		std::string res = text;

		for (auto i = 0; i < text.size(); ++i)
			if (delimeters.find(text[i]) != std::string::npos) {
				res = text.substr(0, i);
				text = text.substr(i);
				break;
			}

		if (res == text) {
			text.clear();
		}

		return res;
	}

	void to_lower(std::string& text) const {
		std::transform(text.begin(), text.end(), text.begin(), ::tolower);
	}

public:
	text_parser() = delete;
	text_parser(se_encoding enc) : text_encoding(enc)
	{ }

	void parse(std::string text,
			   const std::function<void(const std::string& word)>& add_op) const {
		se_encoder::encode(text, text_encoding, DEFAULT_ENCODING);

		while (!text.empty()) {
			skip_delimeters(text);

			if (!text.empty()) {
				auto word = get_word(text);
				se_encoder::encode(word, DEFAULT_ENCODING, text_encoding);
				add_op(word);
			}
		}
	}
};

const std::string text_parser::delimeters = ".,?!;:-_=+'\'\"\\#$*()[]{}<>/|`~@%^& \n\r";