#pragma once

#include <tools/se_encoder.hpp>
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
				std::unique_ptr<sql::ResultSet> res(comps.statement->executeQuery(q));
				if (!res->rowsCount()) {
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

	void sites_id_request_responder(msg_request msg) {
		responder_components comps;
		get_components(msg, typeid(sites_id_request).name(), comps);

		sites_id_response resp;

		try {
			std::unique_ptr<sql::ResultSet> res(comps.statement->executeQuery(comps.queries[0]));
			res->beforeFirst(); res->next();
			auto count = res->getUInt64("id");

			for (auto i = 1; i <= count; ++i)
				resp.ids.push_back(i);

			resp.status.status = runtime_status::SUCCESS;
			resp.status.message = { "Successful query!\n", encoding_t::UTF_8 };
		}
		catch (const std::exception& ex) {
			resp.status.status = runtime_status::FAIL;
			resp.status.message = { "Fail query!\n SITES_ID : " + std::string(ex.what()), encoding_t::UTF_8 };
		}

		MAKE_RESPONSE(sites_id, (
			msg.id,
			resp
		))

		connections.add(comps.connection);
	}

	void words_info_request_responder(msg_request msg) {
		responder_components comps;
		get_components(msg, typeid(words_info_request).name(), comps);

		words_info_response resp;

		try {
			std::unique_ptr<sql::ResultSet> res_count(comps.statement->executeQuery(comps.queries[0]));
			res_count->beforeFirst(); res_count->next();
			resp.pages_count = res_count->getUInt64("result");

			std::unique_ptr<sql::ResultSet> res_info(comps.statement->executeQuery(comps.queries[1]));
			res_info->beforeFirst(); res_info->next();
			while (!res_info->isAfterLast()) {
				auto id   = res_info->getUInt64("id");
				auto freq = res_info->getUInt64("frequency");

				resp.words_id_freq.push_back({ id, freq });
			}

			resp.status.status = runtime_status::SUCCESS;
			resp.status.message = { "Successful query!\n", encoding_t::UTF_8 };
		}
		catch (const std::exception& ex) {
			resp.status.status = runtime_status::FAIL;
			resp.status.message = { "Fail query!\n WORDS_ID : " + std::string(ex.what()), encoding_t::UTF_8 };
		}

		MAKE_RESPONSE(words_info, (
			msg.id,
			resp
		))

		connections.add(comps.connection);
	}

	void pages_selection_request_responder(msg_request msg) {
		responder_components comps;
		get_components(msg, typeid(pages_selection_request).name(), comps);

		pages_selection_response resp;

		try {
			std::unique_ptr<sql::ResultSet> res(comps.statement->executeQuery(comps.queries[0]));

			res->beforeFirst(); res->next();
			while (!res->isAfterLast()) {
				auto id = res->getUInt64("page_id");
				resp.allowed_pages_ids.insert(id);
			}

			resp.status.status = runtime_status::SUCCESS;
			resp.status.message = { "Successful query!\n", encoding_t::UTF_8 };
		}
		catch (const std::exception& ex) {
			resp.status.status = runtime_status::FAIL;
			resp.status.message = { "Fail query!\n PAGES_SELECTION : " + std::string(ex.what()), encoding_t::UTF_8 };
		}

		MAKE_RESPONSE(pages_selection, (
			msg.id,
			resp
		))

		connections.add(comps.connection);
	}

	void page_content_request_responder(msg_request msg) {
		responder_components comps;
		get_components(msg, typeid(page_content_request).name(), comps);

		page_content_response resp;

		try {
			std::unique_ptr<sql::ResultSet> res = std::unique_ptr<sql::ResultSet>(
				comps.statement->executeQuery(comps.queries[0])
			);

			res->beforeFirst(); res->next();
			auto url     = std::string(res->getString("path"));
			auto content = std::string(res->getString("content"));

			auto db_encoding = queries_builder->get_encoding();
			se_encoder::encode(url,     db_encoding, DEFAULT_ENCODING);
			se_encoder::encode(content, db_encoding, DEFAULT_ENCODING);

			resp.url    =  string_enc{ url,     DEFAULT_ENCODING };
			resp.content = string_enc{ content, DEFAULT_ENCODING };

			resp.status.status = runtime_status::SUCCESS;
			resp.status.message = { "Successful query!\n", encoding_t::UTF_8 };
		}
		catch (const std::exception& ex) {
			resp.status.status = runtime_status::FAIL;
			resp.status.message = { "Fail query!\n PAGE_CONTENT : " + std::string(ex.what()), encoding_t::UTF_8 };
		}

		MAKE_RESPONSE(page_content, (
			msg.id,
			resp
		))

		connections.add(comps.connection);
	}

	void page_relevance_request_responder(msg_request msg) {
		responder_components comps;
		get_components(msg, typeid(page_relevance_request).name(), comps);

		page_relevance_response resp;

		try {
			std::unique_ptr<sql::ResultSet> res = std::unique_ptr<sql::ResultSet>(
				comps.statement->executeQuery(comps.queries[0])
			);

			res->beforeFirst(); res->next();
			resp.rank = res->getDouble("relevance");

			resp.status.status = runtime_status::SUCCESS;
			resp.status.message = { "Successful query!\n", encoding_t::UTF_8 };
		}
		catch (const std::exception& ex) {
			resp.status.status = runtime_status::FAIL;
			resp.status.message = { "Fail query!\n PAGE_RELEVANCE : " + std::string(ex.what()), encoding_t::UTF_8 };
		}

		MAKE_RESPONSE(page_relevance, (
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
		service->router->subscribe<sites_id_request>(service);
		service->router->subscribe<words_info_request>(service);
		service->router->subscribe<pages_selection_request>(service);
		service->router->subscribe<page_content_request>(service);
		service->router->subscribe<page_relevance_request>(service);
	}

	void add_power_distribution(const service_ptr& service) const override {
		service->power_distribution.push_back({ &s_db_responder_service::requests_handler, 1 });
	}

	void add_request_responders(const service_ptr& service) const override {
		service->responders.insert(typeid(database_integrity_check_request).name(),
								   &s_db_responder_service::database_integrity_check_request_responder);
		service->responders.insert(typeid(sites_id_request).name(),
								   &s_db_responder_service::sites_id_request_responder);
		service->responders.insert(typeid(words_info_request).name(),
								   &s_db_responder_service::words_info_request_responder);
		service->responders.insert(typeid(pages_selection_request).name(),
								   &s_db_responder_service::pages_selection_request_responder);
		service->responders.insert(typeid(page_content_request).name(),
								   &s_db_responder_service::page_content_request_responder);
		service->responders.insert(typeid(page_relevance_request).name(),
								   &s_db_responder_service::page_relevance_request_responder);
	}

	void add_unused_response_type_names(const service_ptr& service) const override {
		return;
	}
};