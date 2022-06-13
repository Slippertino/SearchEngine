#pragma once

#include "../se_services_infrastructure/message_core.h"
#include <converter_to_encoding_t.h>

struct site_recording_request : context
{
	std::string site;

	SE_CONTEXT(
		site_recording_request,
		site
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		site_recording_request,
		site
	)
};

struct site_recording_response : context
{
	response_status status;

	SE_CONTEXT(
		site_recording_response,
		status
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		site_recording_response,
		status
	)
};

struct url_to_analyze_request : context
{
	std::string prefix;
	std::string url;

	SE_CONTEXT(
		url_to_analyze_request,
		prefix,
		url
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		url_to_analyze_request,
		prefix,
		url
	)
};

struct url_to_analyze_response : context
{
	encoding_t page_encoding;
	std::string content;
	long status_code;
	bool is_valid;
	std::unordered_set<std::string> linked_urls;
	response_status status;

	SE_CONTEXT(
		url_to_analyze_response,
		page_encoding,
		content,
		status_code,
		is_valid,
		linked_urls,
		status
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		url_to_analyze_response,
		page_encoding,
		status_code,
		is_valid,
		status
	)
};

struct record_page_info_request : context
{
	std::string path;
	std::string content;
	long code;

	SE_CONTEXT(
		record_page_info_request,
		path,
		content,
		code
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		record_page_info_request,
		path,
		code
	)
};

struct record_page_info_response : context
{
	response_status status;

	SE_CONTEXT(
		record_page_info_response,
		status
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		record_page_info_response,
		status
	)
};

struct is_unique_page_url_request : context
{
	std::string url;

	SE_CONTEXT(
		is_unique_page_url_request,
		url
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		is_unique_page_url_request,
		url
	)
};

struct is_unique_page_url_response : context
{
	bool answer;
	response_status status;

	SE_CONTEXT(
		is_unique_page_url_response,
		answer,
		status
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		is_unique_page_url_response,
		answer,
		status
	)
};

struct init_database_request : context
{
	std::string database_name;

	SE_CONTEXT(
		init_database_request,
		database_name
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		init_database_request,
		database_name
	)
};

struct init_database_response : context
{
	response_status status;

	SE_CONTEXT(
		init_database_response, 
		status
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		init_database_response,
		status
	)
};

struct page_indexing_request : context
{
	std::string url;
	std::string prefix;

	SE_CONTEXT(
		page_indexing_request,
		url,
		prefix
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		page_indexing_request,
		url,
		prefix
	)
};