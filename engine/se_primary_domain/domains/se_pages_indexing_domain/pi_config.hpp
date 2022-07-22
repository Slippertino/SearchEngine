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
	using db_info_t = std::map<std::string, std::string>;

	SE_CFG_PROPERTY(
		std::vector<std::string>, sources, {},
		SE_PROPERTY_GET(std::vector<std::string>, sources, sources),
		SE_PROPERTY_SET(sources, src, std::initializer_list<std::string>, sources = src)
	)

	SE_CFG_PROPERTY(
		size_t, power_level, -1,
		SE_PROPERTY_GET(size_t, power_level, power_level),
		SE_PROPERTY_SET(power_level, p_l, size_t, power_level = p_l)
	)

	SE_CFG_PROPERTY(
		size_t, indexing_mode, -1,
		SE_PROPERTY_GET(indexing_modes, indexing_mode, static_cast<indexing_modes>(indexing_mode)),
		SE_PROPERTY_SET(indexing_mode, ind_mode, indexing_modes, indexing_mode = static_cast<size_t>(ind_mode))
	)

	SE_CFG_PROPERTY(db_info_t, database, { {} })

	SE_PROPERTY_GET(std::string, db_url, database.at("url"))
	SE_PROPERTY_SET(db_url, in_db_url, std::string, database["url"] = in_db_url)

	SE_PROPERTY_GET(std::string, db_user_name, database.at("user_name"))
	SE_PROPERTY_SET(db_user_name, in_db_user_name, std::string, database["user_name"] = in_db_user_name)

	SE_PROPERTY_GET(std::string, db_password, database.at("password"))
	SE_PROPERTY_SET(db_password, in_db_password, std::string, database["password"] = in_db_password)

	SE_PROPERTY_GET(std::string, db_name, database.at("database_name"))
	SE_PROPERTY_SET(db_name, in_db_name, std::string, database["database_name"] = in_db_name)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		pi_config,
		database,
		sources,
		indexing_mode,
		power_level
	)

public:
	pi_config() = delete;
	pi_config(se_encoding enc) : se_config(enc) 
	{ }

	pi_config(const pi_config& config) = default;
	pi_config& operator=(const pi_config& config) = default;

	void load(const fs::path& path) override {
		std::ifstream istr(path);

		nlohmann::json configure_file;

		try {
			istr >> configure_file;

			database = configure_file.at("database");

			for (auto& cur : configure_file.at("sources")) {
				sources.push_back(cur);
			}
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