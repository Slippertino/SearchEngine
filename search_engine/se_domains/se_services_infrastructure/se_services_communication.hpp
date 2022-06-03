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
		auto res = requests.try_erase(msg_data);
		return res;
	}

	template<typename context_t>
	bool try_get_response(const std::string& msg_id, context_t& body) {
		msg_response msg;
		auto res = responses.try_erase(msg_id, msg);

		if (res) {
			body = *static_cast<context_t*>(msg.body.get());
		}

		return res;
	}

	template<typename context_t, typename... Args>
	std::string make_request(Args&&... args) {
		return router->send_message<message_type::REQUEST, context_t>(
			std::make_shared<context_t>(std::forward<Args>(args)...)
		);
	}

	template<typename context_t, typename... Args>
	void make_response(std::string id, Args&&... args) {
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