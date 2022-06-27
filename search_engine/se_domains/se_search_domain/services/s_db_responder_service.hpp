#pragma once

#include <tools/en_de_coder.hpp>
#include <thread_safe_containers/thread_safe_queue.hpp>
#include <thread_safe_containers/thread_safe_unordered_map.hpp>
#include "../../se_service.hpp"
#include "../s_config.hpp"
#include "../s_messages.hpp"
#include "../../se_services_infrastructure/se_services_communication.hpp"
#include "../s_queries.hpp"
#include "../../se_db_responder.h"

class s_db_responder_service : public se_service<s_db_responder_service, s_config>,
							   public se_db_responder<s_queries> {
	SE_SERVICE(s_db_responder_service)

private:
	void database_integrity_check_request_responder(msg_request msg) {
		responder_components comps;
		get_components(msg, typeid(database_integrity_check_request).name(), comps);

		database_integrity_check_response resp;

		try {
			for (auto& q : comps.queries) {
				auto set = comps.statement->executeQuery(q);
				if (!set->rowsCount()) {
					throw std::exception("Non-existen database or unsuitable structure\n");
				}
			}

			resp.is_valid = true;
			resp.status.status = runtime_status::SUCCESS;
			resp.status.message = { "Successful query!\n", encoding_t::UTF_8 };
		}
		catch (const std::exception& ex) {
			resp.is_valid = false;
			resp.status.status = runtime_status::FAIL;
			resp.status.message = { "Fail query!\n DATABASE_INTEGRITY_CHECK : " + std::string(ex.what()), encoding_t::UTF_8 };
		}

		MAKE_RESPONSE(database_integrity_check, (
			msg.id,
			resp
		))

		connections.add(comps.connection);
	}

protected:
	void setup_base(s_config* config) override {
		std::thread init_thread(&s_db_responder_service::requests_handler, this);
		init_thread.detach();

		auto db_name = config->get_db_name();

		for (auto i = 0; i < pool.size(); ++i) {
			try {
				auto con = std::shared_ptr<sql::Connection>(CONNECT(config));
				con->setSchema(db_name);

				connections.add(con);
			} catch (std::exception& ex) {
				SE_LOG(ex.what());
			}
		}

		MAKE_REQUEST_WITH_RESPONSE(resp, database_integrity_check, (
			resp,
			string_enc{ db_name, encoding_t::ANSI }
		))

		if (!resp.is_valid) {
			stop_flag = true;
		}

		SE_LOG("Setup: " << resp.to_string() << "\n");
	}

	void clear() override {
		se_service<s_db_responder_service, s_config>::clear();

		while (!connections.empty()) {
			std::shared_ptr<sql::Connection> con;
			connections.wait_and_erase(con);
			con->close();
		}
	}

public:
	s_db_responder_service() = delete;
	s_db_responder_service(size_t id, const fs::path& root, const std::shared_ptr<se_router>& in_router) :
		se_service(id, root / R"(services)" / get_full_name(get_component_name()), in_router)
	{ }

	std::string get_component_name() const override {
		return std::string("db_responder_service");
	}
};

template<>
class builder<s_db_responder_service> : public abstract_service_builder<s_db_responder_service> {
protected:
	void add_subscriptions(const service_ptr& service) const override {
		service->router->subscribe<database_integrity_check_request>(service);
		service->router->subscribe<database_integrity_check_response>(service);
	}

	void add_power_distribution(const service_ptr& service) const override {
		service->power_distribution.push_back({ &s_db_responder_service::requests_handler, 1 });
	}

	void add_request_responders(const service_ptr& service) const override {
		service->responders.insert(typeid(database_integrity_check_request).name(),
								   &s_db_responder_service::database_integrity_check_request_responder);
	}

	void add_unused_response_type_names(const service_ptr& service) const override {
		return;
	}
};