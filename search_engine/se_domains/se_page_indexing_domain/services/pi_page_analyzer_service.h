#pragma once

#include <cpr/cpr.h>
#include <html_text_analyzer.h>
#include <en_de_coder.h>
#include <thread_safe_containers/thread_safe_queue.hpp>
#include <thread_safe_containers/thread_safe_unordered_map.hpp>
#include "../../se_service.hpp"
#include "../messages.h"
#include "../../se_services_infrastructure/se_services_communication.hpp"

class pi_page_analyzer_service : public se_service<pi_page_analyzer_service>
{
	SE_SERVICE(pi_page_analyzer_service)

private:
	thread_safe_unordered_map<decltype(html_text_analyzer::page_info::url), 
							  std::pair<html_text_analyzer::page_info, response_status>> info_storage;

	thread_safe_queue<url_to_analyze_request*> source;

	std::mutex mutex;

private:
	void url_to_analyze_request_responder(msg_request msg) {
		auto req = static_cast<url_to_analyze_request*>(msg.body.get());
		source.add(req);

		std::pair<html_text_analyzer::page_info, response_status> info;
		get_from_ts_unordered_map(info_storage, req->url, info);
		if (stop_flag)
			return;

		MAKE_RESPONSE(url_to_analyze, (
			msg.id, 
			info.first.page_encoding, 
			info.first.content,
			info.first.status_code,
			is_valid_page(info.first.status_code),
			info.first.linked_urls,
			info.second
		))
	}

	bool handle_status_code(long status_code) const {
		if (status_code == cpr::status::HTTP_TOO_MANY_REQUESTS || !status_code) {
			std::this_thread::sleep_for(100ms);
			return false;
		}

		return true;
	}

	bool is_valid_page(long code) const {
		return cpr::status::is_success(code);
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
			if (stop_flag && !req)
				continue;
			pool.set_active(std::this_thread::get_id());

			try {
				res.url = req->url;
				res.prefix = req->prefix;

				cpr::Response resp_info;

				size_t count(0);
				do {
					if (count) {
						SE_LOG("Retrying to get info about : " << res.url << " ! Attempt: " << count << "\n");
					}

					resp_info = cpr::Get(cpr::Url(res.url));
					++count;
				} while (!handle_status_code(resp_info.status_code) && count < 10000);

				res.status_code = resp_info.status_code;

				html_text_analyzer analyzer(resp_info.text);
				analyzer.run_parse(res);

				status.status = runtime_status::SUCCESS;
				status.message = "Successful analysis of the page was carried out";
			}
			catch (const std::exception& ex) {
				status.status = runtime_status::FAIL;
				status.message = ex.what();
			}

			SE_LOG("Url : " << res.url <<
				   "; Code : " << res.status_code <<
				   "; Result : " << status.to_string() << "\n"
			);

			info_storage.insert(res.url, { res, status });
		}
	}

protected:
	std::string get_component_name() const override {
		return std::string("page_analyzer_service");
	}

	void clear() override {
		info_storage.clear();
		source      .clear();
	}

public:
	pi_page_analyzer_service(size_t id, const fs::path& root, const std::shared_ptr<se_router>& in_router) : 
		se_service(id, root / R"(services)" / get_full_name(get_component_name()), in_router)
	{ }

	void setup(const configuration& config) override {
		SE_LOG("Successful setup!\n");
	}
};

template<>
class builder<pi_page_analyzer_service> : public abstract_service_builder<pi_page_analyzer_service>
{
protected:
	void add_subscriptions(const service_ptr& service) const override {
		service->router->subscribe<url_to_analyze_request>(service);
	}

	void add_power_distribution(const service_ptr& service) const override {
		service->power_distribution.push_back({ &pi_page_analyzer_service::page_analyze_handler, 1 });
		service->power_distribution.push_back({ &pi_page_analyzer_service::requests_handler,     1 });
	}

	void add_request_responders(const service_ptr& service) const override {
		service->responders.insert(typeid(url_to_analyze_request).name(), 
								   &pi_page_analyzer_service::url_to_analyze_request_responder);
	}

	void configure_logger(const service_ptr& service) const override {
		auto logger = se_loggers_storage::get_instance()->get_logger(service->id);
		auto name   = service->get_component_name();

		logger->add_file(
			se_logger::get_code(name),
			name + std::string("_process"),
			service->logger_path
		);

		logger->add_file(
			se_logger::get_code(name, std::to_string(static_cast<size_t>(message_type::REQUEST))),
			name + std::string("_requests"),
			service->logger_path
		);

		logger->add_file(
			se_logger::get_code(name, std::to_string(static_cast<size_t>(message_type::RESPONSE))),
			name + std::string("_responses"),
			service->logger_path
		);
	}
};