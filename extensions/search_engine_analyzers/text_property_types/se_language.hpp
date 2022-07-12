#pragma once

#include "se_text_property.hpp"
#include <nlohmann/json.hpp>

enum class language_t 
{
	ENGLISH,
	RUSSIAN,
	UNKNOWN
};

class se_language : public se_text_property<language_t>
{
public:
	se_language() = default;
	se_language(language_t type) : se_text_property(type)
	{ }
	explicit se_language(const std::string& value) : se_text_property(value)
	{ }

	se_language& operator=(const se_language& lang) {
		if (this == &lang)
			return *this;

		type = lang.type;

		return *this;
	}

	bool operator==(se_language enc) const {
		return type == enc.type;
	}

	friend std::ostream& operator<<(std::ostream& ostr, const se_language& obj) {
		ostr << obj.to_string();
		return ostr;
	}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(se_language, type)
};

const std::unordered_map<std::string, language_t> se_language::pattern_to_value_interpreter = {
	{".*en.*", language_t::ENGLISH},
	{".*ru.*", language_t::RUSSIAN},
};

const std::unordered_map<language_t, std::string> se_language::value_to_name_interpreter = {
	{language_t::ENGLISH, "english"},
	{language_t::RUSSIAN, "russian"},
};

const language_t se_language::default_value = language_t::UNKNOWN;

template<>
struct std::hash<se_language> {
	std::size_t operator()(const se_language& lang) const noexcept {
		return std::hash<size_t>()(static_cast<size_t>(lang.get_type()));
	}
};

template<>
struct std::less<se_language> {
	bool operator()(const se_language& llang, const se_language& rlang) const {
		return static_cast<size_t>(llang.get_type()) < static_cast<size_t>(rlang.get_type());
	}
};