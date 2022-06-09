#pragma once

#include <thread_safe_containers/thread_safe_unordered_map.hpp>
#include "se_logger.h"

class se_loggers_storage
{
private:
	static std::shared_ptr<se_loggers_storage> storage_instance;

	thread_safe_unordered_map<size_t, std::shared_ptr<se_logger>> loggers_storage;
	
private:
	se_loggers_storage() = default;

public:
	static std::shared_ptr<se_loggers_storage> get_instance() {
		if (!storage_instance)
			storage_instance = std::shared_ptr<se_loggers_storage>(new se_loggers_storage());

		return storage_instance;
	}

	std::shared_ptr<se_logger> get_logger(size_t id) {
		if (loggers_storage.find(id) != loggers_storage.end())
			return loggers_storage.wait_and_get(id);
		else
			throw std::exception("unknown id");
	}

	void set_logger(size_t id, const std::shared_ptr<se_logger>& logger) {
		if (loggers_storage.find(id) == loggers_storage.end())
			return loggers_storage.insert(id, logger);
		else
			throw std::exception("logger has already been registered");
	}
};

std::shared_ptr<se_loggers_storage> se_loggers_storage::storage_instance = nullptr;