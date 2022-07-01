#pragma once

#include <list>
#include <text_property_types/se_language.hpp>
#include <tools/text_parser.hpp>
#include <tools/stemmer.hpp>
#include <tools/word_analyzer.hpp>
#include <tools/html_text_analyzer.hpp>
#include "../../se_service.hpp"
#include "../s_config.hpp"
#include "../s_messages.hpp"
#include <tools/bold_snippet_builder.hpp>

class s_search_service : public se_service<s_search_service, s_config> {

	SE_SERVICE(s_search_service)

private:
	struct page_relevance_estimation_info : context {
		size_t page_id;
		std::vector<size_t> words_ids;

		SE_CONTEXT(
			page_relevance_estimation_info,
			page_id,
			words_ids
		)

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(
			page_relevance_estimation_info,
			page_id,
			words_ids
		)
	};

private:
	std::vector<std::pair<string_enc, string_enc>> key_words;

	thread_safe_unordered_map<size_t, size_t> employed_sites_id;
	thread_safe_unordered_map<std::thread::id, size_t> thread_employed_site_id;

	thread_safe_queue<page_relevance_estimation_info> source_for_analyze;

	std::mutex selection_mutex;

private:
	void analyzing_pages_relevance_handler() {
		while (!stop_flag) {
			page_relevance_estimation_info info;

			pool.set_inactive(std::this_thread::get_id());
			extract_from_ts_queue(source_for_analyze, info);
			SE_LOG("Extracted new page relevance estimation info: " << info.to_string() << "\n");
			pool.set_active(std::this_thread::get_id());

			MAKE_REQUEST(page_content_id_req, page_content, (
				info.page_id
			))
			MAKE_REQUEST(page_relevance_id_req, page_relevance, (
				info.page_id,
				info.words_ids
			))

			GET_RESPONSE(page_content_id_req, page_content_info, page_content_response)
			GET_RESPONSE(page_relevance_id_req, page_relevance_info, page_relevance_response)

			analyzed_page_info_request req;
			req.url = page_content_info.url;
			req.rank = page_relevance_info.rank;

			html_text_analyzer parser(string_enc { page_content_info.content.str, DEFAULT_ENCODING });
			html_text_analyzer::page_info page_info;
			parser.run_parse(page_info);

			req.title = string_enc{ page_info.title, page_info.page_encoding };

			bold_snippet_builder snippet_builder(DEFAULT_ENCODING);
			for (auto& cur : page_info.text_excerpts) {
				snippet_builder.load_snippet(
					string_enc{ std::get<0>(cur), page_info.page_encoding },
					std::get<1>(cur)
				);
			}

			for (auto& cur : key_words) {
				snippet_builder.add_key_word(cur.first, se_language(cur.second.str));
			}

			req.snippet = snippet_builder.build();

			MAKE_REQUEST(analyzed_page_info_id, analyzed_page_info, (
				std::move(req)
			))
		}
	}

	void pages_selection_handler() {
		size_t site_id;

		pool.set_inactive(std::this_thread::get_id());
		get_from_ts_unordered_map(thread_employed_site_id, std::this_thread::get_id(), id);
		SE_LOG("Extracted site_id for selecting pages: " << id << "\n");
		pool.set_active(std::this_thread::get_id());

		MAKE_REQUEST_WITH_RESPONSE(words_info_resp, words_info, (
			words_info_resp,
			site_id,
			key_words
		))

		auto& words = words_info_resp.words_id_freq;
		std::sort(words.begin(), words.end(), [](const auto& v1, const auto& v2) {
			return v1.second < v2.second;
		});
		
		std::unordered_set<size_t> allowed_ids;
		for (auto i = 1; i <= words_info_resp.pages_count; ++i)
			allowed_ids.insert(i);

		for (auto& cur : words) {
			MAKE_REQUEST_WITH_RESPONSE(pages_selection_resp, pages_selection, (
				pages_selection_resp,
				cur.first,
				std::move(allowed_ids)
			))

			allowed_ids = std::move(pages_selection_resp.allowed_pages_ids);
		}
		
		std::vector<size_t> words_ids;
		std::transform(words.cbegin(), words.cend(), words_ids.begin(), [](const auto& val) {
			return val.first;
		});

		for (auto& v : allowed_ids) {
			source_for_analyze.add(page_relevance_estimation_info(
				v,
				words_ids
			));
		}

		analyzing_pages_relevance_handler();
	}

	void distribution_block_handler() {
		SE_LOG("Distributing process started for flow " << std::this_thread::get_id() << "\n");

		MAKE_REQUEST_WITH_RESPONSE(sites_id_resp, sites_id, (
			sites_id_resp
		))

		bool is_emplaced{ false };
		for (auto i = 0; i < sites_id_resp.ids.size() && !is_emplaced; ++i) {
			auto id = sites_id_resp.ids[i];
			while (employed_sites_id.find(id) == employed_sites_id.end()) {
				if (employed_sites_id.find(id) == employed_sites_id.end() && selection_mutex.try_lock()) {
					employed_sites_id.insert(id, id);
					thread_employed_site_id.insert(std::this_thread::get_id(), id);
					is_emplaced = true;
					selection_mutex.unlock();
				}
			}
		}

		if (!is_emplaced) {
			SE_LOG("Flow " << std::this_thread::get_id() << " added for analyzing pages!\n");
			analyzing_pages_relevance_handler();
		} else {
			SE_LOG("Flow " << std::this_thread::get_id() << " added for selecting suitable pages!\n");
			pages_selection_handler();
		}
	}


protected:
	void clear() override {
		se_service<s_search_service, s_config>::clear();
		key_words.clear();
		thread_employed_site_id.clear();
		employed_sites_id.clear();
		source_for_analyze.clear();
	}

	void setup_base(s_config* config) override {
		auto key_words_enc = config->get_encoding();

		text_parser parser(key_words_enc);
		
		std::vector<std::string> words;
		parser.parse(
			config->get_query(),
			[&words](const auto& word) {
				words.push_back(word);
			}
		);

		std::for_each(words.begin(), words.end(), [&](const std::string& word) {
			auto w_lang = word_analyzer(word, key_words_enc).define_word_lang();

			if (w_lang.get_type() != language_t::UNKNOWN) {
				auto stmd_word = stemmer(w_lang, key_words_enc).get_stem(word);
				key_words.push_back({ 
					string_enc{ stmd_word,          key_words_enc     },
					string_enc{ w_lang.to_string(), encoding_t::UTF_8 }
				});
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
		service->router->subscribe<sites_id_response>(service);
		service->router->subscribe<words_info_response>(service);
	}

	void add_power_distribution(const service_ptr& service) const override {
		service->power_distribution.push_back({ &s_search_service::distribution_block_handler,  1 });
	}

	void add_request_responders(const service_ptr& service) const override {
		return;
	}

	void add_unused_response_type_names(const service_ptr& service) const override {
		service->unused_response_type_names.insert(typeid(analyzed_page_info_request).name(), nullptr);
	}
};