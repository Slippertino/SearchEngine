#pragma once

#include <memory>
#include <thread_safe_containers/thread_safe_queue.hpp>
#include <thread_safe_containers/thread_safe_unordered_map.hpp>
#include "message_core.h"
#include "se_router.hpp"

class se_services_communication
{
protected:
	using requests_container = thread_safe_queue<std::pair<std::string, msg_request>>;
	using responses_container = thread_safe_unordered_map<decltype(msg_response::id), msg_response>;

protected:
	std::shared_ptr<se_router> router;

	requests_container  requests;
	responses_container responses;

protected:
	se_services_communication() = delete;
	se_services_communication(const std::shared_ptr<se_router>& in_router) : router(in_router)
	{ }

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

	template<typename context_t, typename... Args>
	std::string send_request(Args&&... args) {
		return router->send_message<message_type::REQUEST, context_t>(
			std::make_shared<context_t>(std::forward<Args>(args)...)
		);
	}

	template<typename context_t, typename... Args>
	void send_response(std::string id, Args&&... args) {
		router->send_message<message_type::RESPONSE, context_t>(
			std::make_shared<context_t>(std::forward<Args>(args)...),
			id
		);
	}

	void clear() {
		requests .clear();
		responses.clear();
	}

public:
	template<typename context_t>
	void record_message(const msg_request& msg) {
		requests.add({ typeid(context_t).name(), msg });
	}

	template<typename context_t>
	void record_message(const msg_response& msg) {
		responses.insert(msg.id, msg);
	}

	virtual ~se_services_communication() = default;
};