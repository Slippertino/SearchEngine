#pragma once

#include "../configuration.h"

class se_api
{
public:
	virtual void run(size_t threads_count) = 0;
	virtual void stop() = 0;
	virtual void setup(const configuration& config) = 0;
	virtual void reset() = 0;
	virtual bool status() const = 0;

	virtual ~se_api() = default;
};