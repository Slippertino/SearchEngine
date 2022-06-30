#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <text_property_types/se_encoding.hpp>

struct string_enc {
	std::string str;
	se_encoding enc;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(string_enc, str, enc)
};

std::ostream& operator<<(std::ostream& ostr, const string_enc& obj) {
	ostr << "{ " << obj.str << " ; " << obj.enc << " }";
	return ostr;
}

template<>
struct std::equal_to<string_enc> {
	bool operator()(const string_enc& lobj, const string_enc& robj) const {
		return (lobj.str == robj.str) && (lobj.enc == robj.enc);
	}
};

template<>
struct std::hash<string_enc> {
	std::size_t operator()(const string_enc& obj) const noexcept {
		auto h1 = std::hash<std::string>()(obj.str);
		auto h2 = std::hash<se_encoding>()(obj.enc);

		if (h1 != h2) {
			return h1 ^ h2;
		}

		return h1;
	}
};