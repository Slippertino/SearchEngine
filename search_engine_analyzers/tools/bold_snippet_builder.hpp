#pragma once

#include <vector>
#include "stemmer.hpp"
#include "../text_property_types/se_encoding.hpp"
#include "../text_property_types/string_enc.hpp"
#include "se_encoder.hpp"

class bold_snippet_builder {
private:
	std::vector<std::string> key_words;
	std::vector<std::vector<std::string>> stemmed_snippets;
	std::vector<std::string> snippets;

	se_encoding output_encoding;

private:


public:
	bold_snippet_builder() = delete;
	bold_snippet_builder(se_encoding encoding) : output_encoding(encoding)
	{ }


	void load_snippet(const string_enc& snip, se_language lang) {

	}

	bold_snippet_builder& load_snippet_fluently(const string_enc& snip, se_language lang) {
		load_snippet(snip, lang);
		return *this;
	}

	void add_key_word(const string_enc& key, se_language lang) {
		stemmer stmr(lang, key.enc);
		auto stemmed = stmr.get_stem(key.str);

		se_encoder::encode(stemmed, key.enc, DEFAULT_ENCODING);

		key_words.push_back(stemmed);


	}

	bold_snippet_builder& add_key_word_fluently(const string_enc& key, se_language lang) {
		add_key_word(key, lang);
		return *this;
	}

	string_enc build() const {
		string_enc res{ "", output_encoding };

		for (auto& snip : snippets)
			res.str += snip + " ";

		se_encoder::encode(res.str, DEFAULT_ENCODING, output_encoding);

		return res;
	}
};