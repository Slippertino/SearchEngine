#pragma once

#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <thread_safe_containers/thread_safe_unordered_map.hpp>
#include "message_core.h"
#include "message_id_generator.h"

class se_services_communication;

class se_router {
private:
	thread_safe_unordered_map<std::string, std::vector<std::weak_ptr<se_services_communication>>> routing_table;
	message_id_generator id_generator;
	std::mutex subscribe_locker;

private:
	std::vector<std::weak_ptr<se_services_communication>> get_services(const std::string& key) {
		return routing_table.wait_and_get(key);
	}

public:
	se_router() = default;

	template<typename context_t>
	void subscribe(const std::shared_ptr<se_services_communication>& service) {
		auto key = typeid(context_t).name();

		if (routing_table.find(key) == routing_table.cend()) {
			routing_table.insert(key, {std::weak_ptr<se_services_communication>(service)} );
		}
		else {
			std::vector<std::weak_ptr<se_services_communication>> cur_subscribers = routing_table.wait_and_get(key);
			std::lock_guard<std::mutex> locker(subscribe_locker);
			cur_subscribers.push_back(std::weak_ptr<se_services_communication>(service));
			routing_table.set(key, cur_subscribers);
		}
	}

	template<message_type message_t, typename context_t>
	std::string send_message(const std::shared_ptr<context>& context, std::string id = std::string()) {
		auto key = typeid(context_t).name();

		message<message_t> msg;

		if (message_t == message_type::REQUEST) {
			msg.id = id_generator.generate({
				std::to_string(std::hash<std::thread::id>()(std::this_thread::get_id())),
				key
			});
		}
		else if (message_t == message_type::RESPONSE) {
			msg.id = id;
		}

		msg.body = context;

		for (auto&& s : get_services(key)) {
			if (auto s_ptr = s.lock())
				s_ptr->record_message<context_t>(msg);
		}

		return msg.id;
	}
};