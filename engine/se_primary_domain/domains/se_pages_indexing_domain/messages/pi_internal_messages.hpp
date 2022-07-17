#pragma once

#include <core/messages/message_core.hpp>
#include <search_engine_analyzers/tools/html_text_analyzer.hpp>s
#include <search_engine_analyzers/text_property_types/se_encoding.hpp>
#include <search_engine_analyzers/text_property_types/se_language.hpp>

struct sites_list_recording_request : context {
	string_enc site;

	SE_CONTEXT(
		sites_list_recording_request, -1,
		site
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		sites_list_recording_request,
		site
	)
};

struct sites_list_recording_response : context {
	response_status status;

	SE_CONTEXT(
		sites_list_recording_response, -1,
		status
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		sites_list_recording_response,
		status
	)
};

struct url_to_analyze_request : context {
	string_enc prefix;
	string_enc url;

	SE_CONTEXT(
		url_to_analyze_request, -1,
		prefix,
		url
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		url_to_analyze_request,
		prefix,
		url
	)
};

struct url_to_analyze_response : context {
	se_encoding page_encoding;
	string_enc content;
	long status_code;
	bool is_valid;
	std::unordered_set<string_enc> linked_urls;
	response_status status;

	SE_CONTEXT(
		url_to_analyze_response, -1,
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

struct record_page_info_request : context {
	string_enc path;
	string_enc content;
	long code;

	SE_CONTEXT(
		record_page_info_request, -1,
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

struct record_page_info_response : context {
	response_status status;

	SE_CONTEXT(
		record_page_info_response, -1,
		status
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		record_page_info_response,
		status
	)
};

struct is_unique_page_url_request : context {
	string_enc url;

	SE_CONTEXT(
		is_unique_page_url_request, -1,
		url
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		is_unique_page_url_request,
		url
	)
};

struct is_unique_page_url_response : context {
	bool answer;
	response_status status;

	SE_CONTEXT(
		is_unique_page_url_response, -1,
		answer,
		status
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		is_unique_page_url_response,
		answer,
		status
	)
};

struct init_database_request : context {
	string_enc database_name;

	SE_CONTEXT(
		init_database_request, -1,
		database_name
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		init_database_request,
		database_name
	)
};

struct init_database_response : context {
	response_status status;

	SE_CONTEXT(
		init_database_response, -1,
		status
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		init_database_response,
		status
	)
};

struct page_indexing_request : context {
	string_enc url;
	string_enc prefix;

	SE_CONTEXT(
		page_indexing_request, -1,
		url,
		prefix
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		page_indexing_request,
		url,
		prefix
	)
};

struct page_indexing_response : context {
	response_status status;

	SE_CONTEXT(
		page_indexing_response, -1,
		status
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		page_indexing_response,
		status
	)
};

struct page_info_request : context {
	string_enc url;

	SE_CONTEXT(
		page_info_request, -1,
		url
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		page_info_request,
		url
	)
};

struct page_info_response : context {
	se_encoding page_encoding;
	std::vector<std::tuple<std::string, se_language, html_text_analyzer::ratio_type>> text_excerpts;
	response_status status;

	SE_CONTEXT(
		page_info_response, -1,
		page_encoding,
		text_excerpts,
		status
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		page_info_response,
		page_encoding,
		status,
		text_excerpts
	)
};

struct page_and_site_id_request : context {
	string_enc page_url;
	string_enc site_url;

	SE_CONTEXT (
		page_and_site_id_request, -1,
		page_url,
		site_url
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE (
		page_and_site_id_request,
		page_url,
		site_url
	)
};

struct page_and_site_id_response : context {
	size_t page_id;
	size_t site_id;
	response_status status;

	SE_CONTEXT (
		page_and_site_id_response, -1,
		page_id,
		site_id,
		status
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE (
		page_and_site_id_response,
		page_id,
		site_id,
		status
	)
};

struct record_word_info_request : context {
	size_t site_id;
	std::vector<std::pair<string_enc, string_enc>> words_params;

	SE_CONTEXT(
		record_word_info_request, -1,
		site_id,
		words_params
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		record_word_info_request,
		site_id,
		words_params
	)
};

struct record_word_info_response : context {
	std::vector<size_t> words_id;
	response_status status;

	SE_CONTEXT(
		record_word_info_response, -1,
		words_id,
		status
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		record_word_info_response,
		words_id,
		status
	)
};

struct record_word_to_index_request : context {
	std::vector<std::tuple<size_t, size_t, html_text_analyzer::ratio_type>> words_index_params;

	SE_CONTEXT(
		record_word_to_index_request, -1,
		words_index_params
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		record_word_to_index_request,
		words_index_params
	)
};

struct record_word_to_index_response : context {
	response_status status;

	SE_CONTEXT(
		record_word_to_index_response, -1,
		status
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		record_word_to_index_response,
		status
	)
};