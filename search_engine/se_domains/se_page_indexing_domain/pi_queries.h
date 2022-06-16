#pragma once

#include "../se_db_queries.h"
#include "messages.h"

class pi_queries : public se_db_queries
{
protected:
	thread_safe_unordered_map<std::string, query_generator> get_message_name_query_interpreter() const override {
		return {
			{ typeid(init_database_request).name(),        &pi_queries::init_database        },
			{ typeid(is_unique_page_url_request).name(),   &pi_queries::is_unique_page_url   },
			{ typeid(record_page_info_request).name(),     &pi_queries::record_page_info     },
			{ typeid(site_recording_request).name(),       &pi_queries::site_recording       },
			{ typeid(page_and_site_id_request).name(),     &pi_queries::page_and_site_id     },
			{ typeid(record_word_info_request).name(),     &pi_queries::record_word_info     },
			{ typeid(record_word_to_index_request).name(), &pi_queries::record_word_to_index },
		};
	}

private:
	static std::vector<std::string> init_database(const std::shared_ptr<context>& args) {
		std::vector<std::string> res;
		std::ostringstream ostr;

		auto req = *static_cast<init_database_request*>(args.get());

		ostr << "drop database if exists " << req.database_name << ";";
		reset(ostr, res);

		ostr << "create database " << req.database_name << ";";
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

		ostr << "create table "
			 << "if not exists " << words_info_tb_name << " ("
			 << "id int primary key auto_increment,"
			 << "site_id int not null,"
			 << "value text not null,"
			 << "lang text not null,"
			 << "frequency int not null,"
			 << "foreign key(site_id) references " << sites_list_tb_name << "(id) on delete cascade);";
		reset(ostr, res);

		ostr << "truncate table " << words_info_tb_name << ";";
		reset(ostr, res);

		ostr << "create table "
			 << "if not exists " << search_index_tb_name << " ("
			 << "id int primary key auto_increment,"
			 << "page_id int not null,"
			 << "word_id int not null,"
			 << "ranking float not null,"
			 << "foreign key(page_id) references " << pages_info_tb_name << "(id) on delete cascade,"
			 << "foreign key(word_id) references " << words_info_tb_name << "(id) on delete cascade);";
		reset(ostr, res);

		ostr << "truncate table " << search_index_tb_name << ";";
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

	static std::vector<std::string> page_and_site_id(const std::shared_ptr<context>& args) {
		std::vector<std::string> res;
		std::ostringstream ostr;

		auto req = *static_cast<page_and_site_id_request*>(args.get());

		ostr << "select id from " << pages_info_tb_name << std::endl
			 << "where path = " << "\"" << req.page_url << "\"";
		reset(ostr, res);

		ostr << "select id from " << sites_list_tb_name << std::endl
			 << "where url = " << "\"" << req.site_url << "\"";
		reset(ostr, res);

		return res;
	}

	static std::vector<std::string> record_word_info(const std::shared_ptr<context>& args) {
		std::vector<std::string> res;
		std::ostringstream ostr;

		auto req = *static_cast<record_word_info_request*>(args.get());

		ostr << "update " << words_info_tb_name << std::endl
			 << "set frequency = frequency + 1 where "
			 << "site_id = " << req.site_id << ","
			 << "value = " << "\"" << req.stemmed_word << "\","
			 << "lang = " << "\"" << req.lang << "\";";
		reset(ostr, res);

		ostr << "insert into " << words_info_tb_name << " (site_id, value, lang, frequency) values"
			 << "(" << req.site_id << ",\"" << req.stemmed_word << "\"" << req.lang << "\"," << 0 << ");";
		reset(ostr, res);

		ostr << "select id from " << words_info_tb_name << " where "
			 << "site_id = " << req.site_id 
			 << ", value = \"" << req.stemmed_word 
			 << "\", lang = \"" << req.lang << "\";";
		reset(ostr, res);

		return res;
	}

	static std::vector<std::string> record_word_to_index(const std::shared_ptr<context>& args) {
		std::vector<std::string> res;
		std::ostringstream ostr;

		auto req = *static_cast<record_word_to_index_request*>(args.get());

		ostr << "insert into " << search_index_tb_name << " (page_id, word_id, ranking) values"
			 << "(" << req.page_id << "," << req.word_id << "," << req.rank << ");";
		reset(ostr, res);

		return res;
	}

public:
	pi_queries() : se_db_queries(get_message_name_query_interpreter())
	{ }
};