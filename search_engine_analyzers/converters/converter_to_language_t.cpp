#include "include/converter_to_language_t.h"

const std::map<std::string, language_t> converter_to_language_t::pattern_to_value_interpreter = {
	{".*en.*", language_t::ENGLISH},
	{".*ru.*", language_t::RUSSIAN},
};

const language_t converter_to_language_t::default_value = language_t::UNKNOWN;