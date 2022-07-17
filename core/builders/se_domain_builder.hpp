#pragma once

#include <memory>
#include "se_ws_builder.hpp"

template<class domain_t>
class se_domain_builder : public se_ws_builder<domain_t> {
protected:
	virtual void configure_own_network(const object_ptr& domain) const override = 0;
	virtual void configure_switches_network(const object_ptr& domain) const override = 0;
	virtual void add_request_responders(const object_ptr& domain) const override = 0;
	virtual void add_unused_response_type_names(const object_ptr& domain) const override = 0;
	virtual void set_power_levels(const object_ptr& domain) const = 0;
	virtual void add_domains(const object_ptr& domain) const = 0;
	virtual void add_services(const object_ptr& domain) const = 0;

public:
	void build(object_ptr object) const override {
		configure_own_network(object);
		configure_switches_network(object);
		configure_logger(object);
		add_request_responders(object);
		add_unused_response_type_names(object);
		add_domains(object);
		add_services(object);
		set_power_levels(object);
	}
};