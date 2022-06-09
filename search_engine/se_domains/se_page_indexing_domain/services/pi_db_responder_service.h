#pragma once

#include <en_de_coder.h>
#include <thread_safe_containers/thread_safe_queue.hpp>
#include <thread_safe_containers/thread_safe_unordered_map.hpp>
#include "../../se_service.hpp"
#include "../messages.h"
#include "../../se_services_infrastructure/se_services_communication.hpp"
#include "../pi_queries.h"
#include "../../se_db_responder.h"

class pi_db_responder_service : public se_service<pi_db_responder_service>,
								public se_db_responder<pi_queries>
{
	SE_SERVICE(pi_db_responder_service)

private:
	void init_database_request_responder(msg_request msg)
	{
		responder_components comps;
		get_components(msg, typeid(init_database_request).name(), comps);

		init_database_response resp;

		try {
			for (auto& cur : comps.queries)
				auto count = comps.statement->executeUpdate(cur);
			resp.status.status = runtime_status::SUCCESS;
			resp.status.message = "Successful query!\n";
		}
		catch (const std::exception& ex) {
			resp.status.status = runtime_status::FAIL;
			resp.status.message = "Fail query!\n" + std::string(ex.what());
		}

		MAKE_RESPONSE(init_database, (
			msg.id,
			resp
		))

		connections.add(comps.connection);
	}

	void sites_list_recording_request_responder(msg_request msg)
	{
		responder_components comps;
		get_components(msg, typeid(site_recording_request).name(), comps);

		site_recording_response resp;

		try {
			auto count = comps.statement->executeUpdate(comps.queries[0]);
			resp.status.status = runtime_status::SUCCESS;
			resp.status.message = "Successful query!\n";
		}
		catch (const std::exception& ex) {
			resp.status.status = runtime_status::FAIL;
			resp.status.message = "Fail query!\n" + std::string(ex.what());
		}

		MAKE_RESPONSE(site_recording, (
			msg.id,
			resp
		))

		connections.add(comps.connection);
	}

	void record_page_info_request_responder(msg_request msg)
	{
		responder_components comps;
		get_components(msg, typeid(record_page_info_request).name(), comps);

		record_page_info_response resp;

		try {
			auto count = comps.statement->executeUpdate(comps.queries[0]);
			resp.status.status = runtime_status::SUCCESS;
			resp.status.message = "Successful query!\n";
		}
		catch (const std::exception& ex) {
			resp.status.status = runtime_status::FAIL;
			resp.status.message = "Fail query!\n RECORD_PAGE_INFO" + std::string(ex.what());
		}

		MAKE_RESPONSE(record_page_info, (
			msg.id,
			resp
		))

		connections.add(comps.connection);
	}

	void is_unique_page_url_request_responder(msg_request msg)
	{
		responder_components comps;
		get_components(msg, typeid(is_unique_page_url_request).name(), comps);

		is_unique_page_url_response resp;

		try {
			std::unique_ptr<sql::ResultSet> count(comps.statement->executeQuery(comps.queries[0]));
			resp.answer = !count->rowsCount();
			resp.status.status = runtime_status::SUCCESS;
			resp.status.message = "Successful query!\n";
		}
		catch (const std::exception& ex) {
			resp.status.status = runtime_status::FAIL;
			resp.status.message = "Fail query!\n" + std::string(ex.what());
		}

		MAKE_RESPONSE(is_unique_page_url, (
			msg.id,
			resp
		))

		connections.add(comps.connection);
	}

	void initialize(const configuration& config) {
		std::thread init_thread(&pi_db_responder_service::requests_handler, this);
		init_thread.detach();

		auto db_name = config.get_db_name();

		for (auto i = 0; i < pool.size(); ++i) {
			try {
				auto con = std::shared_ptr<sql::Connection>(CONNECT(config));
				con->setSchema(db_name);

				connections.add(con);
			}
			catch (std::exception& ex) {
				SE_LOG(ex.what());
			}
		}

		MAKE_REQUEST_WITH_RESPONSE(resp, init_database, (
			resp,
			db_name
		))

		SE_LOG("Init result : " << resp.to_string() << "\n");
	}

protected:
	std::string get_component_name() const override {
		return std::string("db_responder_service");
	}

	void clear() override {
		while (!connections.empty()) {
			std::shared_ptr<sql::Connection> con;
			connections.wait_and_erase(con);
			con->close();
		}
	}

public:
	pi_db_responder_service() = delete;
	pi_db_responder_service(size_t id, const fs::path& root, const std::shared_ptr<se_router>& in_router) :
		se_service(id, root / R"(services)" / get_full_name(get_component_name()), in_router)
	{ }

	void setup(const configuration& config) override {
		try {
			initialize(config);
			SE_LOG("Successful setup!\n");
		} catch (const std::exception& ex) {
			SE_LOG("Unsuccessful setup! " << ex.what() << "\n");
			throw ex;
		}
	}
};

template<>
class builder<pi_db_responder_service> : public abstract_service_builder<pi_db_responder_service>
{
protected:
	void add_subscriptions(const service_ptr& service) const override {
		service->router->subscribe<init_database_request>(service);
		service->router->subscribe<init_database_response>(service);
		service->router->subscribe<record_page_info_request>(service);
		service->router->subscribe<is_unique_page_url_request>(service);
		service->router->subscribe<site_recording_request>(service);
	}

	void add_power_distribution(const service_ptr& service) const override {
		service->power_distribution.push_back({ &pi_db_responder_service::requests_handler, 1 });
	}

	void add_request_responders(const service_ptr& service) const override {
		service->responders.insert(typeid(init_database_request).name(),
								   &pi_db_responder_service::init_database_request_responder);
		service->responders.insert(typeid(is_unique_page_url_request).name(),
								   &pi_db_responder_service::is_unique_page_url_request_responder);
		service->responders.insert(typeid(record_page_info_request).name(),
								   &pi_db_responder_service::record_page_info_request_responder);
		service->responders.insert(typeid(site_recording_request).name(),
								   &pi_db_responder_service::sites_list_recording_request_responder);
	}

	void configure_logger(const service_ptr& service) const override {
		auto logger = se_loggers_storage::get_instance()->get_logger(service->id);
		auto name   = service->get_component_name();

		logger->add_file(
			se_logger::get_code(name),
			name + std::string("_process"),
			service->logger_path
		);

		logger->add_file(
			se_logger::get_code(name, std::to_string(static_cast<size_t>(message_type::REQUEST))),
			name + std::string("_requests"),
			service->logger_path
		);

		logger->add_file(
			se_logger::get_code(name, std::to_string(static_cast<size_t>(message_type::RESPONSE))),
			name + std::string("_responses"),
			service->logger_path
		);
	}
};