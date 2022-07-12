#pragma once

#include "se_pair_extension.hpp"
#include <algorithm>
#include <vector>
#include <unordered_set>
#include "stemmer.hpp"
#include "../text_property_types/se_encoding.hpp"
#include "../text_property_types/string_enc.hpp"
#include "text_parser.hpp"
#include "se_encoder.hpp"

class bold_snippet_builder {
private:
	std::vector<std::string> key_words;
	std::vector<std::unordered_map<std::string, std::string>> stemmed_snippets;
	std::vector<std::string> snippets;

	se_encoding output_encoding;

private:
	void make_bold(std::string& snippet, const std::string& word) {
		const std::string opened_tag = "<b>";
		const std::string closed_tag = "</b>";

		size_t st{ 0 };

		auto pos = snippet.find(word, st);
		while (pos != std::string::npos) {
			snippet.insert(pos, opened_tag);

			auto offset = pos + opened_tag.size() + word.size();
			snippet.insert(offset, closed_tag);

			st = offset + closed_tag.size();

			pos = snippet.find(word, st);
		}
	}

	void apply_key_to_snippet(size_t snippet_id, size_t key_id) {
		auto& st_snpt_words = stemmed_snippets[snippet_id];
		auto& key			= key_words[key_id];

		std::unordered_set<std::string> words_to_erase;

		for (auto& cur : st_snpt_words) {
			if (cur.second == key) {
				words_to_erase.insert(cur.first);
				make_bold(snippets[snippet_id], cur.first);
			}
		}

		for (auto it = st_snpt_words.begin(); it != st_snpt_words.end();) {
			if (words_to_erase.find(it->second) != words_to_erase.end()) {
				it = st_snpt_words.erase(it);
			} else {
				++it;
			}
		}
	}

public:
	bold_snippet_builder() = delete;
	bold_snippet_builder(se_encoding encoding) : output_encoding(encoding)
	{ }

	void load_snippet(const string_enc& snip, se_language lang) {
		auto snip_text = snip.str;
		se_encoder::encode(snip_text, snip.enc, DEFAULT_ENCODING);
		
		size_t snip_id{ snippets.size() };
		snippets.push_back(snip_text);
		stemmed_snippets.push_back({});

		std::vector<std::string> words;
		text_parser parser(DEFAULT_ENCODING);

		parser.parse(
			snip_text,
			[&words](const auto& str) { words.push_back(str); }
		);

		stemmer stmr(lang, DEFAULT_ENCODING);
		for (const auto& w : words) {
			stemmed_snippets[snip_id].insert( { w, stmr.get_stem(w) } );
		}

		for (auto i = 0; i < key_words.size(); ++i) {
			apply_key_to_snippet(snip_id, i);
		}
	}

	bold_snippet_builder& load_snippet_fluently(const string_enc& snip, se_language lang) {
		load_snippet(snip, lang);
		return *this;
	}

	void add_key_word(const string_enc& key, se_language lang) {
		stemmer stmr(lang, key.enc);
		auto stemmed = stmr.get_stem(key.str);

		se_encoder::encode(stemmed, key.enc, DEFAULT_ENCODING);
		
		size_t key_id{ key_words.size() };
		key_words.push_back(stemmed);

		for (auto i = 0; i < snippets.size(); ++i) {
			apply_key_to_snippet(i, key_id);
		}
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