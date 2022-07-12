#pragma once

#include <string>
#include <filesystem>
#include <optional>
#include "se_json.hpp"

namespace fs = std::filesystem;

class se_config {
public:
	virtual void load(const fs::path& path) = 0;
	virtual std::string get_dump() const = 0;
};

class se_config_exception : public std::exception {
public:
	const char* what() const noexcept override {
		return "Error while reading the configuration file!";
	}
};


#define SE_CONFIG_PROPERTY(config_type, property_type, property_name, get_expression, set_expression) \
public: \
	property_type get_##property_name() const { \
		try { \
			return get_expression; \
		} catch (...) { \
			throw se_config_exception(); \
		} \
	} \
	config_type& set_##property_name(const property_type##& in_##property_name) { \
		set_expression = in_##property_name; \
		return *this; \
	} 

#define SE_CONFIG_REAL_PROPERTY(config_type, property_type, property_name, get_expression, set_expression) \
private: \
	property_type property_name; \
	SE_CONFIG_PROPERTY(config_type, property_type, property_name, get_expression, set_expression) \

#define SE_CONFIG_VIRTUAL_PROPERTY(config_type, property_type, property_name, get_expression, set_expression) \
	SE_CONFIG_PROPERTY(config_type, property_type, property_name, get_expression, set_expression)
