#pragma once

#include "../se_domain.h"
#include "services/pi_page_dumper_service.h"
#include "services/pi_page_analyzer_service.h"
#include "services/pi_db_responder_service.h"
#include "services/pi_page_indexing_service.h"

class pi_domain : public se_domain<pi_domain, 4>
{ 
	SE_DOMAIN(pi_domain) 

public:
	pi_domain() = delete;
	pi_domain(size_t id, const fs::path& root) : 
		se_domain(id, root / fs::path(get_full_name(get_component_name())))
	{ }

	std::string get_component_name() const override {
		return std::string("page_index_domain");
	}
};

template<>
class builder<pi_domain> : public abstract_domain_builder<pi_domain>
{
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
		
		add_service<pi_page_analyzer_service>(domain, 0);
		add_service<pi_db_responder_service> (domain, 1);
		add_service<pi_page_dumper_service>  (domain, 2);
		add_service<pi_page_indexing_service>(domain, 3);
	}
};