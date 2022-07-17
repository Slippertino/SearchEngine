#pragma once

#include <core/messages/message_core.hpp>

struct start_indexing_response_success : context {
	bool result;

	SE_CONTEXT(
		start_indexing_response_success, 2,
		result
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		start_indexing_response_success,
		result
	)
};

struct start_indexing_response_fail : context {
	bool result;
	std::string error;

	SE_CONTEXT(
		start_indexing_response_fail, 2,
		result,
		error
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		start_indexing_response_fail,
		result,
		error
	)
};