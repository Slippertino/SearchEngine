#pragma once

#include "se_text_property.hpp"
#include <nlohmann/json.hpp>

enum class encoding_t {
	UTF_8,
	ANSI,
	UNKNOWN
};

class se_encoding : public se_text_property<encoding_t> {
public:
	se_encoding() = default;
	se_encoding(encoding_t type) : se_text_property(type)
	{ }
	explicit se_encoding(const std::string& value) : se_text_property(value)
	{ }

	se_encoding& operator=(const se_encoding& enc) {
		if (this == &enc) {
			return *this;
		}

		type = enc.type;

		return *this;
	}

	bool operator==(se_encoding enc) const {
		return type == enc.type;
	}

	friend std::ostream& operator<<(std::ostream& ostr, const se_encoding& obj) {
		ostr << obj.to_string();
		return ostr;
	}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(se_encoding, type)
};

const std::unordered_map<std::string, encoding_t> se_encoding::pattern_to_value_interpreter = {
	{".*utf-8.*", encoding_t::UTF_8},
	{".*ansi.*",  encoding_t::ANSI},
};

const std::unordered_map<encoding_t, std::string> se_encoding::value_to_name_interpreter = {
	{encoding_t::UTF_8, "UTF-8"},
	{encoding_t::ANSI,  "ANSI"},
};

const encoding_t se_encoding::default_value = encoding_t::UNKNOWN;

template<>
struct std::hash<se_encoding> {
	std::size_t operator()(const se_encoding& enc) const noexcept {
		return std::hash<size_t>()(static_cast<size_t>(enc.get_type()));
	}
};

template<>
struct std::less<se_encoding> {
	bool operator()(const se_encoding& lenc, const se_encoding& renc) const {
		return static_cast<size_t>(lenc.get_type()) < static_cast<size_t>(renc.get_type());
	}
};