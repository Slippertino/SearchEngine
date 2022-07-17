#pragma once

#include <thread_extensions/thread_safe_containers/thread_safe_queue.hpp>
#include <thread_extensions/thread_safe_containers/thread_safe_unordered_map.hpp>
#include <core/se_service.hpp>
#include <core/builders/se_service_builder.hpp>
#include <engine/se_db_entities/se_db_responder.hpp>
#include <engine/se_db_entities/se_db_connection.hpp>
#include "../messages/pi_internal_messages.hpp"
#include "../pi_queries.hpp"
#include "../pi_config.hpp"

class pi_db_responder_service final : public se_service<pi_db_responder_service, pi_config>,
								public se_db_responder<pi_queries> {
	SE_SERVICE(pi_db_responder_service, db_responder_service)

private:
	void init_database_request_responder(msg_request msg) {
		responder_components comps;
		get_components(msg, typeid(init_database_request).name(), comps);

		init_database_response resp;

		try {
			for (auto& cur : comps.queries)
				auto count = comps.statement->executeUpdate(cur);
			resp.status.status = runtime_status::SUCCESS;
			resp.status.message = { "Successful query!\n", encoding_t::UTF_8 };
		}
		catch (const std::exception& ex) {
			resp.status.status = runtime_status::FAIL;
			resp.status.message = { "Fail query!\n INITIALIZATION : " + std::string(ex.what()), encoding_t::UTF_8 };
		}

		MAKE_RESPONSE(init_database_response, (
			msg.id,
			resp
		))

		free_components(comps);
	}

	void sites_list_recording_request_responder(msg_request msg) {
		responder_components comps;
		get_components(msg, typeid(site_recording_request).name(), comps);

		site_recording_response resp;

		try {
			auto count = comps.statement->executeUpdate(comps.queries[0]);

			if (!count) {
				throw std::exception("Table of sites has not been successfully updated!\n");
			}

			resp.status.status = runtime_status::SUCCESS;
			resp.status.message = { "Successful query!\n", encoding_t::UTF_8 };
		}
		catch (const std::exception& ex) {
			resp.status.status = runtime_status::FAIL;
			resp.status.message = { "Fail query!\n SITES_LIST_RECORDING : " + std::string(ex.what()), encoding_t::UTF_8 };
		}

		MAKE_RESPONSE(site_recording_response, (
			msg.id,
			resp
		))

		free_components(comps);
	}

	void record_page_info_request_responder(msg_request msg) {
		responder_components comps;
		get_components(msg, typeid(record_page_info_request).name(), comps);

		record_page_info_response resp;

		try {
			auto count = comps.statement->executeUpdate(comps.queries[0]);

			if (!count) {
				throw std::exception("Table of pages has not been successfully updated!\n");
			}

			resp.status.status = runtime_status::SUCCESS;
			resp.status.message = { "Successful query!\n", encoding_t::UTF_8 };
		}
		catch (const std::exception& ex) {
			resp.status.status = runtime_status::FAIL;
			resp.status.message = { "Fail query!\n RECORD_PAGE_INFO : " + std::string(ex.what()), encoding_t::UTF_8 };
		}

		MAKE_RESPONSE(record_page_info_response, (
			msg.id,
			resp
		))

		free_components(comps);
	}

	void is_unique_page_url_request_responder(msg_request msg) {
		responder_components comps;
		get_components(msg, typeid(is_unique_page_url_request).name(), comps);

		is_unique_page_url_response resp;

		try {
			std::unique_ptr<sql::ResultSet> count(comps.statement->executeQuery(comps.queries[0]));
			resp.answer = !count->rowsCount();
			resp.status.status = runtime_status::SUCCESS;
			resp.status.message = { "Successful query!\n", encoding_t::UTF_8 };
		}
		catch (const std::exception& ex) {
			resp.status.status = runtime_status::FAIL;
			resp.status.message = { "Fail query!\n IS_UNIQUE_PAGE_URL : " + std::string(ex.what()), encoding_t::UTF_8 };
		}

		MAKE_RESPONSE(is_unique_page_url_response, (
			msg.id,
			resp
		))

		free_components(comps);
	}

	void page_and_site_id_request_responder(msg_request msg) {
		responder_components comps;
		get_components(msg, typeid(page_and_site_id_request).name(), comps);

		page_and_site_id_response resp;

		try {
			std::unique_ptr<sql::ResultSet> page_query(comps.statement->executeQuery(comps.queries[0]));
			std::unique_ptr<sql::ResultSet> site_query(comps.statement->executeQuery(comps.queries[1]));

			if (!page_query->rowsCount() || !site_query->rowsCount())
				throw std::exception("Unexisted url!\n");

			page_query->beforeFirst(); page_query->next();
			site_query->beforeFirst(); site_query->next();

			resp.page_id = page_query->getUInt64("id");
			resp.site_id = site_query->getUInt64("id");

			resp.status.status = runtime_status::SUCCESS;
			resp.status.message = { "Successful query!\n", encoding_t::UTF_8 };
		}
		catch (const std::exception& ex) {
			resp.status.status = runtime_status::FAIL;
			resp.status.message = { "Fail query!\n PAGE_AND_SITE_ID : " + std::string(ex.what()), encoding_t::UTF_8 };
		}

		MAKE_RESPONSE(page_and_site_id_response, (
			msg.id,
			resp
		))

		free_components(comps);
	}

	void record_word_info_request_responder(msg_request msg) {
		responder_components comps;
		get_components(msg, typeid(record_word_info_request).name(), comps);

		record_word_info_response resp;

		try {
			auto updated_cnt = comps.statement->executeUpdate(comps.queries[0]);
			auto inserted_cnt = comps.statement->executeUpdate(comps.queries[1]);

			if (!updated_cnt && !inserted_cnt) {
				throw std::exception("Table of words has not been successfully updated!\n");
			}

			for (auto i = 2; i < comps.queries.size(); ++i) {
				std::unique_ptr<sql::ResultSet> res(comps.statement->executeQuery(comps.queries[i]));
				res->beforeFirst(); res->next();
				resp.words_id.push_back(res->getUInt64("id"));
			}

			resp.status.status = runtime_status::SUCCESS;
			resp.status.message = { "Successful query!\n", encoding_t::UTF_8 };
		}
		catch (const std::exception& ex) {
			resp.status.status = runtime_status::FAIL;
			resp.status.message = { "Fail query!\n RECORD_WORD_INFO : " + std::string(ex.what()), encoding_t::UTF_8 };
		}

		MAKE_RESPONSE(record_word_info_response, (
			msg.id,
			resp
		))

		free_components(comps);
	}

	void record_word_to_index_request_responder(msg_request msg) {
		responder_components comps;
		get_components(msg, typeid(record_word_to_index_request).name(), comps);

		record_word_to_index_response resp;

		try {
			auto count = comps.statement->executeUpdate(comps.queries[0]);

			if (!count) {
				throw std::exception("Table of indexes has not been successfully updated!\n");
			}

			resp.status.status = runtime_status::SUCCESS;
			resp.status.message = { "Successful query!\n", encoding_t::UTF_8 };
		}
		catch (const std::exception& ex) {
			resp.status.status = runtime_status::FAIL;
			resp.status.message = { "Fail query!\n RECORD_WORD_TO_INDEX : " + std::string(ex.what()), encoding_t::UTF_8 };
		}

		MAKE_RESPONSE(record_word_to_index_response, (
			msg.id,
			resp
		))

		free_components(comps);
	}

protected:
	void setup_base(pi_config* config) override final {
		std::thread init_thread(&pi_db_responder_service::requests_handler, this);
		init_thread.detach();

		auto db_name = config->get_db_name();

		for (auto i = 0; i < pool.size(); ++i) {
			try {
				auto con = std::shared_ptr<sql::Connection>(get_mysql_connection(*config));
				con->setSchema(db_name);

				connections.add(con);
			}
			catch (std::exception& ex) {
				SE_LOG(ex.what());
			}
		}

		MAKE_REQUEST_WITH_RESPONSE(resp, init_database_request, init_database_response, (
			resp,
			string_enc{ db_name, DEFAULT_ENCODING }
		))

		SE_LOG("Setup: " << resp.to_string() << "\n");
	}

	void clear() override final {
		se_service<pi_db_responder_service, pi_config>::clear();

		while (!connections.empty()) {
			std::shared_ptr<sql::Connection> con;
			connections.wait_and_erase(con);
			con->close();
		}
	}
};

