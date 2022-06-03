#include <array>
#include "se_api.h"
#include "se_services_infrastructure/se_router.hpp"
#include "../configuration.h"
#include "builders/builder.hpp"

template<typename domain_t, size_t services_count>
class se_domain : public se_api 
{
#define SE_DOMAIN(domain_name) friend class builder<domain_name>;
#define NAME s
#define EXECUTE_FOR_SERVICES(expression) \
		std::for_each(services.begin(), \
					  services.end(), \
					  [&](auto& NAME) { expression; }); \

protected:
	enum class status_flags {
		SETUP = 1,
		RUN   = 2
	};

protected:
	std::array<std::pair<std::shared_ptr<se_api>, size_t>, services_count> services;
	std::shared_ptr<se_router> router;

	size_t working_status = 0;

	std::thread services_control_process;

protected:
	void control_services() {
		do {
			std::this_thread::sleep_for(1000ms);
		} while (is_flag_set(status_flags::RUN) && status());

		std::vector<std::thread> stoppers;
		EXECUTE_FOR_SERVICES(stoppers.push_back(std::thread(&se_api::stop, NAME.first.get())))
		for (auto& cur : stoppers)
			cur.join();
	}

	bool is_flag_set(status_flags flag) const {
		return working_status & static_cast<size_t>(flag);;
	}

	void set_flag(status_flags flag) {
		if (!is_flag_set(flag))
			working_status |= static_cast<size_t>(flag);;
	}

	void remove_flag(status_flags flag) {
		if (is_flag_set(flag))
			working_status &= ~static_cast<size_t>(flag);
	}

	void remove_all_flags() {
		working_status = 0;
	}

public:
	se_domain() = default;

	bool status() const override {
		if (is_flag_set(status_flags::SETUP)) {
			bool flag = false;
			EXECUTE_FOR_SERVICES(flag |= NAME.first->status())
			return flag;
		} else {
			throw std::exception("domain was not setup");
		}
	}

	void run(size_t threads_count) override {
		if (is_flag_set(status_flags::SETUP) && 
		    !is_flag_set(status_flags::RUN)) {
			size_t total(0);
			EXECUTE_FOR_SERVICES(total += NAME.second)
			EXECUTE_FOR_SERVICES(NAME.first->run(static_cast<size_t>(threads_count / total) * NAME.second))
			set_flag(status_flags::RUN);
			services_control_process = std::thread(&se_domain::control_services, this);
		} else {
			throw std::exception("domain was not setup");
		}
	}

	void stop() override {
		if (is_flag_set(status_flags::RUN)) {
			remove_flag(status_flags::RUN);
			services_control_process.join();
		} else {
			throw std::exception("domain was not run");
		}
	}

	void setup(const configuration& config) override {
		EXECUTE_FOR_SERVICES(NAME.first->setup(config))
		set_flag(status_flags::SETUP);
	}

	void reset() override {
		if (is_flag_set(status_flags::SETUP)) {
			remove_all_flags();
			if (services_control_process.joinable())
				services_control_process.join();
			EXECUTE_FOR_SERVICES(NAME.first->reset())
		} else {
			throw std::exception("domain was not setup");
		}
	}
};