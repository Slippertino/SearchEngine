#pragma once

#include "../se_domain.h"
#include "s_config.hpp"
#include "services/s_search_service.hpp"
#include "services/s_db_responder_service.hpp"

class s_domain : public se_domain<s_domain, s_config, 2> {
	SE_DOMAIN(s_domain)

public:
	s_domain() = delete;
	s_domain(size_t id, const fs::path& root) :
		se_domain(id, root / fs::path(get_full_name(get_component_name())))
	{ }

	std::string get_component_name() const override {
		return std::string("search_domain");
	}
};

template<>
class builder<s_domain> : public abstract_domain_builder<s_domain> {
private:
	template<typename service_t>
	void add_service(const domain_ptr& domain, size_t id) const {
		auto service = builder<service_t>().build(
			domain->id,
			domain->logger_path,
			domain->router
		);

		domain->services[id] = std::make_pair(service, service->get_power_value());
	}

protected:
	void add_services(const domain_ptr& domain) const override {
		domain->router = std::make_shared<se_router>();

		add_service<s_search_service>(domain, 0);
		add_service<s_db_responder_service>(domain, 1);
	}
};