#pragma once

#include "services/pi_page_dumper_service.h"
#include "services/pi_page_analyzer_service.h"
#include "services/pi_db_responder_service.h"
#include "services/pi_page_indexing_service.h"
#include "pi_config.hpp"
#include <core/se_domain.hpp>
#include <core/builders/se_domain_builder.hpp>
#include <core/builders/se_builder_imp.hpp>
#include <core/se_service.hpp>
#include <core/se_config/validators/se_cfg_validator.hpp>
#include <core/se_config/validators/se_cfg_validator_imp.hpp>
#include "pi_external_messages.hpp"

class pi_domain : public se_domain<pi_domain, pi_config> {
	SE_DOMAIN(pi_domain, pi_config, page_indexing_domain)

private:
	void start_indexing_request_responder(msg_request msg) {

	}
};

template<>
class se_builder_imp<pi_domain> : public se_domain_builder<pi_domain> {
private:
	template<typename service_t>
	void add_service(const object_ptr& domain) const {
		auto service_ptr = std::make_shared<service_t>(
			domain->id,
			domain->logger_path,
			domain->internal_switch
		);

		domain->services.push_back({ service_ptr, service_ptr->get_power_value() });
	}

protected:
	void configure_network(const object_ptr& domain) const override {
		domain->subscribe(domain->external_switch, typeid(start_indexing_request).name());
	}

	void configure_switches_network(const object_ptr& domain) const override {
		domain->external_switch->subscribe(domain, typeid(start_indexing_response_success).name());
		domain->external_switch->subscribe(domain, typeid(start_indexing_response_fail).name());
	}

	void add_request_responders(const object_ptr& domain) const override {
		domain->responders.insert(typeid(start_indexing_request).name(),
								  &pi_domain::start_indexing_request_responder);
	}

	void add_unused_response_type_names(const object_ptr& domain) const override { }

	void add_domains(const object_ptr& domain) const override { }

	void add_services(const object_ptr& domain) const override {
		add_service<pi_page_analyzer_service>(domain);
		add_service<pi_db_responder_service> (domain);
		add_service<pi_page_dumper_service>  (domain);
		add_service<pi_page_indexing_service>(domain);
	}

	void set_power_levels(const object_ptr& domain) override {
		auto min_value = domain->get_services_power_value();

		for (auto i = domain->default_power_level; i < 3; ++i) {
			domain->levels_power.insert({ i, (i + 1) * min_value });
		}
	}
};

template<>
class se_cfg_validator_imp<pi_domain, pi_config> : public se_cfg_validator<pi_domain, pi_config> {
private:
	void valid_db_connection(domain_ptr domain, config_ptr config) const {
		std::shared_ptr<sql::Connection> con = MYSQL_CONNECT(config);
		con->close();
	}

	void valid_sources(domain_ptr domain, config_ptr config) const {
		if (config->get_sources().empty()) {
			throw std::exception("Error! Empty sources.");
		}
	}

	void valid_indexing_mode(domain_ptr domain, config_ptr config) const {
		if (config->get_indexing_mode() <= pi_page_dumper_service::indexing_modes::LOWER_BOUND ||
			config->get_indexing_mode() >= pi_page_dumper_service::indexing_modes::UPPER_BOUND) {
			throw std::exception("Error! Invalid indexing mode.");
		}
	}

	void valid_power_value(domain_ptr domain, config_ptr config) const {
		if (domain->levels_power.find(config->get_power_level()) == domain->levels_power.end()) {
			throw std::exception("Error! Invalid power level.");
		}
	}

public:
	bool is_valid(domain_ptr domain, config_ptr config, std::string& error_msg) const {
		bool res = true;

		try {
			valid_db_connection(domain, config);
			valid_sources(domain, config);
			valid_indexing_mode(domain, config);
			valid_power_value(domain, config);
		} catch (const std::exception& ex) {
			res = false;
			error_msg = ex.what();
		}

		return res;
	}
};