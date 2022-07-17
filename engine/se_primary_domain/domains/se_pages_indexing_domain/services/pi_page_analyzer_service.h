#pragma once

#include <cpr/cpr.h>
#include <search_engine_analyzers/tools/html_text_analyzer.hpp>
#include <thread_extensions/thread_safe_containers/thread_safe_queue.hpp>
#include <thread_extensions/thread_safe_containers/thread_safe_unordered_map.hpp>
#include <core/se_service.hpp>
#include <core/builders/se_service_builder.hpp>
#include "../messages/pi_internal_messages.hpp"
#include "../pi_config.hpp"

class pi_page_analyzer_service final : public se_service<pi_page_analyzer_service, pi_config> {
	SE_SERVICE(pi_page_analyzer_service, page_analyzer_service)

private:
	static const size_t max_rejected_requests;

	thread_safe_unordered_map<decltype(html_text_analyzer::page_info::url), 
							  std::pair<html_text_analyzer::page_info, response_status>> info_storage;

	thread_safe_queue<url_to_analyze_request*> source;

private:
	void url_to_analyze_request_responder(msg_request msg) {
		auto req = static_cast<url_to_analyze_request*>(msg.body.get());
		source.add(req);

		std::pair<html_text_analyzer::page_info, response_status> info;
		get_from_ts_unordered_map(info_storage, req->url.str, info);
		if (stop_flag)
			return;

		MAKE_RESPONSE(url_to_analyze_response, (
			msg.id, 
			info.first.page_encoding, 
			string_enc{ info.first.content, info.first.page_encoding },
			info.first.status_code,
			is_valid_page(info.first) && is_successful_analysis(info.second),
			info.first.linked_urls,
			info.second
		))
	}

	void page_info_request_responder(msg_request msg) {
		auto req = static_cast<page_info_request*>(msg.body.get());

		std::pair<html_text_analyzer::page_info, response_status> info;
		extract_from_ts_unordered_map(info_storage, req->url.str, info);
		if (stop_flag)
			return;

		MAKE_RESPONSE(page_info_response, (
			msg.id,
			info.first.page_encoding,
			info.first.text_excerpts,
			info.second
		))
	}

	bool handle_status_code(long status_code) const {
		if (status_code == cpr::status::HTTP_TOO_MANY_REQUESTS || !status_code) {
			std::this_thread::sleep_for(1s);
			return false;
		}

		return true;
	}

	bool is_successful_analysis(response_status status) {
		return status.status == runtime_status::SUCCESS;
	}

	bool is_valid_page(const html_text_analyzer::page_info& info) const {
		return cpr::status::is_success(info.status_code) && 
			   info.page_encoding.get_type() != encoding_t::UNKNOWN;
	}

	void page_analyze_handler()
	{
		while (!stop_flag)
		{
			url_to_analyze_request* req = nullptr;
			html_text_analyzer::page_info res;
			response_status status;

			pool.set_inactive(std::this_thread::get_id());
			extract_from_ts_queue(source, req);
			if (stop_flag && !req || info_storage.find(req->url.str) != info_storage.end())
				continue;
			pool.set_active(std::this_thread::get_id());

			try {
				res.url = req->url.str;
				res.prefix = req->prefix.str;

				cpr::Response resp_info;

				size_t count(0);
				do {
					if (count) {
						SE_LOG("Retrying to get info about : " << res.url 
							<< " ! Previous code: " << resp_info.status_code 
							<< "; Attempt: " << count << "\n");
					}

					resp_info = cpr::Get(cpr::Url(res.url));
					++count;
				} while (!handle_status_code(resp_info.status_code) && count < max_rejected_requests);

				if (count >= max_rejected_requests)
					throw std::exception("Too many attempts to get information!\n");

				res.status_code = resp_info.status_code;

				html_text_analyzer analyzer(string_enc{ resp_info.text, encoding_t::UTF_8 });
				analyzer.run_parse(res);

				status.status = runtime_status::SUCCESS;
				status.message = { "Successful analysis of the page was carried out", DEFAULT_ENCODING };
			}
			catch (const std::exception& ex) {
				status.status = runtime_status::FAIL;
				status.message = { ex.what(), DEFAULT_ENCODING };
			}

			SE_LOG("Url : " << res.url <<
				   "; Code : " << res.status_code <<
				   "; Result : " << status.to_string() << "\n"
			);

			info_storage.insert(res.url, { res, status });
		}
	}

protected:
	void setup_base(pi_config* config) override final {}

	void clear() override final {
		se_service<pi_page_analyzer_service, pi_config>::clear();
		info_storage.clear();
		source      .clear();
	}
};

const size_t pi_page_analyzer_service::max_rejected_requests = 50;

template<>
class se_builder_imp<pi_page_analyzer_service> final : public se_service_builder<pi_page_analyzer_service> {
protected:
	void configure_own_network(const object_ptr& service) const override final {
		service
			->subscribe(service->internal_switch, typeid(url_to_analyze_response).name())
			->subscribe(service->internal_switch, typeid(page_info_response).name());
	}

	void configure_switches_network(const object_ptr& service) const override final {
		service->internal_switch
			->subscribe(service, typeid(url_to_analyze_request).name())
			->subscribe(service, typeid(page_info_request).name());
	}

	void add_request_responders(const object_ptr& service) const override final {
		service->responders = {
			{ typeid(url_to_analyze_request).name(), &pi_page_analyzer_service::url_to_analyze_request_responder },
			{ typeid(page_info_request).name(),      &pi_page_analyzer_service::page_info_request_responder      },
		};
	}

	void add_unused_response_type_names(const object_ptr& service) const override final {
		return;
	}

	void add_power_distribution(const object_ptr& service) const override final {
		service->power_distribution = {
			{ &pi_page_analyzer_service::page_analyze_handler, 1 },
			{ &pi_page_analyzer_service::requests_handler,     1 },
		};
	}
};