template<>
class se_builder_imp<pi_db_responder_service> final : public se_service_builder<pi_db_responder_service> {
protected:
	void configure_own_network(const object_ptr& service) const override final {
		service
			->subscribe(service->internal_switch, typeid(init_database_request).name())
			->subscribe(service->internal_switch, typeid(init_database_response).name())
			->subscribe(service->internal_switch, typeid(record_page_info_response).name())
			->subscribe(service->internal_switch, typeid(is_unique_page_url_response).name())
			->subscribe(service->internal_switch, typeid(sites_list_recording_response).name())
			->subscribe(service->internal_switch, typeid(page_and_site_id_response).name())
			->subscribe(service->internal_switch, typeid(record_word_info_response).name())
			->subscribe(service->internal_switch, typeid(record_word_to_index_response).name());
	}

	void configure_switches_network(const object_ptr& service) const override final {
		service->internal_switch
			->subscribe(service, typeid(init_database_request).name())
			->subscribe(service, typeid(init_database_response).name())
			->subscribe(service, typeid(record_page_info_request).name())
			->subscribe(service, typeid(is_unique_page_url_request).name())
			->subscribe(service, typeid(sites_list_recording_request).name())
			->subscribe(service, typeid(page_and_site_id_request).name())
			->subscribe(service, typeid(record_word_info_request).name())
			->subscribe(service, typeid(record_word_to_index_request).name());
	}

	void add_request_responders(const object_ptr& service) const override final {
		service->responders = {
			{ typeid(init_database_request).name(),         &pi_db_responder_service::init_database_request_responder        },
			{ typeid(is_unique_page_url_request).name(),    &pi_db_responder_service::is_unique_page_url_request_responder   },
			{ typeid(record_page_info_request).name(),      &pi_db_responder_service::record_page_info_request_responder	 },
			{ typeid(sites_list_recording_request).name(),  &pi_db_responder_service::sites_list_recording_request_responder },
			{ typeid(page_and_site_id_request).name(),      &pi_db_responder_service::page_and_site_id_request_responder	 },
			{ typeid(record_word_info_request).name(),      &pi_db_responder_service::record_word_info_request_responder	 },
			{ typeid(record_word_to_index_request).name(),  &pi_db_responder_service::record_word_to_index_request_responder },
		};
	}

	void add_unused_response_type_names(const object_ptr& service) const override final {
		return;
	}

	void add_power_distribution(const object_ptr& service) const override final {
		service->power_distribution = {
			{ &pi_db_responder_service::requests_handler, 1 },
		};
	}
};