#pragma once

#include <text_property_types/string_enc.hpp>
#include "../se_services_infrastructure/message_core.h"

struct database_integrity_check_request : context {
	string_enc database_name;

	SE_CONTEXT(
		database_integrity_check_request, 
		database_name
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		database_integrity_check_request, 
		database_name
	)
};

struct database_integrity_check_response : context {
	bool is_valid;
	response_status status;

	SE_CONTEXT(
		database_integrity_check_response,
		is_valid,
		status
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		database_integrity_check_response,
		is_valid,
		status
	)
};
