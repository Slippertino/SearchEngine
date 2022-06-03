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

	void crop_content(std::string& text) const {
		const auto tags = std::map<std::string, std::string>{ {"<script",   "</script>"  },
															  {"<style",    "</style>"   },
															  {"<button",   "</button>"  },
															  {"<footer",   "</footer>"  },
															  {"<noscript", "</noscript>"},
															  {"<!--",      "-->"        } };
		size_t off = 0;
		while (true) {
			std::pair<std::string, std::string> tag;
			auto bpos = std::string::npos;

			for (auto& p : tags) {
				auto cur = text.find(p.first, off);

				if (cur < bpos) {
					bpos = cur;
					tag = p;
				}
			}

			if (bpos != std::string::npos) {
				auto epos = text.find(tag.second, bpos);
				off = bpos;
				text.erase(bpos, epos + tag.second.size() - bpos);
			}
			else
				break;
		}
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

				do {
					resp_info = cpr::Get(cpr::Url(res.url));
				} while (!handle_status_code(resp_info.status_code));

				crop_content(resp_info.text);

				res.content = resp_info.text;
				res.status_code = resp_info.status_code;

				html_text_analyzer analyzer(res.content);
				analyzer.parse(res);

				status.status = runtime_status::SUCCESS;
				status.message = "Successful analysis of the page was carried out";
			}
			catch (const std::exception& ex) {
				status.status = runtime_status::FAIL;
				status.message = ex.what();
			}

			info_storage.insert(res.url, { res, status });
		}
	}

protected:
	void clear() override {
		info_storage.clear();
		source      .clear();
	}

public:
	pi_page_analyzer_service(const std::shared_ptr<se_router>& in_router) : se_service(in_router)
	{ }

	void setup(const configuration& config) override 
	{ }
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
};