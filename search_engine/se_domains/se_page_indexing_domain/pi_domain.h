#pragma once

#include "../se_domain.h"
#include "services/pi_page_dumper_service.h"
#include "services/pi_page_analyzer_service.h"
#include "services/pi_db_responder_service.h"

class pi_domain : public se_domain<pi_domain, 3>
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
protected:
	void add_services(const domain_ptr& domain) const override {
		domain->router = std::make_shared<se_router>();

		domain->services = {
			std::make_pair(builder<pi_page_analyzer_service>() .build(domain->id, domain->logger_path, domain->router),  2),
			std::make_pair(builder<pi_db_responder_service>()  .build(domain->id, domain->logger_path, domain->router),  2),
			std::make_pair(builder<pi_page_dumper_service>()   .build(domain->id, domain->logger_path, domain->router),  1),
		};
	}
};