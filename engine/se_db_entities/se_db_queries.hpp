#pragma once

#include <functional>
#include <string>
#include <vector>
#include <exception>
#include <core/messages/message_core.hpp>
#include <thread_extensions/thread_safe_containers/thread_safe_unordered_map.hpp>

class se_db_queries {
protected:
	using query_generator = std::function<std::vector<std::string>(const std::shared_ptr<context>& args)>;

	static const std::string pages_info_tb_name;
	static const std::string words_info_tb_name;
	static const std::string search_index_tb_name;
	static const std::string sites_list_tb_name;

	static const se_encoding internal_encoding;

	thread_safe_unordered_map<std::string, query_generator> message_name_query_interpreter;

protected:
	static void to_mysql_format(const std::initializer_list<string_enc*> args) {
		static const std::string special_symbols = "\"\'\\";

		std::for_each(args.begin(), args.end(), [&](string_enc* val) {
			se_encoder::encode(val->str, val->enc, DEFAULT_ENCODING);

			for (auto i = 0; i < val->str.size(); ++i)
				if (special_symbols.find(val->str[i]) != std::string::npos)
					val->str.insert(i++, "\\");

			se_encoder::encode(val->str, DEFAULT_ENCODING, internal_encoding);
		});
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

	se_encoding get_encoding() const {
		return internal_encoding;
	}

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

const se_encoding se_db_queries::internal_encoding = encoding_t::UTF_8;