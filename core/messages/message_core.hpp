#pragma once

#include <string>
#include <memory>
#include <unordered_set>
#include <search_engine_analyzers/tools/se_encoder.hpp>
#include "../se_json.hpp"
#include "json_encoding_converter.hpp"

enum class runtime_status {
	FAIL,
	SUCCESS
};

enum class message_type {
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

#define DECLARE_ARG(name) const decltype(name)& in_##name
#define DECLATE_INIT(name) name(in_##name)
#define DECLARE_REF(name) std::ref(name)

#define CODE_DATA(func_name, method_name) \
	template<typename... Args> \
	void func_name(Args&&... args) {} \
	template<typename T, typename... Args> \
	void func_name(T&& arg, Args&&... args) { \
		json_encoding_converter<T>().method_name(std::forward<T>(arg)); \
		func_name(std::forward<Args>(args)...); \
	}

#define CODING_DATA_METHODS CODE_DATA(encode_data, encode) CODE_DATA(decode_data, decode)

#define SE_CONTEXT(type, json_dump_format, ...) \
	type() = default; \
	explicit type(SE_EXPAND(SE_PASTE(DECLARE_ARG, __VA_ARGS__))) : \
				  SE_EXPAND(SE_PASTE(DECLATE_INIT, __VA_ARGS__)) { } \
	std::string to_string() override {  \
		nlohmann::json js; \
		encode_data(SE_EXPAND(SE_PASTE(DECLARE_REF, __VA_ARGS__))); \
		nlohmann::to_json(js, *this); \
		decode_data(SE_EXPAND(SE_PASTE(DECLARE_REF, __VA_ARGS__))); \
		return js.dump(json_dump_format); \
	}

struct context {
	CODING_DATA_METHODS

	context() = default;

	virtual std::string to_string() = 0;
};


struct response_status : context {
	runtime_status status = runtime_status::FAIL;
	string_enc message = {"", encoding_t::UNKNOWN};

	SE_CONTEXT(
		response_status, -1,
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
struct message {
	std::string id;
	std::shared_ptr<context> body;
};

using msg_request  = message<message_type::REQUEST>;
using msg_response = message<message_type::RESPONSE>;
