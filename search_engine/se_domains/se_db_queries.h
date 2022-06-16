#pragma once

#include <functional>
#include <string>
#include <vector>
#include <exception>
#include <thread_safe_containers/thread_safe_unordered_map.hpp>
#include "se_services_infrastructure/message_core.h"

class se_db_queries 
{
protected:
	using query_generator = std::function<std::vector<std::string>(const std::shared_ptr<context>& args)>;

	static const std::string pages_info_tb_name;
	static const std::string words_info_tb_name;
	static const std::string search_index_tb_name;
	static const std::string sites_list_tb_name;

	thread_safe_unordered_map<std::string, query_generator> message_name_query_interpreter;

protected:
	static void to_mysql_format(std::string& str) {
		const std::string special_symbols = "\"\'\\";

		for (auto i = 0; i < str.size(); ++i)
			if (special_symbols.find(str[i]) != std::string::npos)
				str.insert(i++, "\\");
	}

	static void reset(std::ostringstream& ostr, std::vector<std::string>& buff) {
		buff.push_back(ostr.str());
		ostr.str("");
	}

	virtual thread_safe_unordered_map<std::string, query_generator> get_message_name_query_interpreter() const = 0;

public:
	se_db_queries() = delete;
	se_db_queries(thread_safe_unordered_map<std::string, query_generator>&& m_n_q_i) 
		: message_name_query_interpreter(std::move(m_n_q_i))
	{ }

	std::vector<std::string> get_query_text(std::string name,
											const std::shared_ptr<context>& args) {
		query_generator gen;
		if (message_name_query_interpreter.try_get(name, gen))
			return gen(args);
		else
			throw std::exception("unknown message type");
	}
};

const std::string se_db_queries::pages_info_tb_name   = "pages";
const std::string se_db_queries::words_info_tb_name   = "words";
const std::string se_db_queries::search_index_tb_name = "search_index";
const std::string se_db_queries::sites_list_tb_name   = "sites";
