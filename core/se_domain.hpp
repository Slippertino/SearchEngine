#pragma once

#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <memory>
#include "se_work_station.hpp"
#include "se_service_behaviour.hpp"
#include "se_domain_behaviour.hpp"
#include "se_config/validators/se_cfg_validator.hpp"
#include "se_config/validators/se_cfg_validator_imp.hpp"

template<class domain_t, class config_t>
class se_domain : public se_work_station<domain_t, config_t>,
				  public se_domain_behaviour {
protected:
	#define SE_DOMAIN(type_name, cfg_type_name, component_name) \
		SE_WORK_STATION(type_name, se_domain, component_name) \
		private: \
			friend class se_cfg_validator_imp<type_name, cfg_type_name>; \

	#define ALIAS s
	#define EXECUTE_FOR_SERVICES(expression) \
		std::for_each(services.begin(), \
					  services.end(), \
					  [&](auto& ALIAS) { expression; }); \

protected:
	static const std::shared_ptr<se_cfg_validator<domain_t, config_t>> cfg_validator;
	static const size_t default_power_level;

protected:
	size_t reserved_power_level = default_power_level;
	std::unordered_map<size_t, size_t> levels_power;

	std::vector<std::pair<std::shared_ptr<se_service_behaviour>, size_t>> services;

	std::shared_ptr<config_type> base_config;


	std::thread services_control_process;
	std::mutex if_locker;

protected:
	size_t get_services_power_value() const {
		size_t total(0);
		EXECUTE_FOR_SERVICES(total += ALIAS.second)
		return total;
	}

	bool get_services_status() const {
		bool flag = false;
		EXECUTE_FOR_SERVICES(flag |= ALIAS.first->status())
		return flag;
	}

	void run_services() {
		size_t total = get_services_power_value();
		EXECUTE_FOR_SERVICES(ALIAS.first->run(static_cast<size_t>(levels_power[reserved_power_level] / total) * ALIAS.second))

		if (services_control_process.joinable()) {
			services_control_process.join();
		}

		services_control_process = std::thread(&station_type::control_services, get_object());
	}

	void stop_services() {
		std::vector<std::thread> stoppers;
		EXECUTE_FOR_SERVICES(stoppers.push_back(std::thread(&se_service_behaviour::stop, ALIAS.first.get())))
		for (auto& cur : stoppers) {
			cur.join();
		}
	}

	void setup_services() {
		EXECUTE_FOR_SERVICES(ALIAS.first->setup(base_config))
	}

	void reset_services() {
		EXECUTE_FOR_SERVICES(ALIAS.first->reset())
	}

	void disturbe_processes() {
		stop_flag = false;
		pool = thread_pool(2);

		pool.add_task(&station_type::requests_handler,  get_object());
		pool.add_task(&station_type::responses_handler, get_object());
	}

	void control_services() {
		pool.set_active(std::this_thread::get_id());

		do {
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		} while (is_flag_set(status_flags::RUN) && get_services_status());

		stop_services();

		pool.set_inactive(std::this_thread::get_id());
	}

	virtual void clear() override = 0 {
		se_work_station<domain_t, config_t>::clear();
		remove_all_flags();
		base_config.reset();
	}

	virtual station_type* get_object() const override = 0;

public:
	se_domain() = delete;
	se_domain(size_t id, const fs::path& path, const std::shared_ptr<se_switch_board>& in_switch) 
		: se_work_station<domain_t, config_t>(id, path, in_switch)
	{ }

	virtual size_t status() const override {
		try {
			if (is_flag_set(working_status, status_flags::SETUP)) {
				SE_LOG("Domain successfully returned " << working_status << " as the status!\n");
				return working_status;
			} else {
				throw std::exception((typeid(domain_t).name() +
					std::string(" has not been setup yet")).c_str());
			}
		} catch (const std::exception& ex) {
			SE_LOG("Error while trying to get status! Message : " << ex.what() << "\n");
		}
	}

	virtual void run() override {
		try {
			std::lock_guard<std::mutex> locker(if_locker);
			if (is_flag_set(working_status, status_flags::SETUP) &&
				!is_flag_set(working_status, status_flags::RUN)) {

				disturbe_processes();

				set_flag(working_status, status_flags::RUN);

				SE_LOG("Domain was successfully run!\n");
			} else {
				throw std::exception((typeid(domain_t).name() +
					std::string(" has not benn setup or has been run yet")).c_str());
			}
		} catch (const std::exception& ex) {
			SE_LOG("Error while trying to run domain! Message : " << ex.what() << "\n");
		}
	}

	virtual void stop() override {
		try {
			std::lock_guard<std::mutex> locker(if_locker);
			if (is_flag_set(working_status, status_flags::RUN)) {

				remove_flag(working_status, status_flags::RUN);

				if (services_control_process.joinable()) {
					services_control_process.join();
				}

				pool.clear();

				SE_LOG("Domain was successfully stopped!\n");
			} else {
				throw std::exception((typeid(domain_t).name() +
					std::string(" has not been run yet")).c_str());
			}
		} catch (const std::exception& ex) {
			SE_LOG("Error while trying to stop! " << "Message : " << ex.what() << "\n");
		}
	}

	virtual void setup(const std::shared_ptr<se_config>& config) override {
		try {
			std::lock_guard<std::mutex> locker(if_locker);

			base_config.reset();

			auto* cfg = dynamic_cast<config_type*>(config.get());

			if (!is_flag_set(working_status, status_flags::SETUP) && cfg) {

				base_config = std::shared_ptr<config_type>(cfg);

				set_flag(working_status, status_flags::SETUP);

				SE_LOG("Domain was successfully setup!\n");
			} else {
				throw std::exception((typeid(domain_t).name() +
					  std::string(" has already been setup")).c_str());
			}
		} catch (const std::exception& ex) {
			SE_LOG("Error while trying to setup! " << "Message : " << ex.what() << "\n");
		}
	}

	virtual void reset() override {
		try {
			std::lock_guard<std::mutex> locker(if_locker);
			if (is_flag_set(working_status, status_flags::SETUP)) {

				remove_all_flags(working_status);

				if (services_control_process.joinable()) {
					services_control_process.join();
				}

				get_object()->clear();

				reset_services();

				SE_LOG("Domain was successfully reset!\n");
			} else {
				throw std::exception((typeid(domain_t).name() +
					std::string(" has not benn setup yet")).c_str());
			}
		} catch (const std::exception& ex) {
			SE_LOG("Error while trying to reset! " << "Message : " << ex.what() << "\n");
		}
	}

	virtual std::string get_component_name() const override = 0;

	virtual ~se_domain() {
		reset();
	}
};

template<class domain_t, class config_t>
const size_t se_domain<domain_t, config_t>::default_power_value = 0;

template<class domain_t, class config_t>
const std::shared_ptr<se_cfg_validator<domain_t, config_t>> se_domain<domain_t, config_t>::cfg_validator = std::make_shared<se_cfg_validator_imp<domain_t, config_t>>();