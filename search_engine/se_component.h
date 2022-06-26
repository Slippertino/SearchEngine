#pragma once

#include <filesystem>
#include "se_config.hpp"

namespace fs = std::filesystem;

class se_component {
protected:
	size_t id;
	fs::path logger_path;

public:
	se_component() = delete;
	se_component(size_t id, const fs::path& path) : id(id), 
											        logger_path(path)
	{ }

	auto get_id() const {
		return id;
	}

	auto get_path() const {
		return logger_path;
	}

	auto get_full_name(const std::string& name) const {
		std::ostringstream ostr;
		auto time = std::time(nullptr);
		ostr << name << "_" << std::put_time(std::localtime(&time), "%Y.%m.%d %H.%M.%S");
		return ostr.str();
	}

	virtual std::string get_component_name() const = 0;

	virtual void run(size_t threads_count) = 0;
	virtual void stop() = 0;
	virtual void setup(const std::shared_ptr<se_config>& config) = 0;
	virtual void reset() = 0;
	virtual bool status() const = 0;

	virtual ~se_component() = default;
};