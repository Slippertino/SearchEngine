#include <array>
#include <filesystem>
#include "../se_component.h"
#include "se_services_infrastructure/se_router.hpp"
#include "../configuration.h"
#include "../builders/builder.hpp"
#include "../se_logger_api.h"

namespace fs = std::filesystem;

template<typename domain_t, size_t services_count>
class se_domain : public se_component
{
#define SE_DOMAIN(domain_name) friend class builder<domain_name>;
#define ALIAS s
#define EXECUTE_FOR_SERVICES(expression) \
		std::for_each(services.begin(), \
					  services.end(), \
					  [&](auto& ALIAS) { expression; }); \

protected:
	enum class status_flags {
		SETUP = 1,
		RUN   = 2
	};

protected:
	std::array<std::pair<std::shared_ptr<se_component>, size_t>, services_count> services;
	std::shared_ptr<se_router> router;

	size_t working_status = 0;

	std::thread services_control_process;
	std::mutex using_api_limiter;

protected:
	void control_services() {
		do {
			std::this_thread::sleep_for(1000ms);
		} while (is_flag_set(status_flags::RUN) && status());

		std::vector<std::thread> stoppers;
		EXECUTE_FOR_SERVICES(stoppers.push_back(std::thread(&se_component::stop, ALIAS.first.get())))
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

	virtual std::string get_component_name() const override = 0;

public:
	se_domain() = delete;
	se_domain(size_t id, const fs::path& root) :
		se_component(id, root)
	{ }

	bool status() const override {
		try {
			if (is_flag_set(status_flags::SETUP)) {
				bool flag = false;
				EXECUTE_FOR_SERVICES(flag |= ALIAS.first->status())
				SE_LOG("Domain successfully returned " << flag << " as the status!\n");
				return flag;
			} else {
				throw std::exception((typeid(domain_t).name() +
						   std::string(" has not been setup yet")).c_str());
			}
		} catch (const std::exception& ex) {
			SE_LOG("Error while trying to get status! Message : " << ex.what() << "\n");
		}
	}

	void run(size_t threads_count) override {
		try {
			std::lock_guard<std::mutex> locker(using_api_limiter);
			if (is_flag_set(status_flags::SETUP) &&
				!is_flag_set(status_flags::RUN)) {
				size_t total(0);
				EXECUTE_FOR_SERVICES(total += ALIAS.second)
				EXECUTE_FOR_SERVICES(ALIAS.first->run(static_cast<size_t>(threads_count / total) * ALIAS.second))
				set_flag(status_flags::RUN);
				services_control_process = std::thread(&se_domain::control_services, this);
				SE_LOG("Domain was successfully run with " << threads_count << " flows!\n"); \
			} else {
				throw std::exception((typeid(domain_t).name() +
						   std::string(" has not benn setup or has been run yet")).c_str());
			}
		} catch (const std::exception& ex) {
			SE_LOG("Error while trying to run domain with " << threads_count << " flows! Message : " << ex.what() << "\n");
		}
	}

	void stop() override {
		try {
			std::lock_guard<std::mutex> locker(using_api_limiter);
			if (is_flag_set(status_flags::RUN)) {
				remove_flag(status_flags::RUN);
				services_control_process.join();
				SE_LOG("Domain was successfully stopped!\n");
			} else {
				throw std::exception((typeid(domain_t).name() +
						   std::string(" has not been run yet")).c_str());
			}
		} catch (const std::exception& ex) {
			SE_LOG("Error while trying to stop! " << "Message : " << ex.what() << "\n");
		}
	}

	void setup(const configuration& config) override {
		try {
			std::lock_guard<std::mutex> locker(using_api_limiter);
			if (!is_flag_set(status_flags::SETUP)) {
				EXECUTE_FOR_SERVICES(ALIAS.first->setup(config))
				set_flag(status_flags::SETUP);
				SE_LOG("Domain was successfully setup!\n");
			} else {
				throw std::exception((typeid(domain_t).name() +
					       std::string(" has already been setup")).c_str());
			}
		} catch (const std::exception& ex) {
			SE_LOG("Error while trying to setup! " << "Message : " << ex.what() << "\n");
		}
	}

	void reset() override {
		try {
			std::lock_guard<std::mutex> locker(using_api_limiter);
			if (is_flag_set(status_flags::SETUP)) {
				remove_all_flags();
				if (services_control_process.joinable())
					services_control_process.join();
				EXECUTE_FOR_SERVICES(ALIAS.first->reset())
				SE_LOG("Domain was successfully reset!\n");
			} else {
				throw std::exception((typeid(domain_t).name() +
					std::string(" has not benn setup yet")).c_str());
			}
		} catch (const std::exception& ex) {
			SE_LOG("Error while trying to reset! " << "Message : " << ex.what() << "\n");
		}
	}

	virtual ~se_domain() {
		for (auto& s : services)
			s.first->reset();
	}
};