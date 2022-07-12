#pragma once

#include <memory>
#include "se_ws_builder.hpp"

template<class service_t>
class se_service_builder : public se_ws_builder<service_t> {
protected:
	virtual void configure_network(const object_ptr& service) override = 0;
	virtual void configure_switches_network(const object_ptr& service) override = 0;
	virtual void add_request_responders(const object_ptr& service) override = 0;
	virtual void add_unused_response_type_names(const object_ptr& service) override = 0;

	virtual void add_power_distribution(const object_ptr& service) = 0;

public:
	void build(object_ptr object) {
		configure_network(object);
		configure_switches_network(object);
		configure_logger(object);
		add_request_responders(object);
		add_unused_response_type_names(object);
		add_power_distribution(object);
	}
};