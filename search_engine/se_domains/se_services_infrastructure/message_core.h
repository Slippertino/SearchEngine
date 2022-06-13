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

#define SE_EXPAND( x ) x
#define SE_GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _NAME,...) _NAME

#define SE_PASTE2(func, v1) func(v1)
#define SE_PASTE3(func, v1, v2) SE_PASTE2(func, v1),SE_PASTE2(func, v2)
#define SE_PASTE4(func, v1, v2, v3) SE_PASTE2(func, v1),SE_PASTE3(func, v2, v3)
#define SE_PASTE5(func, v1, v2, v3, v4) SE_PASTE2(func, v1),SE_PASTE4(func, v2, v3, v4)
#define SE_PASTE6(func, v1, v2, v3, v4, v5) SE_PASTE2(func, v1),SE_PASTE5(func, v2, v3, v4, v5)
#define SE_PASTE7(func, v1, v2, v3, v4, v5, v6) SE_PASTE2(func, v1),SE_PASTE6(func, v2, v3, v4, v5, v6)
#define SE_PASTE8(func, v1, v2, v3, v4, v5, v6, v7) SE_PASTE2(func, v1),SE_PASTE7(func, v2, v3, v4, v5, v6, v7)

#define SE_PASTE(...) SE_EXPAND(SE_GET_MACRO(__VA_ARGS__, \
        SE_PASTE7, \
        SE_PASTE6, \
        SE_PASTE5, \
        SE_PASTE4, \
        SE_PASTE3, \
        SE_PASTE2, \
        SE_PASTE1)(__VA_ARGS__))

struct context
{
#define DECLARE_ARG(name) const decltype(name)& in_##name
#define DECLATE_INIT(name) name(in_##name)

#define SE_CONTEXT(type, ...) \
	type() = default; \
	explicit type(SE_EXPAND(SE_PASTE(DECLARE_ARG, __VA_ARGS__))) : \
				  SE_EXPAND(SE_PASTE(DECLATE_INIT, __VA_ARGS__)) { } \
	std::string to_string() const override {  \
		nlohmann::json js; \
		nlohmann::to_json(js, *this); \
		return js.dump(); \
	}

	context() = default;

	virtual std::string to_string() const = 0;
};

struct response_status : context
{
	runtime_status status = runtime_status::FAIL;
	std::string message;

	SE_CONTEXT(
		response_status, 
		status, 
		message
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		response_status,
		status,
		message
	)
};

template<message_type type>
struct message
{
	std::string id;
	std::shared_ptr<context> body;
};

using msg_request  = message<message_type::REQUEST>;
using msg_response = message<message_type::RESPONSE>;
