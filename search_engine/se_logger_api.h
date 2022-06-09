#pragma once

#include "se_logger/se_loggers_storage.h"
#include "se_domains/se_services_infrastructure/message_core.h"

#define SE_LOG_PROCESS(log_msg) LOG_BASE(log_msg, get_component_name())
#define SE_LOG_MESSAGE(msg_type, log_msg) LOG_BASE(log_msg, get_component_name(), std::to_string(static_cast<size_t>(msg_type)))

#define GET_MACRO(_1, _2, _NAME, ...) _NAME
#define MACRO_RECOMPOSER(argsWithParentheses) GET_MACRO argsWithParentheses
#define SE_LOG(...) MACRO_RECOMPOSER((__VA_ARGS__, SE_LOG_MESSAGE, SE_LOG_PROCESS)(__VA_ARGS__))