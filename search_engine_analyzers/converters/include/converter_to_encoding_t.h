#pragma once

#include "converter_to_property_t.h"

enum class encoding_t {
	UTF_8,
	UNKNOWN
};

class converter_to_encoding_t final: public converter_to_property_t<encoding_t> 
{ };