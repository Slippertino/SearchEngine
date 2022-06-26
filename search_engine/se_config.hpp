#pragma once

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class se_config {
public:
	virtual void load(const fs::path& path) = 0;
	virtual std::string get_dump() const = 0;
};