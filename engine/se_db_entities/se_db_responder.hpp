#pragma once

#include <core/messages/message_core.hpp>
#include <thread_extensions/thread_safe_containers/thread_safe_queue.hpp>
#include "se_db_queries.hpp"
#include "se_db_connection.hpp"

template<class q_builder>
class se_db_responder {
protected:
	struct responder_components {
		std::shared_ptr<sql::Connection> connection;
		std::shared_ptr<sql::Statement> statement;
		std::vector<std::string> queries;
	};

protected:
	thread_safe_queue<std::shared_ptr<sql::Connection>> connections;
	std::shared_ptr<se_db_queries> queries_builder = std::make_shared<q_builder>();
	
protected:
	void get_components(const msg_request& msg,
						const std::string& request_name,
						responder_components& comps) {

		connections.wait_and_erase(comps.connection);
		comps.statement = std::shared_ptr<sql::Statement>(comps.connection->createStatement());
		comps.queries = queries_builder->get_query_text(request_name, msg.body);
	}

	void free_components(responder_components& comps) {
		connections.add(comps.connection);
		comps.statement->close();
	}

public:
	template<class config_t>
	static auto get_mysql_connection(const config_t& config) {
		return se_db_connection::get_mysql_connection(
			config.get_db_url(),
			config.get_db_user_name(),
			config.get_db_password()
		);
	}
};