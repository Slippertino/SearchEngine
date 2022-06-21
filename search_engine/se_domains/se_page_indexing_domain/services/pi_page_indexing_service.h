#pragma once

#include <tools/page_text_parser.hpp>
#include <text_property_types/se_encoding.hpp>
#include <text_property_types/se_language.hpp>
#include <thread_safe_containers/thread_safe_queue.hpp>
#include <thread_safe_containers/thread_safe_unordered_map.hpp>
#include "../../se_service.hpp"
#include "../messages.h"

class pi_page_indexing_service : public se_service<pi_page_indexing_service>
{
	SE_SERVICE(pi_page_indexing_service)

private:
	using word_params     = std::tuple<std::string, se_language, html_text_analyzer::ratio_type>;
	using words_container = std::unordered_map<std::string, word_params>;

	struct page_indexing_info
	{
		size_t page_id;
		size_t site_id;
		string_enc stemmed_word;
		string_enc lang;
		html_text_analyzer::ratio_type rank;

		std::string to_string() const {
			nlohmann::json js; 
			nlohmann::to_json(js, *this); 
			return js.dump(); 
		}

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(
			page_indexing_info, 
			page_id, 
			site_id, 
			stemmed_word, 
			lang,
			rank
		)
	};

private:
	thread_safe_queue<std::pair<std::string, std::string>> source_for_collect;
	thread_safe_queue<page_indexing_info> source_for_record;

	std::mutex mutex;
	size_t counter = 0;
	double average = 0;

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

	void parse_excerpt(se_encoding encoding,
					   const word_params& excerpt, 
					   words_container& cont) {
		std::vector<std::string> words;

		page_text_parser parser(std::get<1>(excerpt), encoding);
		parser.parse(std::get<0>(excerpt), words);

		for (auto& w : words) {
			if (cont.find(w) == cont.end())
				cont.insert({ w, { w, std::get<1>(excerpt), std::get<2>(excerpt) } });
			else
				std::get<2>(cont[w]) += std::get<2>(excerpt);
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

			words_container container;
			for (auto& excerpt : page_info_resp.text_excerpts) {
				parse_excerpt(page_info_resp.page_encoding, excerpt, container);
			}
			
			en_de_coder coder(page_info_resp.page_encoding);

			for (auto& w : container) {
				coder.encode(std::get<0>(w.second));
				source_for_record.add(page_indexing_info {
					page_site_resp.page_id,
					page_site_resp.site_id,
					{ std::get<0>(w.second),						  page_info_resp.page_encoding },
					{ se_language(std::get<1>(w.second)).to_string(), encoding_t::UTF_8			   },
					std::get<2>(w.second)
				});
			}

			auto end = std::chrono::steady_clock::now();

			std::lock_guard<std::mutex> locker(mutex);
			auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
			average = (average * counter + time.count()) / (counter + 1);
			counter++;
			std::cout << "End of analyze! Current time: " << time.count() << " ms ; Total: " << counter << " ; Avg time: " << average << std::endl;
		}
	}

protected:
	void clear() override {
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
		service->power_distribution.push_back({ &pi_page_indexing_service::collecting_info_handler,  35 });
		service->power_distribution.push_back({ &pi_page_indexing_service::recording_info_handler,   60 });
		service->power_distribution.push_back({ &pi_page_indexing_service::requests_handler,         1 });

	}

	void add_request_responders(const service_ptr& service) const override {
		service->responders.insert(typeid(page_indexing_request).name(),
								   &pi_page_indexing_service::page_indexing_request_responder);
	}

	void add_unused_response_type_names(const service_ptr& service) const override {
		service->unused_response_type_names.insert(typeid(record_word_to_index_response).name(), nullptr);
	}
};