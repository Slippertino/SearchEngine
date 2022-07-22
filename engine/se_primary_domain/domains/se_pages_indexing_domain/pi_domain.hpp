#pragma once

#include "services/pi_page_dumper_service.h"
#include "services/pi_page_analyzer_service.h"
#include "services/pi_db_responder_service.h"
#include "services/pi_page_indexing_service.h"
#include "pi_config.hpp"
#include <core/se_domain.hpp>
#include <core/builders/se_domain_builder.hpp>
#include <core/se_service.hpp>
#include <core/se_config/validators/se_cfg_validator.hpp>
#include "messages/pi_external_messages.hpp"
#include <engine/se_primary_domain/messages/p_external_messages.hpp>
#include <engine/se_db_entities/se_db_connection.hpp>

class pi_domain final : public se_domain<pi_domain, pi_config> {
	SE_DOMAIN(pi_domain, pi_config, page_indexing_domain)

private:
	void start_indexing_request_responder(msg_request msg) {
		std::string error_msg;
		if (!cfg_validator->is_valid(this, base_config.get(), error_msg)) {
			MAKE_RESPONSE(start_indexing_response_fail, (
				false,
				error_msg
			))
			
			return;
		}
		
		if (is_flag_set(working_status, status_flags::RUN)) {
			MAKE_RESPONSE(start_indexing_response_fail, (
				false,
				"The indexing process has already started!"
			))

			return;
		}

		reset_services();
		setup_services();
		run_services();

		MAKE_RESPONSE(start_indexing_response_success, (
			true
		))
	}

	void clear() override final {
		se_domain<pi_domain, pi_config>::clear();
	}
};

template<>
class se_builder_imp<pi_domain> final : public se_domain_builder<pi_domain> {
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
	void configure_own_network(const object_ptr& domain) const override final {
		domain
			->subscribe(domain->external_switch, typeid(start_indexing_response_success).name())
			->subscribe(domain->external_switch, typeid(start_indexing_response_fail).name());
	}

	void configure_switches_network(const object_ptr& domain) const override final {
		domain->external_switch
			->subscribe(domain, typeid(start_indexing_request).name());
	}

	void add_request_responders(const object_ptr& domain) const override final {
		domain->responders = {
			{ typeid(start_indexing_request).name(), &pi_domain::start_indexing_request_responder },
		};
	}

	void add_unused_response_type_names(const object_ptr& domain) const override final { }

	void add_domains(const object_ptr& domain) const override final { }

	void add_services(const object_ptr& domain) const override final {
		add_service<pi_page_analyzer_service>(domain);
		add_service<pi_db_responder_service> (domain);
		add_service<pi_page_dumper_service>  (domain);
		add_service<pi_page_indexing_service>(domain);
	}

	void set_power_levels(const object_ptr& domain) const override final {
		auto min_value = domain->get_services_power_value();

		for (auto i = domain->default_power_level; i < 3; ++i) {
			domain->levels_power.insert({ i, (i + 1) * min_value });
		}
	}
};

template<>
class se_cfg_validator_imp<pi_domain, pi_config> final : public se_cfg_validator<pi_domain, pi_config> {
private:
	void valid_existence(domain_ptr domain, config_ptr config) const {
		if (!config) {
			throw std::exception("Error! Domain was not setup or it was a try to setup with an unsuitable configuration file!");
		}
	}

	void valid_db_connection(domain_ptr domain, config_ptr config) const {
		try {
			auto con = pi_db_responder_service::get_mysql_connection(*config);
			con->close();
		} catch (const std::exception& ex) {
			throw std::exception("Error! Connection to database was failed! " + ex.what());
		}
	}

	void valid_sources(domain_ptr domain, config_ptr config) const {
		if (config->get_sources().empty()) {
			throw std::exception("Error! Empty sources.");
		}
	}

	void valid_indexing_mode(domain_ptr domain, config_ptr config) const {
		if (config->get_indexing_mode() <= indexing_modes::LOWER_BOUND ||
			config->get_indexing_mode() >= indexing_modes::UPPER_BOUND) {
			throw std::exception("Error! Invalid indexing mode.");
		}
	}

	void valid_power_value(domain_ptr domain, config_ptr config) const {
		if (domain->levels_power.find(config->get_power_level()) == domain->levels_power.end()) {
			throw std::exception("Error! Invalid power level.");
		}
	}

public:
	bool is_valid(domain_ptr domain, config_ptr config, std::string& error_msg) const override final {
		bool res = true;

		try {
			valid_existence(domain, config);
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