#pragma once

#include <vector>
#include <tools/page_text_parser.hpp>
#include <text_property_types/se_encoding.hpp>
#include <text_property_types/se_language.hpp>
#include <tools/stemmer.hpp>
#include <thread_safe_containers/thread_safe_queue.hpp>
#include <thread_safe_containers/thread_safe_unordered_map.hpp>
#include "../../se_service.hpp"
#include "../pi_messages.h"

class pi_page_indexing_service : public se_service<pi_page_indexing_service> {
	SE_SERVICE(pi_page_indexing_service)

	#define CHECK_FOR_STOP(cont, arg) if (stop_flag) { cont.add(std::move(arg)); continue; }

private:
	using word_params     = std::tuple<std::string, se_language, html_text_analyzer::ratio_type>;
	using words_container = std::unordered_map<std::string, word_params>;
	
	struct page_indexing_info : context {
		size_t page_id;
		size_t site_id;
		std::vector<std::tuple<string_enc, string_enc, html_text_analyzer::ratio_type>> words;

		SE_CONTEXT(
			page_indexing_info,
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
		resp_status.message = { msg.body->to_string() + " was successfully added in a queue for indexing!\n", encoding_t::ANSI };

		MAKE_RESPONSE(page_indexing, (
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

			MAKE_REQUEST_WITH_RESPONSE(record_word_resp, record_word_info, (
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

			MAKE_REQUEST(record_to_index_id, record_word_to_index, (
				words_index_params
			))

			CHECK_FOR_STOP(source_for_record, obj)
		}
	}

	void parse_excerpt(se_encoding encoding,
					   const word_params& excerpt, 
					   words_container& cont) {
		std::vector<std::string> words;

		page_text_parser parser(std::get<1>(excerpt), encoding);
		parser.parse(std::get<0>(excerpt), words);

		en_de_coder coder(encoding);
		stemmer stmr(std::get<1>(excerpt), encoding);
		for (auto& w : words) {
			w = stmr.get_stem(w);
			coder.decode(w);
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

			MAKE_REQUEST(page_site_id_resp, page_and_site_id, (
				string_enc{ page_site_urls.first,   encoding_t::ANSI },
				string_enc{ page_site_urls.second,  encoding_t::ANSI }
			))

			MAKE_REQUEST(page_info_id_resp, page_info, (
				string_enc{ page_site_urls.first, encoding_t::ANSI }
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

			en_de_coder coder(page_info_resp.page_encoding);
			for (auto& w : container) {
				coder.encode(std::get<0>(w.second));
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
	void clear() override {
		se_service<pi_page_indexing_service>::clear();
		source_for_collect.clear();
		source_for_record.clear();
	}

public:
	pi_page_indexing_service(size_t id, const fs::path& root, const std::shared_ptr<se_router>& in_router) :
		se_service(id, root / R"(services)" / get_full_name(get_component_name()), in_router)
	{ }

	std::string get_component_name() const override {
		return std::string("page_indexing_service");
	}

	void setup(const configuration& config) override {
		SE_LOG("Successful setup!\n");
	}
};

template<>
class builder<pi_page_indexing_service>: public abstract_service_builder<pi_page_indexing_service> {
protected:
	void add_subscriptions(const service_ptr& service) const override {
		service->router->subscribe<page_indexing_request>(service);
		service->router->subscribe<page_and_site_id_response>(service);
		service->router->subscribe<page_info_response>(service);
		service->router->subscribe<record_word_info_response>(service);
		service->router->subscribe<record_word_to_index_response>(service);
	}

	void add_power_distribution(const service_ptr& service) const override {
		service->power_distribution.push_back({ &pi_page_indexing_service::collecting_info_handler,  10 });
		service->power_distribution.push_back({ &pi_page_indexing_service::recording_info_handler,   10 });
		service->power_distribution.push_back({ &pi_page_indexing_service::requests_handler,         1 });

	}

	void add_request_responders(const service_ptr& service) const override {
		service->responders.insert(typeid(page_indexing_request).name(),
								   &pi_page_indexing_service::page_indexing_request_responder);
	}

	void add_unused_response_type_names(const service_ptr& service) const override {
		service->unused_response_type_names.insert(typeid(record_word_to_index_request).name(), nullptr);
	}
};