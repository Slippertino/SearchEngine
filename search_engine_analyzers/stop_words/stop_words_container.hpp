#pragma once

#include <filesystem>
#include <unordered_set>
#include <string>
#include <unordered_map>
#include <fstream>
#include "../text_property_types/se_language.hpp"

namespace fs = std::filesystem;

class stop_words_container
{
private:
	static const fs::path lists_root_path;
	static const std::unordered_map<se_language, fs::path> words_language_storage_path_interpreter;

private:
	std::unordered_set<std::string> stop_words;

private:
	void remove_whitespaces(std::string& str) const {
		static const std::string whitespaces = " \n\r,.";
		size_t i{ 0 };
		while (i < str.size()) {
			if (whitespaces.find(str[i]) != std::string::npos) {
				str = str.erase(i, 1);
			} 
			else {
				++i;
			}
		}
	}

	void load_stop_words(se_language lang) {
		std::ifstream source;

		source.open(words_language_storage_path_interpreter.at(lang));

		if (source.is_open()) {
			std::string s;
			while (std::getline(source, s)) {
				remove_whitespaces(s);
				if (!s.empty()) {
					stop_words.insert(s);
				}
			}
		}

		source.close();
	}

public:
	stop_words_container() = delete;
	stop_words_container(se_language lang) {
		load_stop_words(lang);
	}

	bool is_stop_word(const std::string& word) {
		return stop_words.find(word) != stop_words.end();
	}

	void get_stop_words(std::unordered_set<std::string>& cont) {
		cont.insert(stop_words.begin(), stop_words.end());
	}
};

const fs::path stop_words_container::lists_root_path = fs::path(R"(..\..\..\search_engine_analyzers\stop_words\stop_words_lists)");

const std::unordered_map<se_language, fs::path> stop_words_container::words_language_storage_path_interpreter = {
	{language_t::ENGLISH, stop_words_container::lists_root_path / fs::path(R"(english.txt)")},
	{language_t::RUSSIAN, stop_words_container::lists_root_path / fs::path(R"(russian.txt)")},
	{language_t::UNKNOWN, stop_words_container::lists_root_path / fs::path(R"(unknown.txt)")},
};