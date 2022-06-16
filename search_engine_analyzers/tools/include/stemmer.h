#pragma once

#include <string>
#include <map>
#include "en_de_coder.h"
#include "libstemmer.h"
#include "converter_to_encoding_t.h"
#include "converter_to_language_t.h"

class stemmer
{
private:
	static const std::map<language_t, const char*> language_t_to_name_interpreter;
	static const std::map<encoding_t, const char*> encoding_t_to_name_interpreter;

private:
	sb_stemmer* stem_obj;
	en_de_coder external_coder;
	en_de_coder internal_coder;

private:
	const char* def_language_name(language_t) const;
	const char* def_encoding_name(encoding_t) const;

	void to_lower(std::string&) const;

public:
	stemmer() = delete;
	stemmer(language_t, encoding_t);

	std::string get_stem(std::string) const;

	~stemmer();
};