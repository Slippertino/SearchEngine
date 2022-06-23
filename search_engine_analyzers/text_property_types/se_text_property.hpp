#pragma once

#include <string>
#include <algorithm>
#include <unordered_map>
#include <regex>

template<typename TTextProperty>
class se_text_property {
protected:
	static const std::unordered_map<std::string, TTextProperty> pattern_to_value_interpreter;
	static const std::unordered_map<TTextProperty, std::string> value_to_name_interpreter;
	static const TTextProperty default_value;

protected:
	TTextProperty type = default_value;

protected:
	void to_lower(std::string& text) {
		std::transform(text.begin(), text.end(), text.begin(), ::tolower);
	}

	TTextProperty convert_to_property_type(std::string text) {
		to_lower(text);

		for (const auto& p : pattern_to_value_interpreter) {
			std::regex pattern(p.first);
			if (std::regex_match(text, pattern)) {
				return p.second;
			}
		}

		return default_value;
	}

public:
	se_text_property() = default;
	se_text_property(TTextProperty in_type) : type(in_type)
	{ }
	explicit se_text_property(const std::string& value) : type(convert_to_property_type(value))
	{ }

	se_text_property& operator= (const se_text_property& prop) {
		if (this == &prop) {
			return *this;
		}

		type = prop.type;

		return *this;
	}

	TTextProperty get_type() const {
		return type;
	}

	std::string to_string() const {
		return value_to_name_interpreter.at(type);
	}

	virtual ~se_text_property() = default;
};

template<typename TTextProperty>
const std::unordered_map<std::string, TTextProperty> se_text_property<TTextProperty>::pattern_to_value_interpreter;

template<typename TTextProperty>
const std::unordered_map<TTextProperty, std::string> se_text_property<TTextProperty>::value_to_name_interpreter;

template<typename TTextProperty>
const TTextProperty se_text_property<TTextProperty>::default_value;