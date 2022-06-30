#pragma once

#include <text_property_types/string_enc.hpp>
#include "../se_services_infrastructure/message_core.h"

struct database_integrity_check_request : context {
	string_enc database_name;

	SE_CONTEXT(
		database_integrity_check_request, 
		database_name
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		database_integrity_check_request, 
		database_name
	)
};

struct database_integrity_check_response : context {
	bool is_valid;
	response_status status;

	SE_CONTEXT(
		database_integrity_check_response,
		is_valid,
		status
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		database_integrity_check_response,
		is_valid,
		status
	)
};

struct sites_id_request : context 
{
	size_t dummy{ 0 };

	SE_CONTEXT(
		sites_id_request,
		dummy
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		sites_id_request,
		dummy
	)
};	

struct sites_id_response : context {
	std::vector<size_t> ids;
	response_status status;

	SE_CONTEXT(
		sites_id_response,
		ids,
		status
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		sites_id_response,
		ids,
		status
	)
};

struct words_info_request : context {
	size_t site_id;
	std::vector<std::pair<string_enc, string_enc>> stemmed_words;

	SE_CONTEXT(
		words_info_request,
		site_id,
		stemmed_words
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		words_info_request,
		site_id,
		stemmed_words
	)
};

struct words_info_response : context {
	std::vector<std::pair<size_t, size_t>> words_id_freq;
	size_t pages_count;
	response_status status;

	SE_CONTEXT(
		words_info_response,
		words_id_freq
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		words_info_response,
		words_id_freq
	)
};

struct pages_selection_request : context {
	size_t word_id;
	std::unordered_set<size_t> allowed_pages_ids;

	SE_CONTEXT(
		pages_selection_request,
		word_id,
		allowed_pages_ids
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		pages_selection_request,
		word_id,
		allowed_pages_ids
	)
};

struct pages_selection_response : context {
	std::unordered_set<size_t> allowed_pages_ids;
	response_status status;

	SE_CONTEXT(
		pages_selection_response,
		allowed_pages_ids,
		status
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		pages_selection_response,
		allowed_pages_ids,
		status
	)
};

struct page_content_request : context {
	size_t page_id;

	SE_CONTEXT(
		page_content_request,
		page_id
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		page_content_request,
		page_id
	)
};

struct page_content_response : context {
	string_enc url;
	string_enc content;
	response_status status;

	SE_CONTEXT(
		page_content_response,
		url,
		content,
		status
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		page_content_response,
		url,
		content,
		status
	)
};

struct page_relevance_request : context {
	size_t page_id;
	std::vector<size_t> words_ids;

	SE_CONTEXT(
		page_relevance_request,
		page_id,
		words_ids
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		page_relevance_request,
		page_id,
		words_ids
	)
};

struct page_relevance_response : context {
	double rank;
	response_status status;

	SE_CONTEXT(
		page_relevance_response,
		rank,
		status
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		page_relevance_response,
		rank,
		status
	)
};

struct analyzed_page_info_request : context {
	string_enc url;
	string_enc title;
	string_enc snippet;
	double rank;

	SE_CONTEXT(
		analyzed_page_info_request,
		url,
		title,
		snippet,
		rank
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		analyzed_page_info_request,
		url,
		title,
		snippet,
		rank
	)
};

struct analyzed_page_info_response : context {
	response_status status;

	SE_CONTEXT(
		analyzed_page_info_response,
		status
	)

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(
		analyzed_page_info_response,
		status
	)
};