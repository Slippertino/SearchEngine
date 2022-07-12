#pragma once

#include <filesystem>
#include <sstream>
#include <string>
#include <iomanip>
#include <ctime>
#include "se_logger/se_loggers_storage.hpp"
#include "messages/message_core.hpp"

namespace fs = std::filesystem;

#define SE_LOG_PROCESS(log_msg) log(ostringstream() << log_msg); 
#define SE_LOG_MESSAGE(msg_type, log_msg) log(ostringstream() << log_msg, std::to_string(static_cast<size_t>(msg_type)))
#define GET_MACRO(_1, _2, _NAME, ...) _NAME
#define MACRO_RECOMPOSER(argsWithParentheses) GET_MACRO argsWithParentheses
#define SE_LOG(...) MACRO_RECOMPOSER((__VA_ARGS__, SE_LOG_MESSAGE, SE_LOG_PROCESS)(__VA_ARGS__))

class se_logable_component {
protected:
	size_t id;
	fs::path logger_path;

public:
	se_logable_component() = delete;
	se_logable_component(size_t id, const fs::path& path) : id(id), logger_path(path)
	{ }

	auto get_id() const {
		return id;
	}

	auto get_path() const {
		return logger_path;
	}

	auto get_full_name() const {
		std::ostringstream ostr;
		auto time = std::time(nullptr);
		ostr << get_component_name() << "_" << std::put_time(std::localtime(&time), "%Y.%m.%d %H.%M.%S");
		return ostr.str();
	}

	template<class... Args>
	void log(std::ostringstream&& ostr, Args&&... params) const {
		se_loggers_storage::get_instance()
			->get_logger(id)
			->record(se_logger::get_code(get_full_name(), std::forward<Args>(params)...), std::move(ostr.str));
	}

	virtual std::string get_component_name() const = 0;

	virtual ~se_logable_component() = default;
};