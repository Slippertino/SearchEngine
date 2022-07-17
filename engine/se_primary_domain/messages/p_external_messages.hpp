#pragma once

#include <core/messages/message_core.hpp>
#include <search_engine_analyzers/text_property_types/string_enc.hpp>

struct start_indexing_request : context {
	string_enc name;

	SE_CONTEXT(
		start_indexing_request, 2,
		name,
		)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		start_indexing_request,
		name,
	)
};