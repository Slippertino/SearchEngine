#pragma once

#include <string>
#include <memory>
#include <mutex>
#include "../extensions/thread_extensions/thread_safe_containers/thread_safe_unordered_map.hpp"
#include "messages/message_core.hpp"

class se_network_component {
protected:
	thread_safe_unordered_map<size_t, std::vector<std::weak_ptr<se_network_component>>> switch_table;
	std::mutex subscribe_locker;

protected:
	std::vector<std::weak_ptr<se_network_component>> get_components(const std::string& key) {
		return switch_table.wait_and_get(std::hash<std::string>()(key));
	}

public:
	bool has_switch(const std::string& msg_name) const {
		auto msg_hash = std::hash<std::string>()(msg_name);

		return switch_table.find(msg_hash) != switch_table.cend();
	}

	void subscribe(std::shared_ptr<se_network_component> receiver, const std::string& msg_name) {
		auto msg_hash = std::hash<std::string>()(msg_name);

		if (switch_table.find(msg_hash) == switch_table.cend()) {
			switch_table.insert(msg_hash, { std::weak_ptr<se_network_component>(receiver) });
		} else {
			std::vector<std::weak_ptr<se_network_component>> cur_subscribers = switch_table.wait_and_get(msg_hash);
			std::lock_guard<std::mutex> locker(subscribe_locker);
			cur_subscribers.push_back(std::weak_ptr<se_network_component>(receiver));
			switch_table.set(msg_hash, cur_subscribers);
		}
	}

	virtual std::string send_message(message_type type, const std::string& msg_name, const std::shared_ptr<context>& body, std::string id) = 0;
};