#pragma once

#include <thread_safe_containers/thread_safe_queue.hpp>
#include <thread_safe_containers/thread_safe_unordered_map.hpp>
#include "../../se_service.hpp"
#include "../messages.h"

class pi_page_indexing_service : public se_service<pi_page_indexing_service>
{
	SE_SERVICE(pi_page_indexing_service)

private:
	thread_safe_queue<std::pair<std::string, std::string>> source;

private:
	void page_indexing_request_responder(msg_request msg) {
		auto req = static_cast<page_indexing_request*>(msg.body.get());
		source.add({req->url, req->prefix});

		response_status resp_status;
		resp_status.status  = runtime_status::SUCCESS;
		resp_status.message = msg.body->to_string() + " was successfully added in a queue for indexing!\n";

		MAKE_RESPONSE(page_indexing, (
			msg.id,
			resp_status
		))
	}

	void page_indexing_handler() {
		while (!stop_flag) {
			std::pair<std::string, std::string> page_site_urls;

			pool.set_inactive(std::this_thread::get_id());
			extract_from_ts_queue(source, page_site_urls);
			SE_LOG("Extracted: " << page_site_urls.first << " ; " << page_site_urls.second << "\n");
			pool.set_active(std::this_thread::get_id());
		}
	}

protected:
	void clear() override {
		source.clear();
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
	}

	void add_power_distribution(const service_ptr& service) const override {
		service->power_distribution.push_back({ &pi_page_indexing_service::page_indexing_handler, 1 });
		service->power_distribution.push_back({ &pi_page_indexing_service::requests_handler,      1 });
	}

	void add_request_responders(const service_ptr& service) const override {
		service->responders.insert(typeid(page_indexing_request).name(),
								   &pi_page_indexing_service::page_indexing_request_responder);
	}

	void add_unused_response_type_names(const service_ptr& service) const override {

	}
};