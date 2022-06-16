#include "include/page_text_parser.h"

const std::string page_text_parser::whitespaces = " .,?!;:'\"\\#$*()[]{}<>/|`~@%^&";

const std::map<language_t, std::pair<char, char>> page_text_parser::language_t_alf_range_interpreter = {
	{language_t::ENGLISH, {'a', 'z'}},
	{language_t::RUSSIAN, {'à', 'ÿ'}},
	{language_t::UNKNOWN, {'a', 'z'}},
};

page_text_parser::page_text_parser(language_t lang, encoding_t enc) : coder(enc),
																	  stem_obj(lang, enc),
																	  alf_range(language_t_alf_range_interpreter.at(lang))
{ }

bool page_text_parser::is_valid_word(const std::string& text) const {

	for (auto& ch : text)
		if (ch < alf_range.first || ch > alf_range.second)
			return false;

	return true;
}

void page_text_parser::skip_whitespaces(std::string& text) const {
	for (auto i = 0; i < text.size(); ++i)
		if (whitespaces.find(text[i]) == std::string::npos) {
			text = text.substr(i);
			break;
		}
}

std::string page_text_parser::get_word(std::string& text) const {
	std::string res;

	for (auto i = 0; i < text.size(); ++i)
		if (whitespaces.find(text[i]) == std::string::npos) {
			res = text.substr(0, i);
			text = text.substr(i);
			break;
		}

	return res;
}

void page_text_parser::parse(std::string text,
							 std::vector<std::string>& stemmed_words) const {
	coder.decode(text);

	while (!text.empty()) {
		skip_whitespaces(text);

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