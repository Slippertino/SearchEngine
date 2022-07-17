#pragma once

#include <vector>
#include <list>
#include <core/se_service.hpp>
#include <core/builders/se_service_builder.hpp>
#include <search_engine_analyzers/tools/text_parser.hpp>
#include <search_engine_analyzers/tools/word_analyzer.hpp>
#include <search_engine_analyzers/tools/stemmer.hpp>
#include <search_engine_analyzers/text_property_types/se_encoding.hpp>
#include <search_engine_analyzers/text_property_types/se_language.hpp>
#include <thread_extensions/thread_safe_containers/thread_safe_queue.hpp>
#include <thread_extensions/thread_safe_containers/thread_safe_unordered_map.hpp>
#include "../messages/pi_internal_messages.hpp"
#include "../pi_config.hpp"

class pi_page_indexing_service : public se_service<pi_page_indexing_service, pi_config> {
	SE_SERVICE(pi_page_indexing_service, page_indexing_service)

	#define CHECK_FOR_STOP(cont, arg) if (stop_flag) { cont.add(std::move(arg)); continue; }

private:
	using word_params     = std::tuple<std::string, se_language, html_text_analyzer::ratio_type>;
	using words_container = std::unordered_map<std::string, word_params>;
	
	struct page_indexing_info : context {
		size_t page_id;
		size_t site_id;
		std::vector<std::tuple<string_enc, string_enc, html_text_analyzer::ratio_type>> words;

		SE_CONTEXT(
			page_indexing_info, -1,
			page_id,
			site_id,
			words
		)

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(
			page_indexing_info, 
			page_id, 
			site_id, 
			words
		)
	};

private:
	thread_safe_queue<std::pair<std::string, std::string>> source_for_collect;
	thread_safe_queue<page_indexing_info> source_for_record;

	std::atomic<double> analysis_time_avg{ 0 };
	std::atomic<size_t> analyses_count{ 0 };

private:
	void page_indexing_request_responder(msg_request msg) {
		auto req = static_cast<page_indexing_request*>(msg.body.get());
		source_for_collect.add({req->url.str, req->prefix.str});

		response_status resp_status;
		resp_status.status  = runtime_status::SUCCESS;
		resp_status.message = { msg.body->to_string() + " was successfully added in a queue for indexing!\n", DEFAULT_ENCODING };

		MAKE_RESPONSE(page_indexing_response, (
			msg.id,
			resp_status
		))
	}

	void recording_info_handler() {
		while (!stop_flag) {
			page_indexing_info obj;
			pool.set_inactive(std::this_thread::get_id());
			extract_from_ts_queue(source_for_record, obj);
			SE_LOG("Extracted word params for record: " << obj.to_string() << "\n");
			pool.set_active(std::this_thread::get_id());

			std::vector<std::pair<string_enc, string_enc>> words_to_record(obj.words.size());
			for (auto i = 0; i < words_to_record.size(); ++i) {
				words_to_record[i] = { std::get<0>(obj.words[i]), std::get<1>(obj.words[i]) };
			}

			MAKE_REQUEST_WITH_RESPONSE(record_word_resp, record_word_info_request, record_word_info_response, (
				record_word_resp,
				obj.site_id,
				words_to_record
			))

			CHECK_FOR_STOP(source_for_record, obj)

			if (record_word_resp.status.status == runtime_status::FAIL) {
				continue;
			}

			std::vector<std::tuple<size_t, size_t, html_text_analyzer::ratio_type>> words_index_params(obj.words.size());
			for (auto i = 0; i < obj.words.size(); ++i) {
				words_index_params[i] = {  obj.page_id, record_word_resp.words_id[i], std::get<2>(obj.words[i]) };
			}

			MAKE_REQUEST(record_to_index_id, record_word_to_index_request, (
				words_index_params
			))

			CHECK_FOR_STOP(source_for_record, obj)
		}
	}

	void parse_excerpt(se_encoding encoding,
					   const word_params& excerpt, 
					   words_container& cont) {
		std::list<std::string> words;

		text_parser parser(encoding);
		parser.parse(
			std::get<0>(excerpt), 
			[&words](const auto& word) { 
				words.push_back(word); 
			}
		);

		words.remove_if([&excerpt, &encoding](const std::string& s) {
			word_analyzer analyzer(s, encoding);
			return !analyzer.is_valid_word(std::get<1>(excerpt));
		});

		stemmer stmr(std::get<1>(excerpt), encoding);
		for (auto& w : words) {
			w = stmr.get_stem(w);
			se_encoder::encode(w, encoding, DEFAULT_ENCODING);
		}

		for (auto& w : words) {
			if (cont.find(w) == cont.end()) {
				cont.insert({ w, { w, std::get<1>(excerpt), std::get<2>(excerpt) } });
			} else {
				std::get<2>(cont[w]) += std::get<2>(excerpt);
			}
		}
	}

