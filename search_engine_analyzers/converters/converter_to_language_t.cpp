#include "include/converter_to_language_t.h"

const std::map<std::string, language_t> converter_to_language_t::pattern_to_value_interpreter = {
	{".*en.*", language_t::ENGLISH},
	{".*ru.*", language_t::RUSSIAN},
};

const std::map<language_t, std::string> converter_to_language_t::value_to_name_interpreter = {
	{language_t::ENGLISH, "ENGLISH"},
	{language_t::RUSSIAN, "RUSSIAN"},
};

const language_t converter_to_language_t::default_value = language_t::UNKNOWN;