#pragma once

#include <functional>
#include <thread_pool.hpp>
#include <thread_safe_containers/thread_safe_unordered_map.hpp>
#include "../configuration.h"
#include "../builders/builder.hpp"
#include "se_services_infrastructure/se_services_communication.hpp"
#include "../se_component.h"
#include "../se_logger_api.h"

template<typename service_t>
class se_service : public se_services_communication,
				   public se_component
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
		try { \
			disturbe_processes(this, threads_count); \
			SE_LOG("Service was successfully run with " << threads_count << " flows!\n"); \
		} catch (const std::exception& ex) { \
			SE_LOG("Error while trying to run service with " << threads_count << " flows! Message : " << ex.what() << "\n"); \
			throw ex; \
		} \
	} \
	void reset() override { \
		try { \
			se_services_communication::clear(); \
			service_name::clear(); \
			SE_LOG("Service was successfully reset!\n"); \
		} catch (const std::exception& ex) { \
			SE_LOG("Error while trying to reset! " << "Message : " << ex.what() << "\n"); \
			throw ex; \
		} \
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
		try {
			EXECUTE_CONTROLLED_WAITING(msg_info, msg_info, try_get_request(msg_info))
			SE_LOG(message_type::REQUEST, "Received request : " << msg_info.second.body->to_string() << "\n");
		} catch (const std::exception& ex) {
			SE_LOG(message_type::REQUEST, "Error while trying to get request! Message : " << ex.what() << "\n");
			throw ex;
		}
	}

	template<typename context_t>
	void get_response(const std::string& msg_id, context_t& body) {
		try {
			EXECUTE_CONTROLLED_WAITING(body, body, try_get_response(msg_id, body))
			SE_LOG(message_type::RESPONSE, "Received response : " << body.to_string() << "\n");
		} catch (const std::exception& ex) {
			SE_LOG(message_type::RESPONSE, "Error while trying to get response with following body type: " << typeid(context_t).name()
										<< "! Message : " << ex.what() << "\n");
			throw ex;
		}
	}

	template<typename context_t, typename... Args>
	std::string make_request(Args&&... args) {
		try {
			auto body = std::make_shared<context_t>(std::forward<Args>(args)...);
			auto msg_id = send_request<context_t>(std::ref(*body));
			SE_LOG(message_type::REQUEST, "Sent request : " << body->to_string() << "\n");
			return msg_id;
		} catch (const std::exception& ex) {
			SE_LOG(message_type::REQUEST, "Error while trying to make request with following body type: " << typeid(context_t).name()
									   << "! Message : " << ex.what() << "\n");
			throw ex;
		}
	}

	template<typename context_t, typename... Args>
	void make_response(std::string msg_id, Args&&... args) {
		try {
			auto body = std::make_shared<context_t>(std::forward<Args>(args)...);
			send_response<context_t>(msg_id, std::ref(*body));
			SE_LOG(message_type::RESPONSE, "Sent response : " << body->to_string() << "\n");
		} catch (const std::exception& ex) {
			SE_LOG(message_type::RESPONSE, "Error while trying to make response with following body type: " << typeid(context_t).name()
										<< "! Message : " << ex.what() << "\n");
			throw ex;
		}
	}

	template<typename request_t, typename response_t, typename... Args>
	void make_request_with_response(response_t& body, Args... args) {
		auto id = se_service<service_t>::make_request<request_t>(args...);
		se_service<service_t>::get_response<response_t>(id, body);
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

	virtual std::string get_component_name() const override = 0;

	virtual void clear() = 0;

public:
	se_service() = delete;
	se_service(size_t id, const fs::path& path, const std::shared_ptr<se_router>& in_router) : 
		se_component(id, path), 
		se_services_communication(in_router)
	{ }

	bool status() const override {
		auto st = !pool.is_work_finished();
		SE_LOG("Service successfully returned " << st << " as the status!\n");
		return st;
	}

	void stop() override {
		try {
			stop_flag = true;
			pool.clear();
			se_services_communication::clear();
			SE_LOG("Service was successfully stopped!\n");
		} catch (const std::exception& ex) {
			SE_LOG("Error while trying to stop! " << "Message : " << ex.what() << "\n");
			throw ex;
		}
	}

	virtual void run(size_t threads_count) override = 0;
	virtual void setup(const configuration& config) override = 0;
	virtual void reset() override = 0;

	virtual ~se_service() = default;
};