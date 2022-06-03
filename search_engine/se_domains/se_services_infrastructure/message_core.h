#pragma once

#include <string>
#include <memory>
#include <unordered_set>
#include <nlohmann/json.hpp>

enum class runtime_status
{
	FAIL,
	SUCCESS
};

enum class message_type
{
	REQUEST,
	RESPONSE
};

struct context
{
	context() = default;

	virtual std::string to_string() const = 0;

	#define CONVERT_TO_STRING std::string to_string() const override { nlohmann::json js; nlohmann::to_json(js, *this); return js.dump(); }
};

struct response_status : context
{
	runtime_status status = runtime_status::FAIL;
	std::string message;

	response_status() = default;
	explicit response_status(runtime_status in_status,
		const std::string& in_message) :
		status(in_status),
		message(in_message)
	{}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(response_status,
								   status,
								   message)
	CONVERT_TO_STRING
};

template<message_type type>
struct message
{
	std::string id;
	std::shared_ptr<context> body;
};

using msg_request  = message<message_type::REQUEST>;
using msg_response = message<message_type::RESPONSE>;