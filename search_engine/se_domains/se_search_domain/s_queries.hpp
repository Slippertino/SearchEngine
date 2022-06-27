#pragma once

#include "../se_db_queries.h"
#include "s_messages.hpp"

class s_queries : public se_db_queries {
protected:
	thread_safe_unordered_map<std::string, query_generator> get_message_name_query_interpreter() const override {
		return {
			{typeid(database_integrity_check_request).name(), &s_queries::database_integrity_check},
		};
	}

private:
	static std::vector<std::string> database_integrity_check(const std::shared_ptr<context>& args) {
		std::vector<std::string> res;
		std::ostringstream ostr;

		auto req = *static_cast<database_integrity_check_request*>(args.get());

		to_mysql_format({ &req.database_name });

		ostr << "show databases like "
			 << "\"" << req.database_name << "\"";
		reset(ostr, res);

		ostr << "use " << req.database_name << ";"
			 << "show tables like "
			 << "\"" << sites_list_tb_name << "\"";
		reset(ostr, res);

		ostr << "use " << req.database_name << ";"
			<< "show tables like "
			<< "\"" << pages_info_tb_name << "\"";
		reset(ostr, res);

		ostr << "use " << req.database_name << ";"
			<< "show tables like "
			<< "\"" << words_info_tb_name << "\"";
		reset(ostr, res);

		ostr << "use " << req.database_name << ";"
			<< "show tables like "
			<< "\"" << search_index_tb_name << "\"";
		reset(ostr, res);

		return res;
	}

public:
	s_queries() : se_db_queries(get_message_name_query_interpreter())
	{ }
};