#pragma once

#include <core/messages/message_core.hpp>
#include <search_engine_analyzers/tools/html_text_analyzer.hpp>s
#include <search_engine_analyzers/text_property_types/se_encoding.hpp>
#include <search_engine_analyzers/text_property_types/se_language.hpp>
#include "pi_config.hpp"

struct start_indexing_request : context {
	pi_config config;

	SE_CONTEXT(
		start_indexing_request, 2,
		config,
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		start_indexing_request,
		config,
	)
};

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