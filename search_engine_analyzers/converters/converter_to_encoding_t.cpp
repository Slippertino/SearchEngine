#include "include/converter_to_encoding_t.h"

const std::map<std::string, encoding_t> converter_to_encoding_t::pattern_to_value_interpreter = {
	{".*utf-8.*", encoding_t::UTF_8},
};

const encoding_t converter_to_encoding_t::default_value = encoding_t::UNKNOWN;