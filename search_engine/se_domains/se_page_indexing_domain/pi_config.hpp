#pragma once

#include <map>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include "../../se_config.hpp"

class pi_config : public se_config {
private:
	std::map<std::string, std::string> database;
	std::vector<std::string> sources;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(pi_config, database, sources)

public:
	pi_config() = default;
	pi_config(const pi_config& config) = default;
	pi_config& operator=(const pi_config& config) = default;

	void load(const fs::path& path) override {
		std::ifstream istr(path);

		nlohmann::json configure_file;

		try {
			istr >> configure_file;

			database = configure_file.at("database");

			for (auto& cur : configure_file.at("sources"))
				sources.push_back(cur);
		} catch (const std::exception& ex) {
			throw std::exception("Error while reading the configuration file!");
		}
	}

	std::string get_db_url() const {
		try {
			return database.at("url");
		} catch (...) {
			throw std::exception("Error while reading the configuration file!");
		}
	}

	pi_config& set_db_url(const std::string& url) {
		database["url"] = url;
		return *this;
	}

	std::string get_db_user_name() const {
		try {
			return database.at("user_name");
		} catch (...) {
			throw std::exception("Error while reading the configuration file!");
		}
	}

	pi_config& set_db_user_name(const std::string& name) {
		database["user_name"] = name;
		return *this;
	}

	std::string get_db_password() const {
		try {
			return database.at("password");
		} catch (...) {
			throw std::exception("Error while reading the configuration file!");
		}
	}

	pi_config& set_db_password(const std::string& pass) {
		database["password"] = pass;
		return *this;
	}

	std::string get_db_name() const {
		try {
			return database.at("database_name");
		} catch (...) {
			throw std::exception("Error while reading the configuration file!");
		}
	}

	pi_config& set_db_name(const std::string& name) {
		database["database_name"] = name;
		return *this;
	}

	std::vector<std::string> get_sources() const {
		try {
			return sources;
		} catch (...) {
			throw std::exception("Error while reading the configuration file!");
		}
	}

	pi_config& set_db_sources(std::initializer_list<std::string>&& list) {
		sources = std::move(list);
		return *this;
	}

	std::string get_dump() const override {
		nlohmann::json js;
		nlohmann::to_json(js, *this);
		return js.dump();
	}
};