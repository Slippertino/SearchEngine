#pragma once

#include <engine/db_entities/se_db_queries.hpp>
#include "pi_internal_messages.hpp"

class pi_queries : public se_db_queries
{
protected:
	thread_safe_unordered_map<std::string, query_generator> get_message_name_query_interpreter() const override {
		return {
			{ typeid(init_database_request).name(),        &pi_queries::get_init_database_queries		 },
			{ typeid(is_unique_page_url_request).name(),   &pi_queries::get_is_unique_page_url_queries   },
			{ typeid(record_page_info_request).name(),     &pi_queries::get_record_page_info_queries     },
			{ typeid(sites_list_recording_request).name(), &pi_queries::get_sites_list_recording_queries },
			{ typeid(page_and_site_id_request).name(),     &pi_queries::get_page_and_site_id_queries     },
			{ typeid(record_word_info_request).name(),     &pi_queries::get_record_word_info_queries     },
			{ typeid(record_word_to_index_request).name(), &pi_queries::get_record_word_to_index_queries },
		};
	}

private:
	static std::vector<std::string> get_init_database_queries(const std::shared_ptr<context>& args) {
		std::vector<std::string> res;
		std::ostringstream ostr;

		auto req = *static_cast<init_database_request*>(args.get());

		to_mysql_format({ &req.database_name });

		ostr << "drop database if exists " << req.database_name.str << ";";
		reset(ostr, res);

		ostr << "create database " << req.database_name.str << ";";
		reset(ostr, res);

		ostr << "use " << req.database_name.str << ";";
		reset(ostr, res);

		ostr << "create table "
			 << "if not exists " << pages_info_tb_name << " ("
			 << "id int primary key auto_increment,"
			 << "path mediumtext not null,"
			 << "code int not null,"
			 << "content mediumtext not null);";
		reset(ostr, res);

		ostr << "create index " << pages_info_tb_name << "_index on " << pages_info_tb_name << "(path(50));";
		reset(ostr, res);

		ostr << "create table "
			 << "if not exists " << sites_list_tb_name << " ("
			 << "id int primary key auto_increment,"
			 << "url mediumtext not null);";
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

		ostr << "create unique index " << words_info_tb_name << "_index on " << words_info_tb_name << "(site_id, value(100),lang(30));";
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

		ostr << "create unique index " << search_index_tb_name << "_index on " << search_index_tb_name << "(page_id, word_id);";
		reset(ostr, res);

		return res;
	}

	static std::vector<std::string> get_sites_list_recording_queries(const std::shared_ptr<context>& args) {
		std::vector<std::string> res;
		std::ostringstream ostr;

		auto req = *static_cast<site_recording_request*>(args.get());

		to_mysql_format({ &req.site });

		ostr << "insert into " << sites_list_tb_name << " (url) values"
			 << "(\"" << req.site.str << "\");";
		reset(ostr, res);

		return res;
	}

	static std::vector<std::string> get_is_unique_page_url_queries(const std::shared_ptr<context>& args) {
		std::vector<std::string> res;
		std::ostringstream ostr;

		auto req = *static_cast<is_unique_page_url_request*>(args.get());

		to_mysql_format({ &req.url });

		ostr << "select path from " << pages_info_tb_name << "\n";
		ostr << "where path = \"" << req.url.str << "\"";
		reset(ostr, res);

		return res;
	}

	static std::vector<std::string> get_record_page_info_queries(const std::shared_ptr<context>& args) {
		std::vector<std::string> res;
		std::ostringstream ostr;

		auto req = *static_cast<record_page_info_request*>(args.get());

		to_mysql_format({ &req.path, &req.content });

		ostr << "insert into " << pages_info_tb_name << " (path, code, content) values"
			 << "(\"" << req.path.str << "\"," << req.code << ",\"" << req.content.str << "\");";
		reset(ostr, res);

		return res;
	}

	static std::vector<std::string> get_page_and_site_id_queries(const std::shared_ptr<context>& args) {
		std::vector<std::string> res;
		std::ostringstream ostr;

		auto req = *static_cast<page_and_site_id_request*>(args.get());

		to_mysql_format({ &req.page_url, &req.site_url });

		ostr << "select id from " << pages_info_tb_name << std::endl
			 << "where path = " << "\"" << req.page_url.str << "\"";
		reset(ostr, res);

		ostr << "select id from " << sites_list_tb_name << std::endl
			 << "where url = " << "\"" << req.site_url.str << "\"";
		reset(ostr, res);

		return res;
	}

	static std::vector<std::string> get_record_word_info_queries(const std::shared_ptr<context>& args) {
		std::vector<std::string> res;
		std::ostringstream ostr;

		auto req = *static_cast<record_word_info_request*>(args.get());

		for (auto& cur : req.words_params) {
			to_mysql_format({ &cur.first, &cur.second });
		}

		ostr << "update " << words_info_tb_name << std::endl
			 << "set frequency = frequency + 1 where ";
		for (auto i = 0; i < req.words_params.size(); ++i) {
			auto& cur = req.words_params[i];

			ostr << "(site_id = " << req.site_id << " and "
				 << "value = " << "\"" << cur.first.str << "\" and "
				 << "lang = " << "\"" << cur.second.str << "\")";

			if (i + 1 == req.words_params.size()) {
				ostr << ";";
			} else {
				ostr << " or ";
			}
		}
		reset(ostr, res);

		ostr << "insert ignore into " << words_info_tb_name << " (site_id, value, lang, frequency) values ";
		for (auto i = 0; i < req.words_params.size(); ++i) {
			auto& cur = req.words_params[i];

			ostr << "(" << req.site_id << ",\"" << cur.first.str << "\",\"" << cur.second.str << "\"," << 1 << ")";

			if (i + 1 == req.words_params.size()) {
				ostr << ";";
			}
			else {
				ostr << ",";
			}
		}
		reset(ostr, res);

		for (auto i = 0; i < req.words_params.size(); ++i) {
			auto& cur = req.words_params[i];
			ostr << "select id from " << words_info_tb_name << " where "
				 << "site_id = " << req.site_id << " and "
				 << "value = \"" << cur.first.str << "\" and "
				 << "lang = \"" << cur.second.str << "\";";
			reset(ostr, res);
		}

		return res;
	}

	static std::vector<std::string> get_record_word_to_index_queries(const std::shared_ptr<context>& args) {
		std::vector<std::string> res;
		std::ostringstream ostr;

		auto req = *static_cast<record_word_to_index_request*>(args.get());

		ostr << "insert into " << search_index_tb_name << " (page_id, word_id, ranking) values";

		for (auto i = 0; i < req.words_index_params.size(); ++i) {
			auto cur = req.words_index_params[i];

			ostr << "(" << std::get<0>(cur) << "," << std::get<1>(cur) << "," << std::get<2>(cur) << ")";

			if (i + 1 == req.words_index_params.size()) {
				ostr << ";";
			} else {
				ostr << ",";
			}
		}

		reset(ostr, res);

		return res;
	}

public:
	pi_queries() : se_db_queries(get_message_name_query_interpreter())
	{ }
};