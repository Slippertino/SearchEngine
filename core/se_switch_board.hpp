#pragma once

#include <string>
#include <memory>
#include "messages/message_core.hpp"
#include "se_network_component.hpp"

class se_switch_board : public se_network_component {
public:
	se_switch_board() = default;

	std::string send_message(message_type type, 
							 const std::string& msg_name, 
							 const std::shared_ptr<context>& body, 
							 std::string id) override {
		for (auto&& s : get_components(msg_name)) {
			if (auto s_ptr = s.lock()) {
				s_ptr->send_message(type, msg_name, body, id);
			}
		}

		return id;
	}
};