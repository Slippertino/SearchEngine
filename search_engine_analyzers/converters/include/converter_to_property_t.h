#pragma once

#include <string>
#include <map>
#include <regex>
#include <algorithm>

template<class TTextProperty>
class converter_to_property_t
{
protected:
	static const std::map<std::string, TTextProperty> pattern_to_value_interpreter;
	static const TTextProperty default_value;

protected:
	void to_lower(std::string& text) {
		std::transform(text.begin(), text.end(), text.begin(), ::tolower);
	}

public:
	converter_to_property_t() = default;

	TTextProperty get_value(std::string text) {
		to_lower(text);

		for (const auto& p : pattern_to_value_interpreter) {
			std::regex pattern(p.first);
			if (std::regex_match(text, pattern))
				return p.second;
		}

		return default_value;
	}

	virtual ~converter_to_property_t() = default;
};