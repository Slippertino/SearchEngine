#pragma once

#include <map>
#include <fstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

class configuration
{
private:
	struct configure_struct
	{
		std::map<std::string, std::string> database;
		std::vector<std::string> sources;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(configure_struct, database, sources)
	};

private:
	configure_struct configure_info;
	bool is_filled = false;

public:
	configuration() = default;
	configuration(const configuration& config) = default;
	configuration& operator=(const configuration& config) = default;

	void load(const std::string& path)
	{
		std::ifstream istr(path);

		nlohmann::json configure_file;

		try {
			istr >> configure_file;

			configure_info.database = configure_file.at("database");

			for (auto& cur : configure_file.at("sources"))
				configure_info.sources.push_back(cur);

			is_filled = true;
		}
		catch (...)
		{
			throw std::exception("Error while reading the configuration file!");
		}
	}

	std::string get_db_url() const {
		if (!is_filled)
			return std::string();

		try {
			return configure_info.database.at("url");
		}
		catch (...) {
			throw std::exception("Error while reading the configuration file!");
		}
	}

	configuration& set_db_url(const std::string& url) {
		configure_info.database["url"] = url;
		return *this;
	}

	std::string get_db_user_name() const {
		if (!is_filled)
			return std::string();

		try {
			return configure_info.database.at("user_name");
		}
		catch (...) {
			throw std::exception("Error while reading the configuration file!");
		}
	}

	configuration& set_db_user_name(const std::string& name) {
		configure_info.database["user_name"] = name;
		return *this;
	}

	std::string get_db_password() const {
		if (!is_filled)
			return std::string();

		try {
			return configure_info.database.at("password");
		}
		catch (...) {
			throw std::exception("Error while reading the configuration file!");
		}
	}

	configuration& set_db_password(const std::string& pass) {
		configure_info.database["password"] = pass;
		return *this;
	}

	std::string get_db_name() const {
		if (!is_filled)
			return std::string();

		try {
			return configure_info.database.at("database_name");
		}
		catch (...) {
			throw std::exception("Error while reading the configuration file!");
		}
	}

	configuration& set_db_name(const std::string& name) {
		configure_info.database["database_name"] = name;
		return *this;
	}

	std::vector<std::string> get_sources() const {
		if (!is_filled)
			return {};

		try {
			return configure_info.sources;
		}
		catch (...) {
			throw std::exception("Error while reading the configuration file!");
		}
	}

	configuration& set_db_sources(std::initializer_list<std::string>&& list) {
		configure_info.sources = std::move(list);
		return *this;
	}

	std::string get_dump() const {
		nlohmann::json js;
		nlohmann::to_json(js, configure_info);
		return js.dump();
	}
};