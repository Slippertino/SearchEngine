#pragma once

#include <list>
#include <text_property_types/se_language.hpp>
#include <tools/text_parser.hpp>
#include <tools/word_analyzer.hpp>
#include "../../se_service.hpp"
#include "../s_config.hpp"

class s_search_service : public se_service<s_search_service, s_config> {

	SE_SERVICE(s_search_service)

private:
	std::vector<std::pair<std::string, se_language>> key_words;
	thread_safe_unordered_map<std::string, std::pair<std::string, size_t>> employed_sites;
	thread_safe_queue<size_t> pages_id;

private:

protected:
	void clear() override {
		se_service<s_search_service, s_config>::clear();
		key_words.clear();
		employed_sites.clear();
		pages_id.clear();
	}

	void setup_base(s_config* config) override {
		auto enc = config->get_encoding();

		text_parser parser(enc);
		
		std::vector<std::string> words;
		parser.parse<std::vector<std::string>>(
			config->get_query(),
			words,
			[](std::vector<std::string>& cont, const std::string& word) {
				cont.push_back(word);
			}
		);

		std::for_each(words.begin(), words.end(), [&](const std::string& word) {
			auto w_lang = word_analyzer(word, enc).define_word_lang();

			if (w_lang.get_type() != language_t::UNKNOWN) {
				key_words.push_back({ word, w_lang });
			}
		});
	}

public:
	s_search_service() = delete;
	s_search_service(size_t id, const fs::path& root, const std::shared_ptr<se_router>& in_router) :
		se_service(id, root / R"(services)" / get_full_name(get_component_name()), in_router)
	{ }

	std::string get_component_name() const override {
		return std::string("search_service");
	}
};

template<>
class builder<s_search_service> : public abstract_service_builder<s_search_service> {
protected:
	void add_subscriptions(const service_ptr& service) const override {

	}

	void add_power_distribution(const service_ptr& service) const override {

	}

	void add_request_responders(const service_ptr& service) const override {

	}

	void add_unused_response_type_names(const service_ptr& service) const override {

	}
};