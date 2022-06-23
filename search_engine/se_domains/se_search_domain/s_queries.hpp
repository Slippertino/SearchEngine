#pragma once

#include "../se_db_queries.h"
#include "s_messages.h"

class s_queries : public se_db_queries {
protected:
	thread_safe_unordered_map<std::string, query_generator> get_message_name_query_interpreter() const override {
		return {
			
		};
	}

private:

public:
	s_queries() : se_db_queries(get_message_name_query_interpreter())
	{ }
};