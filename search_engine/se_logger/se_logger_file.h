#pragma once

#include <filesystem>
#include <mutex>
#include <sstream>
#include <fstream>

namespace fs = std::filesystem;

class se_logger_file {
private:
	std::ofstream file;
	std::mutex record_locker;

private:
	std::string get_log_prefix() const {
		std::ostringstream ostr;
		auto time = std::time(nullptr);
		ostr << "[" << std::put_time(std::localtime(&time), "%Y.%m.%d %H:%M:%S") << "] : ";
		return ostr.str();
	}

public:
	se_logger_file() = delete;
	se_logger_file(const fs::path& path) : file(path) {
		file.clear();
	}

	void record(const std::string& msg) {
		std::lock_guard<std::mutex> locker(record_locker);
		file << get_log_prefix() << msg;
	}

	~se_logger_file() {
		file.close();
	}
};