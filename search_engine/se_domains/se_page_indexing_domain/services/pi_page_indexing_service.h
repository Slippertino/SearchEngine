#pragma once

#include <thread_safe_containers/thread_safe_queue.hpp>
#include <thread_safe_containers/thread_safe_unordered_map.hpp>
#include "../../se_service.hpp"
#include "../messages.h"

class pi_page_indexing_service : public se_service<pi_page_indexing_service>
{
	SE_SERVICE(pi_page_indexing_service)

private:

private:
	void page_indexing_handler() {

	}

protected:


public:
	std::string get_component_name() const override {
		return std::string("page_indexing_service");
	}

};

template<>
class builder<pi_page_indexing_service>: public abstract_service_builder<pi_page_indexing_service>
{
protected:
	void add_subscriptions(const service_ptr& service) const override {

	}

	void add_power_distribution(const service_ptr& service) const override {
		service->power_distribution.push_back({ &pi_page_indexing_service::page_indexing_handler, 1 });
		service->power_distribution.push_back({ &pi_page_indexing_service::requests_handler,      1 });
	}

	void add_request_responders(const service_ptr& service) const override {

	}

	void add_unused_response_type_names(const service_ptr& service) const override {

	}
};