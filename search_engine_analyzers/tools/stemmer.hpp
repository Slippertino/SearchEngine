#pragma once

#include <string>
#include <unordered_map>
#include "../libs/libstemmer/include/libstemmer.h"
#include "en_de_coder.hpp"
#include "../text_property_types/se_encoding.hpp"
#include "../text_property_types/se_language.hpp"

class stemmer
{
private:
	static const std::unordered_map<se_language, const char*> language_t_to_name_interpreter;
	static const std::unordered_map<se_encoding, const char*> encoding_t_to_name_interpreter;

private:
	sb_stemmer* stem_obj;
	en_de_coder external_coder;
	en_de_coder internal_coder;

private:
	const char* def_language_name(se_language lang) const {
		if (language_t_to_name_interpreter.find(lang) != language_t_to_name_interpreter.cend())
			return language_t_to_name_interpreter.at(lang);
		else
			return (*language_t_to_name_interpreter.begin()).second;
	}

	const char* def_encoding_name(se_encoding encoding) const {
		if (encoding_t_to_name_interpreter.find(encoding) != encoding_t_to_name_interpreter.cend())
			return encoding_t_to_name_interpreter.at(encoding);
		else
			return (*encoding_t_to_name_interpreter.begin()).second;
	}

	void to_lower(std::string& text) const {
		std::transform(text.begin(), text.end(), text.begin(), ::tolower);
	}

public:
	stemmer() = delete;
	stemmer(se_language lang, se_encoding enc) : stem_obj(sb_stemmer_new(def_language_name(lang),
																		 def_encoding_name(enc))),
												 external_coder(enc),
												 internal_coder(encoding_t::UTF_8)
	{ }

	std::string get_stem(std::string word) const {
		external_coder.decode(word);
		to_lower(word);
		internal_coder.encode(word);

		auto stemmed = sb_stemmer_stem(
			stem_obj,
			reinterpret_cast<const sb_symbol*>(word.c_str()),
			word.size()
		);

		std::string res = std::string(reinterpret_cast<const char*>(
			sb_stemmer_stem(stem_obj,
							reinterpret_cast<const sb_symbol*>(word.c_str()),
							word.size()))
		);

		internal_coder.decode(res);
		external_coder.encode(res);

		return res;
	}

	stemmer::~stemmer() {
		sb_stemmer_delete(stem_obj);
	}
};

const std::unordered_map<se_encoding, const char*> stemmer::encoding_t_to_name_interpreter = {
	{encoding_t::UTF_8,   nullptr},
	{encoding_t::UNKNOWN, nullptr},
};

const std::unordered_map<se_language, const char*> stemmer::language_t_to_name_interpreter = {
	{language_t::ENGLISH, "english"},
	{language_t::RUSSIAN, "russian"},
	{language_t::UNKNOWN, "english"},
};