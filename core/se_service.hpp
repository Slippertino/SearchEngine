#pragma once 

#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <functional>
#include <memory>
#include "se_work_station.hpp"
#include "se_service_behaviour.hpp"
#include "se_switch_board.hpp"

template<class service_t, class config_t>
class se_service : public se_work_station<service_t, config_t>,
				   public se_service_behaviour {
protected:
	using power_consumer = std::function<void(service_t*)>;
	using power_distribution_container = std::vector<std::pair<power_consumer, size_t>>;

protected:
	power_distribution_container power_distribution;

protected:
	void disturbe_processes(service_t* service, size_t threads_count) {
		stop_flag = false;
		pool = thread_pool(threads_count + 1);

		size_t total(0);
		std::for_each(power_distribution.begin(),
					  power_distribution.end(),
					  [&total](const auto& val) {total += val.second; });

		for (auto& process : power_distribution) {
			int cur_threads = threads_count / total * process.second;

			if (cur_threads < 1)
				throw std::exception("lack of threads");

			for (auto i = 0; i < cur_threads; ++i) {
				pool.add_task(process.first, service);
			}
		}

		pool.add_task(&service_t::responses_handler, service);
	}

	virtual service_t* get_object() const override = 0;
	virtual void setup_base(config_type* config) = 0;
	virtual std::string get_component_name() const override = 0;

public:
	se_service() = delete;
	se_service(size_t id, const fs::path& path, const std::shared_ptr<se_switch_board>& in_switch) 
		: se_work_station<service_t, config_t>(id, path, in_switch)
	{ }

	auto get_power_value() const {
		size_t res{ 0 };
		for (auto& cur : power_distribution)
			res += cur.second;
		return res;
	}

	bool status() const override {
		auto st = !pool.is_work_finished();
		SE_LOG("Service successfully returned " << st << " as the status!\n");
		return st;
	}

	void run(size_t threads_count) override {
		try {
			disturbe_processes(get_object(), threads_count);
			SE_LOG("Service was successfully run with " << threads_count << " flows!\n");
		} catch (const std::exception& ex) {
			SE_LOG("Error while trying to run service with " << threads_count << " flows! Message : " << ex.what() << "\n");
		}
	}

	void stop() override {
		try {
			stop_flag = true;
			pool.clear();
			se_services_communication::clear();
			SE_LOG("Service was successfully stopped!\n");
		} catch (const std::exception& ex) {
			SE_LOG("Error while trying to stop! " << "Message : " << ex.what() << "\n");
		}
	}

	void setup(const std::shared_ptr<se_config>& config) override {
		try {
			config_type* conf = dynamic_cast<config_type*>(config.get());
			if (conf) {
				setup_base(conf);
				SE_LOG("Successful setup!\n");
			} else {
				throw std::exception("Unsuitable config for setup!\n");
			}
		} catch (const std::exception& ex) {
			SE_LOG("Unsuccessful setup! " << ex.what() << "\n");
		}
	}

	void reset() override {
		try {
			se_service<service_t, config_t>::clear();
			SE_LOG("Service was successfully reset!\n");
		} catch (const std::exception& ex) {
			SE_LOG("Error while trying to reset! " << "Message : " << ex.what() << "\n");
		}
	}

	virtual ~se_service() = default;
};