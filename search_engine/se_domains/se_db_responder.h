#pragma once

#include <mysql/jdbc.h>
#include "se_services_infrastructure/message_core.h"
#include <thread_safe_containers/thread_safe_queue.hpp>
#include "se_db_queries.h"

template<typename q_builer>
class se_db_responder
{
protected:
	#define CONNECT(config) sql::mysql::get_mysql_driver_instance()->connect(config.get_db_url(), config.get_db_user_name(), config.get_db_password())

	struct responder_components {
		std::shared_ptr<sql::Connection> connection;
		std::shared_ptr<sql::Statement> statement;
		std::vector<std::string> queries;
	};

protected:
	thread_safe_queue<std::shared_ptr<sql::Connection>> connections;
	std::shared_ptr<se_db_queries> queries_builder = std::make_shared<q_builer>();

protected:
	void get_components(const msg_request& msg,
						const std::string& request_name,
						responder_components& comps) {

		connections.wait_and_erase(comps.connection);
		comps.statement = std::shared_ptr<sql::Statement>(comps.connection->createStatement());
		comps.queries = queries_builder->get_query_text(request_name, msg.body);
	}
};