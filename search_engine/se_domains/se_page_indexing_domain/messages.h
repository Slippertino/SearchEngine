#pragma once

#include "../se_services_infrastructure/message_core.h"
#include <converter_to_encoding_t.h>

struct site_recording_request : context
{
	std::string site;

	site_recording_request() = default;
	explicit site_recording_request(const std::string& in_site) :
		site(in_site)
	{}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(site_recording_request,
								   site)
	CONVERT_TO_STRING
};

struct site_recording_response : context
{
	response_status status;

	site_recording_response() = default;
	explicit site_recording_response(const response_status& in_status) :
		status(in_status)
	{}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(site_recording_response,
								   status)
	CONVERT_TO_STRING
};

struct url_to_analyze_request : context
{
	std::string prefix;
	std::string url;

	url_to_analyze_request() = default;
	explicit url_to_analyze_request(const std::string& in_prefix,
								    const std::string& in_url) :
		prefix(in_prefix),
		url(in_url)
	{}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(url_to_analyze_request,
								   prefix,
								   url)
	CONVERT_TO_STRING
};

struct url_to_analyze_response : context
{
	encoding_t page_encoding;
	std::string content;
	long status_code;
	bool is_valid;
	std::unordered_set<std::string> linked_urls;
	response_status status;

	url_to_analyze_response() = default;
	explicit url_to_analyze_response(encoding_t in_page_encoding,
									 const std::string& in_content,
									 long in_status_code,
									 bool in_is_valid,
									 const std::unordered_set<std::string>& in_linked_urls,
									 const response_status& in_status) :
		page_encoding(in_page_encoding),
		content(in_content),
		status_code(in_status_code),
		is_valid(in_is_valid),
		linked_urls(in_linked_urls),
		status(in_status)
	{}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(url_to_analyze_response,
								   page_encoding,
								   content,
								   status_code,
								   is_valid,
								   linked_urls,
								   status)
	CONVERT_TO_STRING
};

struct record_page_info_request : context
{
	std::string path;
	std::string content;
	long code;

	record_page_info_request() = default;
	explicit record_page_info_request(const std::string& in_path,
									  const std::string& in_content,
									  long in_code) :
		path(in_path),
		content(in_content),
		code(in_code)
	{}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(record_page_info_request,
								   path,
								   content,
								   code)
	CONVERT_TO_STRING
};

struct record_page_info_response : context
{
	response_status status;

	record_page_info_response() = default;

	explicit record_page_info_response(const response_status& in_status) :
		status(in_status)
	{}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(record_page_info_response,
								   status)
	CONVERT_TO_STRING
};

struct is_unique_page_url_request : context
{
	std::string url;

	is_unique_page_url_request() = default;
	explicit is_unique_page_url_request(const std::string& in_url) :
		url(in_url)
	{}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(is_unique_page_url_request,
								   url)
	CONVERT_TO_STRING
};

struct is_unique_page_url_response : context
{
	bool answer;
	response_status status;

	is_unique_page_url_response() = default;
	explicit is_unique_page_url_response(bool in_answer,
									     const response_status& in_status) :
		answer(in_answer),
		status(in_status)
	{}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(is_unique_page_url_response,
								   answer,
							       status)
	CONVERT_TO_STRING
};

struct init_database_request : context
{
	std::string database_name;

	init_database_request() = default;
	explicit init_database_request(const std::string& in_database_name) :
		database_name(in_database_name)
	{}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(init_database_request,
								   database_name)
	CONVERT_TO_STRING
};

struct init_database_response : context
{
	response_status status;

	init_database_response() = default;
	explicit init_database_response(const response_status& in_status) :
		status(in_status)
	{}

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(init_database_response,
								  status)
	CONVERT_TO_STRING
};