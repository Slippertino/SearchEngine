#pragma once

#include <functional>
#include <thread_pool.hpp>
#include <thread_safe_containers/thread_safe_unordered_map.hpp>
#include "../configuration.h"
#include "../se_domains/builders/builder.hpp"
#include "se_services_infrastructure/se_services_communication.hpp"
#include "se_api.h"

template<typename service_t>
class se_service : public se_services_communication,
				   public se_api
{
#define SE_SERVICE(service_name) \
private: \
	friend class builder<service_name>; \
	void requests_handler() { \
		while (!stop_flag) { \
			std::pair<std::string, msg_request> msg_info; \
			pool.set_inactive(std::this_thread::get_id()); \
			GET_REQUEST(msg_info) \
			if (stop_flag) \
				continue; \
			pool.set_active(std::this_thread::get_id()); \
			request_responder responder; \
			if (responders.try_get(msg_info.first, responder)) \
				responder(this, msg_info.second); \
		} \
	} \
public: \
	void run(size_t threads_count) override { \
		disturbe_processes(this, threads_count); \
	} \
	void reset() override { \
		se_services_communication::clear(); \
		service_name::clear(); \
	} \

#define EXECUTE_CONTROLLED_WAITING(var_name, alias, expression) \
	execute_controlled_waiting<decltype(var_name)>([&](decltype(var_name)& alias) { \
		return expression; \
	}, var_name); \

#define GET_REQUEST(msg_info) get_request(msg_info); 
#define GET_RESPONSE(msg_id, body) get_response(msg_id, body);

#define MAKE_REQUEST(msg_name, params) \
	make_request<msg_name##_request> params; 
#define MAKE_REQUEST_WITH_RESPONSE(name, msg_name, params) \
	msg_name##_response name; make_request_with_response<msg_name##_request, msg_name##_response> params; 
#define MAKE_RESPONSE(msg_name, params) \
	make_response<msg_name##_response> params; 

protected:
	using request_responder = std::function<void(service_t*, msg_request)>;
	using power_consumer = std::function<void(service_t*)>;
	using responders_container = thread_safe_unordered_map<std::string, request_responder>;
	using power_distribution_container = std::vector<std::pair<power_consumer, size_t>>;

protected:
	bool stop_flag = false;

	thread_pool pool;

	responders_container responders;
	power_distribution_container power_distribution;

protected:
	template<typename T>
	void execute_controlled_waiting(const std::function<bool(T&)>& func, T& value) {
		bool res;
		do {
			std::this_thread::sleep_for(10ms);
			res = func(value);
		} while (!res && !stop_flag);
	}

	void get_request(std::pair<std::string, msg_request>& msg_info) {
		EXECUTE_CONTROLLED_WAITING(msg_info, msg_info, try_get_request(msg_info))
	}

	template<typename context_t>
	void get_response(const std::string& msg_id, context_t& body) {
		EXECUTE_CONTROLLED_WAITING(body, body, try_get_response(msg_id, body))
	}

	template<typename request_t, typename response_t, typename... Args>
	void make_request_with_response(response_t& body, Args... args) {
		auto id = make_request<request_t>(args...);
		get_response<response_t>(id, body);
	}

	template<typename T>
	void extract_from_ts_queue(thread_safe_queue<T>& queue, T& value) {
		EXECUTE_CONTROLLED_WAITING(value, value, queue.try_erase(value))
	}

	template<typename T>
	void get_from_ts_queue(thread_safe_queue<T>& queue, T& value) {
		EXECUTE_CONTROLLED_WAITING(value, value, queue.try_get(value))
	}

	template<typename TKey, typename TValue>
	void extract_from_ts_unordered_map(thread_safe_unordered_map<TKey, TValue>& map, 
									   TKey& key, 
									   TValue& value) {
		EXECUTE_CONTROLLED_WAITING(value, value, map.try_erase(key, value))
	}

	template<typename TKey, typename TValue>
	void get_from_ts_unordered_map(thread_safe_unordered_map<TKey, TValue>& map,
								   TKey& key,
								   TValue& value) {
		EXECUTE_CONTROLLED_WAITING(value, value, map.try_get(key, value))
	}

	void disturbe_processes(service_t* service, size_t threads_count) {
		stop_flag = false;
		pool = thread_pool(threads_count);

		size_t total(0);
		std::for_each(power_distribution.begin(),
					  power_distribution.end(),
					  [&total](const auto& val) {total += val.second; });

		for (auto& process : power_distribution) {
			int cur_threads = threads_count / total * process.second;

			if (cur_threads < 1)
				throw std::exception("lack of threads");

			for (auto i = 0; i < cur_threads; ++i) {
				auto id = pool.add_task(process.first, service);
			}
		}
	}

	virtual void clear() = 0;

public:
	se_service() = delete;
	se_service(const std::shared_ptr<se_router>& in_router) : se_services_communication(in_router)
	{ }

	bool status() const override {
		return !pool.is_work_finished();
	}

	void stop() override {
		stop_flag = true;
		pool.clear();
		se_services_communication::clear();
		std::cout << "бшьек!\n";
	}

	virtual void run(size_t threads_count) override
	{ }

	virtual void setup(const configuration& config) override
	{ }

	virtual void reset() override 
	{ }

	virtual ~se_service() = default;
};