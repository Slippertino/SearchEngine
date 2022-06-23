#pragma once

#include <tools/en_de_coder.hpp>
#include <thread_safe_containers/thread_safe_queue.hpp>
#include <thread_safe_containers/thread_safe_unordered_map.hpp>
#include "../../se_service.hpp"
#include "../s_messages.hpp"
#include "../../se_services_infrastructure/se_services_communication.hpp"
#include "../s_queries.hpp"
#include "../../se_db_responder.h"

class s_db_responder_service : public se_service<s_db_responder_service>,
							   public se_db_responder<s_queries> {
	SE_SERVICE(s_db_responder_service)

private:

public:
	s_db_responder_service() = delete;
	s_db_responder_service(size_t id, const fs::path& root, const std::shared_ptr<se_router>& in_router) :
		se_service(id, root / R"(services)" / get_full_name(get_component_name()), in_router)
	{ }

	std::string get_component_name() const override {
		return std::string("search_responder_service");
	}
};

template<>
class builder<s_db_responder_service> : public abstract_service_builder<s_db_responder_service> {
protected:
	void add_subscriptions(const service_ptr& service) const override {

	}

	void add_power_distribution(const service_ptr& service) const override {
		service->power_distribution.push_back({ &s_db_responder_service::requests_handler, 1 });
	}

	void add_request_responders(const service_ptr& service) const override {

	}

	void add_unused_response_type_names(const service_ptr& service) const override {
		return;
	}
};