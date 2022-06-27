#pragma once

#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include <text_property_types/se_encoding.hpp>
#include "../../se_config.hpp"

class s_config : public se_config {
private:
	std::map<std::string, std::string> database;
	std::string query;
	se_encoding encoding;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(s_config, database, query)

public:
	s_config() = default;
	s_config(const s_config& config) = default;
	s_config& operator=(const s_config& config) = default;

	void load(const fs::path& path) override {
		std::ifstream istr(path);

		nlohmann::json configure_file;

		try {
			istr >> configure_file;

			database = configure_file.at("database");
			query = configure_file.at("query");
			encoding = se_encoding(std::string(configure_file.at("encoding")));
		} catch (const std::exception& ex) {
			throw se_config_exception();
		}
	}

	std::string get_db_url() const {
		try {
			return database.at("url");
		} catch (...) {
			throw se_config_exception();
		}
	}

	s_config& set_db_url(const std::string& url) {
		database["url"] = url;
		return *this;
	}

	std::string get_db_user_name() const {
		try {
			return database.at("user_name");
		} catch (...) {
			throw se_config_exception();
		}
	}

	s_config& set_db_user_name(const std::string& name) {
		database["user_name"] = name;
		return *this;
	}

	std::string get_db_password() const {
		try {
			return database.at("password");
		} catch (...) {
			throw se_config_exception();
		}
	}

	s_config& set_db_password(const std::string& pass) {
		database["password"] = pass;
		return *this;
	}

	std::string get_db_name() const {
		try {
			return database.at("database_name");
		} catch (...) {
			throw se_config_exception();
		}
	}

	s_config& set_db_name(const std::string& name) {
		database["database_name"] = name;
		return *this;
	}

	std::string get_query() const {
		try {
			return query;
		} catch (...) {
			throw se_config_exception();
		}
	}

	s_config& set_query(const std::string& qr) {
		query = qr;
		return *this;
	}

	se_encoding get_encoding() const {
		try {
			return encoding;
		} catch (...) {
			throw se_config_exception();
		}
	}

	s_config& set_encoding(se_encoding enc) {
		encoding = enc;
		return *this;
	}

	std::string get_dump() const override {
		nlohmann::json js;
		nlohmann::to_json(js, *this);
		return js.dump();
	}
};