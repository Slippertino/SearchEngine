#pragma once

#include "se_config.hpp"

class se_service_behaviour {
public:
	virtual void run(size_t threads_count) = 0;
	virtual void stop() = 0;
	virtual void setup(const std::shared_ptr<se_config>& config) = 0;
	virtual void reset() = 0;
	virtual bool status() const = 0;
};