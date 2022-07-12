#pragma once

#include "se_config.hpp"

class se_domain_behaviour {
protected:
	enum class status_flags {
		SETUP = 1,
		RUN = 2
	};

protected:
	bool is_flag_set(size_t status, status_flags flag) const {
		return working_status & static_cast<size_t>(flag);;
	}

	void set_flag(size_t& status, status_flags flag) {
		if (!is_flag_set(flag)) {
			status |= static_cast<size_t>(flag);
		}
	}

	void remove_flag(size_t& status, status_flags flag) {
		if (is_flag_set(flag)) {
			status &= ~static_cast<size_t>(flag);
		}
	}

	void remove_all_flags(size_t& status) {
		status = 0;
	}

protected:
	size_t working_status = 0;

public:
	virtual void run() = 0;
	virtual void stop() = 0;
	virtual void setup(const std::shared_ptr<se_config>& config) = 0;
	virtual void reset() = 0;
	virtual size_t status() const = 0;
};