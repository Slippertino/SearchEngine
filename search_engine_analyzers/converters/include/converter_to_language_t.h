#pragma once

#include "converter_to_property_t.h"

enum class language_t {
	ENGLISH,
	RUSSIAN,
	UNKNOWN
};

class converter_to_language_t final: public converter_to_property_t<language_t>
{ };