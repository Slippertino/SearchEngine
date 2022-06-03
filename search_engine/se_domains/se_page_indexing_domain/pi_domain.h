#pragma once

#include "../se_domain.h"
#include "services/pi_page_dumper_service.h"
#include "services/pi_page_analyzer_service.h"
#include "services/pi_db_responder_service.h"

class pi_domain : public se_domain<pi_domain, 3>
{ SE_DOMAIN(pi_domain) };

template<>
class builder<pi_domain> : public abstract_domain_builder<pi_domain>
{
protected:
	void add_services(const domain_ptr& service) const override {
		service->router = std::make_shared<se_router>();

		service->services = { 
			std::make_pair(builder<pi_page_analyzer_service>() .build(service->router),  2),
			std::make_pair(builder<pi_db_responder_service>()  .build(service->router),  2),
			std::make_pair(builder<pi_page_dumper_service>()   .build(service->router),  1),
		};
	}
};