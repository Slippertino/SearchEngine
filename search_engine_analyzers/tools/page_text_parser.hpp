#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "stemmer.hpp"
#include "en_de_coder.hpp"
#include "../stop_words/stop_words_container.hpp"

class page_text_parser {
private:
	static const std::unordered_map<se_language, std::shared_ptr<stop_words_container>> language_stop_words_interpreter;
	static const std::unordered_map<se_language, std::vector<std::pair<char, char>>> language_t_alf_ranges_interpreter;
	static const std::string delimeters;

private:
	std::shared_ptr<stop_words_container> stop_words;
	en_de_coder coder;
	stemmer stem_obj;
	std::vector<std::pair<char, char>> alf_ranges;

private:
	bool is_valid_word(const std::string& text) const {
		for (auto& ch : text) {
			bool is_included = false;
			for (auto& range : alf_ranges) {
				is_included |= (ch >= range.first && ch <= range.second);
			}
			if (!is_included) {
				return is_included;
			}
		}

		return !stop_words->is_stop_word(text);
	}

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
	page_text_parser() = delete;
	page_text_parser(se_language lang, se_encoding enc) :
		coder(enc),
		stem_obj(lang, enc),
		alf_ranges(language_t_alf_ranges_interpreter.at(lang)),
		stop_words(language_stop_words_interpreter.at(lang))
	{ }

	void parse(std::string text,
		       std::vector<std::string>& stemmed_words) const {
		coder.decode(text);

		while (!text.empty()) {
			skip_delimeters(text);

			if (!text.empty()) {
				auto word = get_word(text);
				if (is_valid_word(word)) {
					coder.encode(word);
					word = stem_obj.get_stem(word);
					coder.decode(word);
					stemmed_words.push_back(word);
				}
			}
		}
	}
};

const std::unordered_map<se_language, std::shared_ptr<stop_words_container>> page_text_parser::language_stop_words_interpreter = {
	{language_t::ENGLISH, std::make_shared<stop_words_container>(stop_words_container(language_t::ENGLISH))},
	{language_t::RUSSIAN, std::make_shared<stop_words_container>(stop_words_container(language_t::RUSSIAN))},
	{language_t::UNKNOWN, std::make_shared<stop_words_container>(stop_words_container(language_t::UNKNOWN))},
};

const std::unordered_map<se_language, std::vector<std::pair<char, char>>> page_text_parser::language_t_alf_ranges_interpreter = {
	{language_t::ENGLISH, {{'a', 'z'}, {'0', '9'}}},
	{language_t::RUSSIAN, {{'à', 'ÿ'}, {'0', '9'}}},
	{language_t::UNKNOWN, {{'a', 'z'}, {'0', '9'}}},
};

const std::string page_text_parser::delimeters = ".,?!;:-_=+'\"\\#$*()[]{}<>/|`~@%^& \n\r";