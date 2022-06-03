#pragma once

#include "../se_db_queries.h"
#include "messages.h"

class pi_queries : public se_db_queries
{
protected:
	thread_safe_unordered_map<std::string, query_generator> get_message_name_query_interpreter() const override {
		return {
			{typeid(init_database_request).name(),        &pi_queries::init_database       },
			{typeid(is_unique_page_url_request).name(),   &pi_queries::is_unique_page_url  },
			{typeid(record_page_info_request).name(),     &pi_queries::record_page_info    },
			{typeid(site_recording_request).name(),       &pi_queries::site_recording      },
		};
	}

private:
	static std::vector<std::string> init_database(const std::shared_ptr<context>& args) {
		std::vector<std::string> res;
		std::ostringstream ostr;

		auto req = *static_cast<init_database_request*>(args.get());

		ostr << "create database "
			<< "if not exists " << req.database_name << ";";
		reset(ostr, res);

		ostr << "use " << req.database_name << ";";
		reset(ostr, res);

		ostr << "create table "
			<< "if not exists " << pages_info_tb_name << " ("
			<< "id int primary key auto_increment,"
			<< "path mediumtext not null,"
			<< "code int not null,"
			<< "content mediumtext not null);";
		reset(ostr, res);

		ostr << "truncate table " << pages_info_tb_name << ";";
		reset(ostr, res);

		ostr << "create table "
			 << "if not exists " << sites_list_tb_name << " ("
			 << "id int primary key auto_increment,"
			 << "url mediumtext not null);";
		reset(ostr, res);

		ostr << "truncate table " << sites_list_tb_name << ";";
		reset(ostr, res);

		return res;
	}

	static std::vector<std::string> site_recording(const std::shared_ptr<context>& args) {
		std::vector<std::string> res;
		std::ostringstream ostr;

		auto req = *static_cast<site_recording_request*>(args.get());

		ostr << "insert into " << sites_list_tb_name << " (url) values"
			 << "(\"" << req.site << "\");";
		reset(ostr, res);

		return res;
	}

	static std::vector<std::string> is_unique_page_url(const std::shared_ptr<context>& args) {
		std::vector<std::string> res;
		std::ostringstream ostr;

		auto req = *static_cast<is_unique_page_url_request*>(args.get());

		ostr << "select path from "
			<< pages_info_tb_name << "\n";
		ostr << "where path = \"" << req.url << "\"";
		reset(ostr, res);

		return res;
	}

	static std::vector<std::string> record_page_info(const std::shared_ptr<context>& args) {
		std::vector<std::string> res;
		std::ostringstream ostr;

		auto req = *static_cast<record_page_info_request*>(args.get());

		to_mysql_format(req.content);

		ostr << "insert into " << pages_info_tb_name << " (path, code, content) values"
			<< "(\"" << req.path << "\"," << req.code << ",\"" << req.content << "\");";
		reset(ostr, res);

		return res;
	}
public:
	pi_queries() : se_db_queries(get_message_name_query_interpreter())
	{ }
};