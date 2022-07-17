#pragma once

#include <map>
#include <fstream>
#include <vector>
#include <optional>
#include "se_json.hpp"
#include "se_config/se_config.hpp"

enum class indexing_modes {
	LOWER_BOUND = -1,
	FULL,
	SELECTIVE,
	UPPER_BOUND
};

class pi_config : public se_config {
private:
	std::map<std::string, std::string> database;
	SE_CONFIG_REAL_PROPERTY(se_config, std::vector<std::string>, sources, sources, sources, {})
	SE_CONFIG_REAL_PROPERTY(se_config, size_t, indexing_mode, indexing_mode, indexing_mode, -1)
	SE_CONFIG_REAL_PROPERTY(se_config, size_t, power_level, power_level, power_level, -1)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		pi_config, 
		database, 
		sources,
		indexing_mode,
		power_level
	)

	SE_CONFIG_VIRTUAL_PROPERTY(se_config, std::string, db_url, database.at("url"), database["url"])
	SE_CONFIG_VIRTUAL_PROPERTY(se_config, std::string, db_user_name, database.at("user_name"), database["user_name"])
	SE_CONFIG_VIRTUAL_PROPERTY(se_config, std::string, db_password, database.at("password"), database["password"])
	SE_CONFIG_VIRTUAL_PROPERTY(se_config, std::string, db_name, database.at("database_name"), database["database_name"])

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
			throw se_config_exception();
		}
	}

	std::string get_dump() const override {
		nlohmann::json js;
		nlohmann::to_json(js, *this);
		return js.dump(2);
	}
};