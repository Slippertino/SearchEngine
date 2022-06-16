#include "include/stemmer.h"

const std::map<encoding_t, const char*> stemmer::encoding_t_to_name_interpreter = {
	{encoding_t::UTF_8,   nullptr},
	{encoding_t::UNKNOWN, nullptr},
};

const std::map<language_t, const char*> stemmer::language_t_to_name_interpreter = {
	{language_t::ENGLISH, "english"},
	{language_t::RUSSIAN, "russian"},
	{language_t::UNKNOWN, "english"},
};

stemmer::stemmer(language_t lang, encoding_t enc) : stem_obj(sb_stemmer_new(def_language_name(lang),
																			def_encoding_name(enc))),
													external_coder(enc)
{ }

const char* stemmer::def_language_name(language_t lang) const {
	if (language_t_to_name_interpreter.find(lang) != language_t_to_name_interpreter.cend())
		return language_t_to_name_interpreter.at(lang);
	else
		return (*language_t_to_name_interpreter.begin()).second;
}

const char* stemmer::def_encoding_name(encoding_t encoding) const {
	if (encoding_t_to_name_interpreter.find(encoding) != encoding_t_to_name_interpreter.cend())
		return encoding_t_to_name_interpreter.at(encoding);
	else
		return (*encoding_t_to_name_interpreter.begin()).second;
}

void stemmer::to_lower(std::string& text) const {
	std::transform(text.begin(), text.end(), text.begin(), ::tolower);
}

std::string stemmer::get_stem(std::string word) const {
	to_lower(word);

	external_coder.decode(word);
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

	internal_coder.decode(word);
	external_coder.encode(word);

	return res;
}

stemmer::~stemmer() {
	sb_stemmer_delete(stem_obj);
}