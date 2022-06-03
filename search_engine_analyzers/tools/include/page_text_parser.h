#pragma once

#include <map>
#include <string>
#include <vector>
#include "en_de_coder.h"
#include "converter_to_encoding_t.h"
#include "converter_to_language_t.h"
#include "stemmer.h"

class page_text_parser
{
private:
	static const std::map<language_t, std::pair<char, char>> language_t_alf_range_interpreter;
	static const std::string whitespaces;

private:
	en_de_coder coder;
	stemmer stem_obj;
	std::pair<char, char> alf_range;

private:
	bool is_valid_word(const std::string&);
	void skip_whitespaces(std::string&);
	std::string get_word(std::string&);

public:
	page_text_parser() = delete;
	page_text_parser(language_t, encoding_t);

	void parse(std::string&,
			   std::vector<std::string>&);
};