	void collecting_info_handler() {
		while (!stop_flag) {
			std::pair<std::string, std::string> page_site_urls;

			pool.set_inactive(std::this_thread::get_id());
			extract_from_ts_queue(source_for_collect, page_site_urls);
			SE_LOG("Extracted data: " << page_site_urls.first << " ; " << page_site_urls.second << "\n");
			pool.set_active(std::this_thread::get_id());

			auto begin = std::chrono::steady_clock::now();

			MAKE_REQUEST(page_site_id_resp, page_and_site_id_request, (
				string_enc{ page_site_urls.first,   DEFAULT_ENCODING },
				string_enc{ page_site_urls.second,  DEFAULT_ENCODING }
			))

			MAKE_REQUEST(page_info_id_resp, page_info_request, (
				string_enc{ page_site_urls.first, DEFAULT_ENCODING }
			))

			GET_RESPONSE(page_site_id_resp, page_site_resp, page_and_site_id_response)
			GET_RESPONSE(page_info_id_resp, page_info_resp, page_info_response)

			CHECK_FOR_STOP(source_for_collect, page_site_urls)

			words_container container;
			for (auto& excerpt : page_info_resp.text_excerpts) {
				parse_excerpt(page_info_resp.page_encoding, excerpt, container);
			}

			page_indexing_info info;
			info.page_id = page_site_resp.page_id;
			info.site_id = page_site_resp.site_id;

			for (auto& w : container) {
				se_encoder::encode(std::get<0>(w.second), DEFAULT_ENCODING, page_info_resp.page_encoding);
				info.words.push_back({
					string_enc{ std::get<0>(w.second),						    page_info_resp.page_encoding },
					string_enc{ se_language(std::get<1>(w.second)).to_string(), encoding_t::UTF_8			 },
					std::get<2>(w.second)
				});
			}
			source_for_record.add(std::move(info));

			auto end = std::chrono::steady_clock::now();

			auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

			analysis_time_avg = (analysis_time_avg * analyses_count + time.count()) / (analyses_count + 1);
			++analyses_count;

			SE_LOG("End of analyze! Current time: " << time.count() << " ms ; " 
				 <<"Total: " << analyses_count << "; Avg time : " << analysis_time_avg << "\n");
		}
	}

protected:
	void setup_base(pi_config* config) override {}

	void clear() override final {
		se_service<pi_page_indexing_service, pi_config>::clear();
		source_for_collect.clear();
		source_for_record .clear();
	}
};

template<>
class se_builder_imp<pi_page_indexing_service>: public se_service_builder<pi_page_indexing_service> {
protected:
	void configure_own_network(const object_ptr& service) const override final {
		service
			->subscribe(service->internal_switch, typeid(page_indexing_response).name())
			->subscribe(service->internal_switch, typeid(page_and_site_id_request).name())
			->subscribe(service->internal_switch, typeid(page_info_request).name())
			->subscribe(service->internal_switch, typeid(record_word_info_request).name())
			->subscribe(service->internal_switch, typeid(record_word_to_index_request).name());
	}

	void configure_switches_network(const object_ptr& service) const override final {
		service->internal_switch
			->subscribe(service, typeid(page_indexing_request).name())
			->subscribe(service, typeid(page_and_site_id_response).name())
			->subscribe(service, typeid(page_info_response).name())
			->subscribe(service, typeid(record_word_info_response).name())
			->subscribe(service, typeid(record_word_to_index_response).name());
	}

	void add_request_responders(const object_ptr& service) const override final {
		service->responders = {
			{ typeid(page_indexing_request).name(), &pi_page_indexing_service::page_indexing_request_responder },
		};
	}

	void add_unused_response_type_names(const object_ptr& service) const override final {
		service->unused_response_type_names = {
			{ typeid(record_word_to_index_request).name(), nullptr },
		};
	}

	void add_power_distribution(const object_ptr& service) const override final {
		service->power_distribution = {
			{ &pi_page_indexing_service::collecting_info_handler,  10 },
			{ &pi_page_indexing_service::recording_info_handler,   10 },
			{ &pi_page_indexing_service::requests_handler,         1  },
		};
	}
};