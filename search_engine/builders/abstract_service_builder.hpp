#pragma once

#include <memory>
#include "se_component.h"

template<typename service_t>
class abstract_service_builder
{
protected:
	using service_ptr = std::shared_ptr<service_t>;

protected:
	virtual void add_subscriptions(const service_ptr& service) const = 0;
	virtual void add_power_distribution(const service_ptr& service) const = 0;
	virtual void add_request_responders(const service_ptr& service) const = 0;
	virtual void add_unused_response_type_names(const service_ptr& service) const = 0;

protected:
	void configure_logger(const std::shared_ptr<se_component>& service) const {
		auto logger = se_loggers_storage::get_instance()->get_logger(service->get_id());
		auto name = service->get_component_name();

		logger->add_file(
			se_logger::get_code(name),
			name + std::string("_process"),
			service->get_path()
		);

		logger->add_file(
			se_logger::get_code(name, std::to_string(static_cast<size_t>(message_type::REQUEST))),
			name + std::string("_requests"),
			service->get_path()
		);

		logger->add_file(
			se_logger::get_code(name, std::to_string(static_cast<size_t>(message_type::RESPONSE))),
			name + std::string("_responses"),
			service->get_path()
		);
	}

public:
	template<typename... Args>
	service_ptr build(Args&&... args) const {
		service_ptr res = std::make_shared<service_t>(std::forward<Args>(args)...);

		add_subscriptions(res);
		add_power_distribution(res);
		add_request_responders(res);
		configure_logger(res);

		return res;
	}
};