#pragma once

#include "../se_db_queries.h"
#include "s_messages.hpp"

class s_queries : public se_db_queries {
protected:
	thread_safe_unordered_map<std::string, query_generator> get_message_name_query_interpreter() const override {
		return {
			{ typeid(database_integrity_check_request).name(), &s_queries::database_integrity_check },
			{ typeid(sites_id_request).name(),				   &s_queries::sites_id				    },
			{ typeid(words_info_request).name(),			   &s_queries::words_id					},
			{ typeid(pages_selection_request).name(),          &s_queries::pages_selection			},
			{ typeid(page_content_request).name(),             &s_queries::page_content			    },
			{ typeid(page_relevance_request).name(),           &s_queries::page_relevance		    },
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

	static std::vector<std::string> sites_id(const std::shared_ptr<context>& args) {
		std::vector<std::string> res;
		std::ostringstream ostr;

		auto req = *static_cast<sites_id_request*>(args.get());

		ostr << "select count(*) from " << sites_list_tb_name;
		reset(ostr, res);

		return res;
	}

	static std::vector<std::string> words_id(const std::shared_ptr<context>& args) {
		std::vector<std::string> res;
		std::ostringstream ostr;

		auto req = *static_cast<words_info_request*>(args.get());

		for (auto& cur : req.stemmed_words) {
			to_mysql_format({ &cur.first, &cur.second});
		}

		ostr << "select count(*) as result from " << pages_info_tb_name << "\n";
		reset(ostr, res);

		ostr << "select id, frequency from " << words_info_tb_name << " where ";

		for (auto i = 0; i < req.stemmed_words.size(); ++i) {
			ostr << "(site_id = " << req.site_id
				 << " and value = \"" << req.stemmed_words[i].first.str << "\""
				 << " and lang = \"" << req.stemmed_words[i].second.str << "\")";

			if (i == req.stemmed_words.size() - 1) {
				ostr << ";";
			} else {
				ostr << " or ";
			}
		}
		reset(ostr, res);
	
		return res;
	}

	static std::vector<std::string> pages_selection(const std::shared_ptr<context>& args) {
		std::vector<std::string> res;
		std::ostringstream ostr;

		auto req = *static_cast<pages_selection_request*>(args.get());

		ostr << "select page_id from " << search_index_tb_name << " where "
			 << "word_id = " << req.word_id
			 << " and page_id in (" << *(req.allowed_pages_ids.begin());

		for (auto& v : req.allowed_pages_ids) {
			ostr << "," << v;
		}

		ostr << ");\n";

		reset(ostr, res);

		return res;
	}

	static std::vector<std::string> page_content(const std::shared_ptr<context>& args) {
		std::vector<std::string> res;
		std::ostringstream ostr;

		auto req = *static_cast<page_content_request*>(args.get());

		ostr << "select path, content from " << pages_info_tb_name << " where "
			 << "id = " << req.page_id << ";\n";

		reset(ostr, res);

		return res;
	}

	static std::vector<std::string> page_relevance(const std::shared_ptr<context>& args) {
		std::vector<std::string> res;
		std::ostringstream ostr;

		auto req = *static_cast<page_relevance_request*>(args.get());

		ostr << "select sum(ranking) as relevance from " << search_index_tb_name
			 << " where page_id = " << req.page_id << " and word_id in (" << *req.words_ids.begin();

		for (auto& id : req.words_ids) {
			ostr << "," << id;
		}

		ostr << ");\n";

		reset(ostr, res);

		return res;
	}

public:
	s_queries() : se_db_queries(get_message_name_query_interpreter())
	{ }
};