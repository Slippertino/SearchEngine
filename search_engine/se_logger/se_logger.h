#pragma once

#include <filesystem>
#include <thread_safe_containers/thread_safe_unordered_map.hpp>
#include <fstream>
#include "se_logger_file.h"

namespace fs = std::filesystem;

class se_logger
{
private:
	std::unordered_map<size_t, std::shared_ptr<se_logger_file>> code_file_interpreter;
	fs::path root;
	std::string files_extension;

public:
	se_logger() = delete;
	se_logger(const fs::path& rt, const std::string& extension) : root(rt), 
																  files_extension(extension) {
		fs::create_directories(root);
	}

	template<class T>
	static size_t get_code(T obj) {
		return std::hash<T>()(obj);
	}

	template<class T, class... Args>
	static size_t get_code(T obj, Args&&... args) {
		return std::hash<T>()(std::forward<T>(obj)) * get_code(std::forward<Args>(args)...);
	}

	void add_file(size_t code,
				  const std::string& name,
				  const fs::path& path) {
		fs::create_directories(path);

		code_file_interpreter.insert({ 
				code,
				std::make_shared<se_logger_file>(path / fs::path(name + files_extension))
		});
	}

	void record(size_t code, const std::string& msg) {
		try {
			auto f_ptr = code_file_interpreter.at(code);
			f_ptr->record(msg);
		} catch (const std::exception& ex) {
			std::cout << ex.what() << std::endl;
			throw std::exception("file does not exist");
		}
	}
};

#define LOG_BASE(log_msg, ...) \
	se_loggers_storage::get_instance()->get_logger(id)->record( \
		se_logger::get_code(__VA_ARGS__), \
		(std::ostringstream() << log_msg).str())