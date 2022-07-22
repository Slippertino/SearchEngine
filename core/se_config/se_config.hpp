#pragma once

#include <string>
#include <filesystem>
#include <optional>
#include "se_json.hpp"
#include <search_engine_analyzers/text_property_types/se_encoding.hpp>

namespace fs = std::filesystem;

class se_config {
protected:
	se_encoding encoding;

public:
	se_config() = delete;
	se_config(se_encoding enc) : encoding(enc) 
	{ }

	se_encoding get_encoding() const {
		return encoding;
	}

	virtual void load(const fs::path& path) = 0;
	virtual std::string get_dump() const = 0;
};

class se_config_exception : public std::exception {
public:
	const char* what() const noexcept override {
		return "Error while reading the configuration file!";
	}
};

#define SE_CFG_PROPERTY(type, name, default_value, GET, SET) \
private: \
	type name = default_value; \
public: \
	GET \
	SET \

#define SE_PROPERTY_GET(type, name, expression) \
	type get_##name() const { \
		try { \
			return expression; \
		} catch (...) { \
			throw se_config_exception(); \
		} \
	} \

#define SE_PROPERTY_SET(name, alias, alias_type, expression) \
	void set_##name(const alias_type##& alias) { \
		expression; \
	} 
