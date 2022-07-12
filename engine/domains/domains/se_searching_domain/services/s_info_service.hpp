#pragma once

#include <list>
#include <algorithm>
#include <text_property_types/se_language.hpp>
#include <tools/text_parser.hpp>
#include <tools/stemmer.hpp>
#include <tools/word_analyzer.hpp>
#include <tools/html_text_analyzer.hpp>
#include "../../se_service.hpp"
#include "../s_config.hpp"
#include "../s_messages.hpp"
#include <tools/bold_snippet_builder.hpp>

class s_info_service : public se_service<s_info_service, s_config> {

	SE_SERVICE(s_info_service)

private:
	struct suitable_page_info : context {
		std::string url;
		std::string title;
		std::string snippet;
		double relevance;

		SE_CONTEXT(
			suitable_page_info, 2,
			url,
			title,
			snippet,
			relevance
		)

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(
			suitable_page_info,
			url,
			title,
			snippet,
			relevance
		)
	};

	struct success_search_results : context {
		bool result;
		size_t count;
		std::vector<suitable_page_info> data;

		SE_CONTEXT(
			success_search_results, 2,
			result,
			count,
			data
		)

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(
			success_search_results,
			result,
			count,
			data
		)
	};

	struct unsuccess_search_results : context {
		bool result;
		std::string error;

		SE_CONTEXT(
			unsuccess_search_results, 2,
			result,
			error
		)

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(
			unsuccess_search_results,
			result,
			error
		)
	};

private:
	bool is_search_success = true;

	std::optional<size_t> offset;
	std::optional<size_t> limit;
	std::atomic<double> max_relevance = 0;
	std::atomic<size_t> pages_count   = 0;

	std::optional<std::string> errors;

	std::mutex storage_mutex;
	std::vector<suitable_page_info> storage;


private:
	void analyzed_page_info_request_responder(msg_request msg) {
		auto req = static_cast<analyzed_page_info_request*>(msg.body.get());

		analyzed_page_info_response resp;

		try {
			se_encoder::encode(req->url.str,     req->url.enc,     encoding_t::UTF_8);
			se_encoder::encode(req->title.str,   req->title.enc,   encoding_t::UTF_8);
			se_encoder::encode(req->snippet.str, req->snippet.enc, encoding_t::UTF_8);

			pages_count++;
			max_relevance = std::max(max_relevance.load(), req->rank);

			std::lock_guard<std::mutex> locker(storage_mutex);
			storage.push_back(suitable_page_info{
				req->url.str,
				req->title.str,
				req->snippet.str,
				req->rank
			});

			resp.status.status  = runtime_status::SUCCESS;
			resp.status.message = { req->to_string(), encoding_t::UTF_8 };
		} catch (const std::exception& ex) {
			resp.status.status  = runtime_status::FAIL;
			resp.status.message = { "ANALYZED_PAGE_INFO : " + std::string(ex.what()), encoding_t::UTF_8 };
		}

		MAKE_RESPONSE(analyzed_page_info, (
			msg.id,
			resp
		))
	}

private:

protected:
	void clear() override {
		se_service<s_info_service, s_config>::clear();
		storage.clear();
		offset.reset();
		limit.reset();
		max_relevance = 0;
		pages_count = 0;
	}

	void setup_base(s_config* config) override {
		offset = config->get_offset();
		limit  = config->get_limit();
	}

public:
	s_info_service() = delete;
	s_info_service(size_t id, const fs::path& root, const std::shared_ptr<se_router>& in_router) :
		se_service(id, root / R"(services)" / get_full_name(get_component_name()), in_router)
	{ }

	std::string get_component_name() const override {
		return std::string("info_service");
	}

	std::string get_search_results() {
		while (!stop_flag) { 
			std::this_thread::sleep_for(std::chrono::milliseconds(100); 
		}

		if (errors.)

	}
};

template<>
class builder<s_info_service> : public abstract_service_builder<s_info_service> {
protected:
	void add_subscriptions(const service_ptr& service) const override {
		service->router->subscribe<analyzed_page_info_request>(service);
	}

	void add_power_distribution(const service_ptr& service) const override {
		service->power_distribution.push_back({ &s_info_service::requests_handler,  1 });
	}

	void add_request_responders(const service_ptr& service) const override {
		service->responders.insert(typeid(page_relevance_request).name(),
								   &s_info_service::analyzed_page_info_request_responder);
	}

	void add_unused_response_type_names(const service_ptr& service) const override {
		return;
	}
};