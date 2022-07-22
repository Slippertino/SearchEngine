#pragma once

#include <string>
#include <memory>
#include <functional>
#include "../extensions/thread_extensions/thread_safe_containers/thread_safe_unordered_map.hpp"
#include "../extensions/thread_extensions/thread_safe_containers/thread_safe_queue.hpp"
#include "../extensions/thread_extensions/thread_pool.hpp"
#include "messages/message_core.hpp"
#include "se_network_component.hpp"
#include "se_switch_board.hpp"
#include "se_logable_component.hpp"
#include "builders/se_builder.hpp"
#include "messages/message_id_generator.hpp"

template<class station_t, class config_t>
class se_work_station : public se_network_component,
						public se_logable_component {
#define SE_WORK_STATION(type_name, inherited_type_name, component_name) \
private: \
	friend class se_builder_imp<type_name>; \
protected: \
	type_name* get_object() const { \
		return const_cast<type_name*>(this); \
	} \
	std::string get_component_name() const override { \
		return std::string(#component_name); \
	} \
public: \
	type_name() = delete; \
	type_name(size_t id, const fs::path& path, const std::shared_ptr<se_switch_board>& in_switch) \
			: inherited_type_name(id, root / fs::path(get_full_name(get_component_name())), in_switch) \
	{ builder->build(const_cast<type_name*>(this)); } \

#define EXECUTE_CONTROLLED_WAITING(var_name, alias, expression) \
	execute_controlled_waiting<decltype(var_name)>([&](decltype(var_name)& alias) { \
		return expression; \
	}, var_name); \

#define GET_REQUEST(msg_info) get_request(msg_info); 
#define GET_RESPONSE(msg_id, var_name, type_name) type_name var_name; get_response(msg_id, var_name);

#define MAKE_REQUEST(id_name, msg_name, params) \
	std::string id_name = make_request<msg_name> params; 
#define MAKE_REQUEST_WITH_RESPONSE(name, msg_name_req, msg_name_resp, params) \
	msg_name_resp name; make_request_with_response<msg_name_req, msg_name_resp> params; 
#define MAKE_RESPONSE(msg_name, params) \
	make_response<msg_name> params; 

protected:
	using station_type				   = typename station_t;
	using config_type                  = typename config_t;
	using request_responder            = std::function<void(station_t*, msg_request)>;
	using responders_container         = thread_safe_unordered_map<std::string, request_responder>;
	using requests_container           = thread_safe_queue<std::pair<std::string, msg_request>>;
	using responses_container          = thread_safe_unordered_map<decltype(msg_response::id), msg_response>;

protected:
	static message_id_generator id_generator;
	static std::shared_ptr<se_builder<station_t>> builder;

protected:
	std::shared_ptr<se_switch_board> internal_switch = new se_switch_board();
	std::shared_ptr<se_switch_board> external_switch = new se_switch_board();

	requests_container  requests;
	responses_container responses;

protected:
	bool stop_flag = false;

	thread_pool pool;

	responders_container responders;

	thread_safe_queue<std::pair<std::string, std::string>> responses_id;
	thread_safe_unordered_map<std::string, nullptr_t> unused_response_type_names;

protected:
	bool try_get_request(std::pair<std::string, msg_request>& msg_data) {
		return requests.try_get(msg_data);
	}

	bool try_erase_request(std::pair<std::string, msg_request>& msg_data) {
		return requests.try_erase(msg_data);
	}

	bool try_get_response(std::pair<std::string, msg_response>& msg_data) {
		return responses.try_get(msg_data.first, msg_data.second);
	}

	bool try_erase_response(std::pair<std::string, msg_response>& msg_data) {
		return responses.try_erase(msg_data.first, msg_data.second);
	}

	template<class context_t, class... Args>
	std::string send_request(Args&&... args) {
		return send_message(
				message_type::REQUEST,
				typeid(context_t).name(),
				std::make_shared<context_t>(std::forward<Args>(args)...)
			);
	}

	template<class context_t, class... Args>
	void send_response(std::string id, Args&&... args) {
		send_message(
			message_type::RESPONSE,
			typeid(context_t).name(),
			std::make_shared<context_t>(std::forward<Args>(args)...),
			id
		);
	}

	virtual void clear() = 0 {
		pool	    .clear();
		responses_id.clear();
		requests	.clear();
		responses	.clear();
	}

protected:
	template<class T>
	void execute_controlled_waiting(const std::function<bool(T&)>& func, T& value) {
		bool res;

		pool.set_inactive(std::this_thread::get_id());
		do {
			std::this_thread::sleep_for(10ms);
			res = func(value);
		} while (!res && !stop_flag);
		pool.set_active(std::this_thread::get_id());
	}

	void requests_handler() {
		while (!stop_flag) {
			std::pair<std::string, msg_request> msg_info;
			GET_REQUEST(msg_info)

			if (stop_flag) {
				continue;
			}

			request_responder responder;
			if (responders.try_get(msg_info.first, responder)) {
				responder(get_object(), msg_info.second);
			}
		}
	}

	void responses_handler() {
		while (!stop_flag) {
			std::pair<std::string, std::string> resp_info;
			extract_from_ts_queue(responses_id, resp_info);

			if (unused_response_type_names.find(resp_info.second) != unused_response_type_names.end()) {
				try {
					std::pair<std::string, msg_response> msg_info{ resp_info.first, {} };
					EXECUTE_CONTROLLED_WAITING(msg_info, msg_info, try_erase_response(msg_info))
					SE_LOG(message_type::RESPONSE, "Received response : " << msg_info.second.body->to_string() << "\n");
				} catch (const std::exception& ex) {
					SE_LOG(message_type::RESPONSE, "Error while trying to get response with following body type: " << resp_info.second
						<< "! Message : " << ex.what() << "\n");
				}
			}
		}
	}

	void get_request(std::pair<std::string, msg_request>& msg_info) {
		try {
			EXECUTE_CONTROLLED_WAITING(msg_info, msg_info, try_erase_request(msg_info))
			SE_LOG(message_type::REQUEST, "Received request : " << msg_info.second.body->to_string() << "\n");
		} catch (const std::exception& ex) {
			std::cout << "Error while trying to get request! Message : " << ex.what() << "\n";
			SE_LOG(message_type::REQUEST, "Error while trying to get request! Message : " << ex.what() << "\n");
		}
	}

	template<typename context_t>
	void get_response(const std::string& msg_id, context_t& body) {
		try {
			std::pair<std::string, msg_response> msg_info{ msg_id, {} };
			EXECUTE_CONTROLLED_WAITING(msg_info, msg_info, try_erase_response(msg_info))
			body = *static_cast<context_t*>(msg_info.second.body.get());
			SE_LOG(message_type::RESPONSE, "Received response : " << body.to_string() << "\n");
		} catch (const std::exception& ex) {
			std::cout << "Error while trying to get response with following body type: " << typeid(context_t).name()
					  << "! Message : " << ex.what() << "\n";
			SE_LOG(message_type::RESPONSE, "Error while trying to get response with following body type: " << typeid(context_t).name()
											<< "! Message : " << ex.what() << "\n");
		}
	}

	template<typename context_t, typename... Args>
	std::string make_request(Args&&... args) {
		try {
			auto body = std::make_shared<context_t>(std::forward<Args>(args)...);
			auto msg_id = send_request<context_t>(std::ref(*body));
			responses_id.add({ msg_id, std::string(typeid(context_t).name()) });
			SE_LOG(message_type::REQUEST, "Sent request : " << body->to_string() << "\n");
			return msg_id;
		} catch (const std::exception& ex) {
			std::cout << "Error while trying to make request with following body type: " << typeid(context_t).name()
				<< "! Message : " << ex.what() << "\n";
			SE_LOG(message_type::REQUEST, "Error while trying to make request with following body type: " << typeid(context_t).name()
				<< "! Message : " << ex.what() << "\n");
		}
	}

	template<typename context_t, typename... Args>
	void make_response(std::string msg_id, Args&&... args) {
		try {
			auto body = std::make_shared<context_t>(std::forward<Args>(args)...);
			send_response<context_t>(msg_id, std::ref(*body));
			SE_LOG(message_type::RESPONSE, "Sent response : " << body->to_string() << "\n");
		} catch (const std::exception& ex) {
			std::cout << "Error while trying to make response with following body type: " << typeid(context_t).name()
				<< "! Message : " << ex.what() << "\n";
			SE_LOG(message_type::RESPONSE, "Error while trying to make response with following body type: " << typeid(context_t).name()
				<< "! Message : " << ex.what() << "\n");
		}
	}

	template<typename request_t, typename response_t, typename... Args>
	void make_request_with_response(response_t& body, Args&&... args) {
		auto id = se_work_station<station_t, config_t>::make_request<request_t>(std::forward<Args>(args)...);
		se_work_station<station_t, config_t>::get_response<response_t>(id, body);
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

	virtual station_t* get_object() const = 0;

public:
	se_work_station() = delete;
	se_work_station(size_t id, const fs::path& path, const std::shared_ptr<se_switch_board>& in_switch) :
		se_logable_component(id, path)
		external_switch(in_switch) 
	{ }

	std::string send_message(
		message_type type, 
		const std::string& msg_name, 
		const std::shared_ptr<context>& body, 
		std::string id = std::string()) override {

		if (id.empty()) {
			id = id_generator.generate({
				std::to_string(std::hash<std::thread::id>()(std::this_thread::get_id())),
				msg_name
			});
		}

		if (has_switch(msg_name)) {
			for (auto&& s : get_components(msg_name)) {
				if (auto s_ptr = s.lock()) {
					s_ptr->send_message(type, msg_name, body, id);
				}
			}

			return id;

		} else {
			switch (type)
			{
			case message_type::REQUEST:
				requests.add({ msg_name, msg_request{id, body} });
				break;
			case message_type::RESPONSE:
				responses.insert(id, msg_response{ id, body });
				break;
			default:
				break;
			}
		}
	}

	virtual std::string get_component_name() const override = 0;

	virtual ~se_work_station() = default;
};

template<class station_t, class config_t>
message_id_generator se_work_station<station_t, config_t>::id_generator;

template<class station_t, class config_t>
std::shared_ptr<se_builder<station_t>> se_work_station<station_t, config_t>::builder = std::make_shared<se_builder_imp<station_t>>();