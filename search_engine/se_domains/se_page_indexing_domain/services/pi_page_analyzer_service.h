#pragma once

#include <cpr/cpr.h>
#include <tools/html_text_analyzer.hpp>
#include <thread_safe_containers/thread_safe_queue.hpp>
#include <thread_safe_containers/thread_safe_unordered_map.hpp>
#include "../../se_service.hpp"
#include "../pi_messages.h"
#include "../../se_services_infrastructure/se_services_communication.hpp"
#include "../pi_config.hpp"

class pi_page_analyzer_service : public se_service<pi_page_analyzer_service, pi_config> {
	SE_SERVICE(pi_page_analyzer_service)

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

		MAKE_RESPONSE(url_to_analyze, (
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

		MAKE_RESPONSE(page_info, (
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
	void setup_base(pi_config* config) override {}

	void clear() override {
		se_service<pi_page_analyzer_service, pi_config>::clear();
		info_storage.clear();
		source      .clear();
	}

public:
	pi_page_analyzer_service(size_t id, const fs::path& root, const std::shared_ptr<se_router>& in_router) : 
		se_service(id, root / R"(services)" / get_full_name(get_component_name()), in_router)
	{ }

	std::string get_component_name() const override {
		return std::string("page_analyzer_service");
	}
};

const size_t pi_page_analyzer_service::max_rejected_requests = 50;

template<>
class builder<pi_page_analyzer_service> : public abstract_service_builder<pi_page_analyzer_service> {
protected:
	void add_subscriptions(const service_ptr& service) const override {
		service->router->subscribe<url_to_analyze_request>(service);
		service->router->subscribe<page_info_request>(service);
	}

	void add_power_distribution(const service_ptr& service) const override {
		service->power_distribution.push_back({ &pi_page_analyzer_service::page_analyze_handler, 1 });
		service->power_distribution.push_back({ &pi_page_analyzer_service::requests_handler,     1 });
	}

	void add_request_responders(const service_ptr& service) const override {
		service->responders.insert(typeid(url_to_analyze_request).name(), 
								   &pi_page_analyzer_service::url_to_analyze_request_responder);
		service->responders.insert(typeid(page_info_request).name(),
								   &pi_page_analyzer_service::page_info_request_responder);
	}

	void add_unused_response_type_names(const service_ptr& service) const override {
		return;
	}
};