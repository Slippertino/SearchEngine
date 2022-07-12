#pragma once

#include "../text_property_types/se_encoding.hpp"
#include "../text_property_types/se_language.hpp"
#include "se_encoder.hpp"
#include "../stop_words/stop_words_container.hpp"

class word_analyzer {
private:
	static const std::unordered_map<se_language, std::shared_ptr<stop_words_container>> language_stop_words_interpreter;
	static const std::unordered_map<se_language, std::vector<std::pair<char, char>>> language_t_alf_ranges_interpreter;

private:
	std::string word;

private:
	bool is_word_suited_to_ranges(const std::vector<std::pair<char, char>>& ranges) const {
		for (auto& ch : word) {
			bool flag{ false };

			for (auto& cur_range : ranges) {
				flag |= (cur_range.first <= ch && ch <= cur_range.second);
			}

			if (!flag) {
				return false;
			}
		}

		return true;
	}

public:
	word_analyzer() = default;
	word_analyzer(const std::string& w, se_encoding enc) : word(w) {
		se_encoder::encode(word, enc, DEFAULT_ENCODING);
	}

	bool is_valid_word(se_language lang) const {
		return (lang == define_word_lang()) && !language_stop_words_interpreter.at(lang)->is_stop_word(word);
	}

	se_language define_word_lang() const {
		for (auto& cur : language_t_alf_ranges_interpreter) {
			if (is_word_suited_to_ranges(cur.second)) {
				return cur.first;
			}
		}

		return language_t::UNKNOWN;
	}
};

const std::unordered_map<se_language, std::shared_ptr<stop_words_container>> word_analyzer::language_stop_words_interpreter = {
	{language_t::ENGLISH, std::make_shared<stop_words_container>(language_t::ENGLISH)},
	{language_t::RUSSIAN, std::make_shared<stop_words_container>(language_t::RUSSIAN)},
	{language_t::UNKNOWN, std::make_shared<stop_words_container>(language_t::UNKNOWN)},
};

const std::unordered_map<se_language, std::vector<std::pair<char, char>>> word_analyzer::language_t_alf_ranges_interpreter = {
	{language_t::ENGLISH, {{'a', 'z'}, {'0', '9'}}},
	{language_t::RUSSIAN, {{'à', 'ÿ'}, {'0', '9'}}},
	{language_t::UNKNOWN, {}},
};