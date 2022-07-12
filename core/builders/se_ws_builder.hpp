#pragma once

#include <memory>
#include "se_builder.hpp"
#include "../messages/message_core.hpp"
#include "../se_logable_component.hpp"
#include "../se_logger/se_loggers_storage.hpp"

template<class ws_t>
class se_ws_builder : public se_builder<ws_t> {
protected:
	void configure_logger(const std::shared_ptr<se_logable_component>& comp) const {
		auto logger = se_loggers_storage::get_instance()->get_logger(comp->get_id());
		auto name = comp->get_component_name();

		logger->add_file(
			se_logger::get_code(name),
			name + std::string("_process"),
			comp->get_path()
		);

		logger->add_file(
			se_logger::get_code(name, std::to_string(static_cast<size_t>(message_type::REQUEST))),
			name + std::string("_requests"),
			comp->get_path()
		);

		logger->add_file(
			se_logger::get_code(name, std::to_string(static_cast<size_t>(message_type::RESPONSE))),
			name + std::string("_responses"),
			comp->get_path()
		);
	}

protected:
	virtual void configure_network(const object_ptr& ws) = 0;
	virtual void configure_switches_network(const object_ptr& ws) = 0;
	virtual void add_request_responders(const object_ptr& ws) = 0;
	virtual void add_unused_response_type_names(const object_ptr& ws) = 0;

public:
	virtual void build(object_ptr object) override = 0;
};