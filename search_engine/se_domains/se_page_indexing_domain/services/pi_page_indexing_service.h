#pragma once

#include <thread_safe_containers/thread_safe_queue.hpp>
#include <thread_safe_containers/thread_safe_unordered_map.hpp>
#include "../../se_service.hpp"
#include "../messages.h"
#include <page_text_parser.h>

class pi_page_indexing_service : public se_service<pi_page_indexing_service>
{
	SE_SERVICE(pi_page_indexing_service)

private:
	using word_params = std::tuple<std::string, language_t, html_text_analyzer::ratio_type>;
	using words_container = thread_safe_unordered_map<std::string, word_params>;

	struct S  {
		size_t page_id;
		size_t site_id;
		std::string stemmed_word;
		std::string lang;
		html_text_analyzer::ratio_type rank;

		std::string to_string() const {
			nlohmann::json js; 
			nlohmann::to_json(js, *this); 
			return js.dump(); 
		}

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(S, page_id, site_id, stemmed_word, lang,rank)
	};


	thread_safe_queue<std::pair<std::string, std::string>> source_for_analyze;
	thread_safe_queue<S> source_for_record;

private:
	void page_indexing_request_responder(msg_request msg) {
		auto req = static_cast<page_indexing_request*>(msg.body.get());
		source_for_analyze.add({req->url, req->prefix});

		response_status resp_status;
		resp_status.status  = runtime_status::SUCCESS;
		resp_status.message = msg.body->to_string() + " was successfully added in a queue for indexing!\n";

		MAKE_RESPONSE(page_indexing, (
			msg.id,
			resp_status
		))
	}
	
	void f(encoding_t encoding, 
		   word_params& params,
		   words_container& container) {
		page_text_parser parser(std::get<1>(params), encoding);

		std::vector<std::string> words;
		parser.parse(std::get<0>(params), words);

		for (auto& w : words) {
			container.update(
				w,
				{ w, std::get<1>(params), std::get<2>(params) },
				[](const auto& v1, const auto& v2) {
					return word_params{ std::get<0>(v1), std::get<1>(v1), std::get<2>(v1) + std::get<2>(v2) };
				}
			);
		}
	}

	void record_handler() {
		while (!stop_flag) {
			S obj;
			pool.set_inactive(std::this_thread::get_id());
			extract_from_ts_queue(source_for_record, obj);
			SE_LOG("Extracted word params for record: " << obj.to_string() << "\n");
			pool.set_active(std::this_thread::get_id());

			MAKE_REQUEST_WITH_RESPONSE(record_word_resp, record_word_info, (
				record_word_resp,
				obj.site_id,
				obj.lang,
				obj.stemmed_word
			))

			MAKE_REQUEST(record_to_index_id, record_word_to_index, (
				obj.page_id,
				record_word_resp.word_id,
				obj.rank
			))
		}
	}

	void page_indexing_handler() {
		while (!stop_flag) {
			std::pair<std::string, std::string> page_site_urls;

			pool.set_inactive(std::this_thread::get_id());
			extract_from_ts_queue(source_for_analyze, page_site_urls);
			SE_LOG("Extracted data for analysis: " << page_site_urls.first << " ; " << page_site_urls.second << "\n");
			pool.set_active(std::this_thread::get_id());

			MAKE_REQUEST(page_site_id_resp, page_and_site_id, (
				page_site_urls.first,
				page_site_urls.second
			))

			MAKE_REQUEST_WITH_RESPONSE(page_info, page_info, (
				page_info,
				page_site_urls.first
			))

			words_container words;
			std::vector<std::thread> threads(page_info.text_excerpts.size());
			for (auto& excerpt : page_info.text_excerpts) {
				threads.push_back(std::thread(
					&pi_page_indexing_service::f,
					this,
					page_info.page_encoding,
					std::ref(excerpt),
					std::ref(words)
				));
			}

			GET_RESPONSE(page_site_id_resp, page_site_resp, page_and_site_id_response)

			for (auto& th : threads)
				th.join();

			for (auto& w : words) {
				source_for_record.add(S{
					page_site_resp.page_id,
					page_site_resp.site_id,
					std::get<0>(w.second),
					converter_to_language_t().to_string(std::get<1>(w.second)),
					std::get<2>(w.second)
				});
			}
		}
	}

protected:
	void clear() override {
		source_for_analyze.clear();
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
class builder<pi_page_indexing_service>: public abstract_service_builder<pi_page_indexing_service>
{
protected:
	void add_subscriptions(const service_ptr& service) const override {
		service->router->subscribe<page_indexing_request>(service);
		service->router->subscribe<page_and_site_id_response>(service);
		service->router->subscribe<page_info_response>(service);
		service->router->subscribe<record_word_info_response>(service);
		service->router->subscribe<record_word_to_index_response>(service);
	}

	void add_power_distribution(const service_ptr& service) const override {
		service->power_distribution.push_back({ &pi_page_indexing_service::page_indexing_handler, 1 });
		service->power_distribution.push_back({ &pi_page_indexing_service::requests_handler,      1 });
		service->power_distribution.push_back({ &pi_page_indexing_service::record_handler,        1 });
	}

	void add_request_responders(const service_ptr& service) const override {
		service->responders.insert(typeid(page_indexing_request).name(),
								   &pi_page_indexing_service::page_indexing_request_responder);
	}

	void add_unused_response_type_names(const service_ptr& service) const override {
		service->unused_response_type_names.insert(typeid(record_word_to_index_response).name(), nullptr);
	}
};