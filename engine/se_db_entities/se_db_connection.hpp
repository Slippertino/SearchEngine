#pragma once

#include <mutex>
#include <memory>
#include "mysql/jdbc.h"

class se_db_connection {
private:
	static std::mutex limiter;

public:
	template<class... Args>
	static auto get_mysql_connection(Args&&... params) {
		std::lock_guard<std::mutex> locker(limiter);
		return std::shared_ptr<sql::Connection>(
			sql::mysql::get_mysql_driver_instance()->connect(std::forward<Args>(params)...)
		);
	}
};

std::mutex se_db_connection::limiter;