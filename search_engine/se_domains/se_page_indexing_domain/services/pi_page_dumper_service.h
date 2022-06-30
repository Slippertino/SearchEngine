#pragma once

#include <tools/se_encoder.hpp>
#include <tools/url_analyzer.hpp>
#include <thread_safe_containers/thread_safe_queue.hpp>
#include <thread_safe_containers/thread_safe_unordered_map.hpp>
#include <semaphore.hpp>
#include "../../se_service.hpp"
#include "../pi_messages.h"
#include "../../se_services_infrastructure/se_services_communication.hpp"
#include <thread_pool.hpp>
#include "../pi_config.hpp"


class pi_page_dumper_service : public se_service<pi_page_dumper_service, pi_config> {
	SE_SERVICE(pi_page_dumper_service)

	#define CHECK_FOR_STOP(link_prefix) if (check_for_stop(link_prefix)) continue;

private:
	thread_safe_queue<std::string> keys;
	thread_safe_unordered_map<std::string, std::pair<std::string, std::string>> source;

	thread_safe_unordered_map<std::string, nullptr_t> cache;

	semaphore semaphore;

	std::mutex record_locker;
	std::mutex mutex;

private:
	void add_to_buffer(const std::pair<std::string, std::string>& link_prefix) {
		if (link_prefix.first.empty() ||
			link_prefix.second.empty()) {
			return;
		}

		std::lock_guard<std::mutex> locker(record_locker);
		if (source.find(link_prefix.first) == source.end()) {
			keys.add(link_prefix.first);
			source.insert(link_prefix.first, link_prefix);

			SE_LOG("Added to buffer: " << link_prefix.first << "\n");
		}
	}

	void get_from_buffer(std::pair<std::string, std::string>& link_prefix) {
		std::string key;

		extract_from_ts_queue(keys, key);
		get_from_ts_unordered_map(source, key, link_prefix);

		SE_LOG("Gotten from buffer: " << "{ " << link_prefix.first << " ; "
			<< link_prefix.second << " }" << "\n");
	}

	void extract_from_buffer(std::pair<std::string, std::string>& link_prefix) {
		extract_from_ts_unordered_map(source, link_prefix.first, link_prefix);

		SE_LOG("Extracted from buffer: " << "{ " << link_prefix.first << " ; "
				<< link_prefix.second << " }" << "\n");
	}

	void add_to_cache(const std::string& url) {
		cache.insert(url, nullptr);

		if (cache.size() > 100000) {
			cache.clear();
		}
	}

	bool check_for_stop(const std::pair<std::string, std::string>& link_prefix) {
		if (stop_flag) {
			semaphore.exit();
			add_to_buffer(link_prefix);
		}

		return stop_flag;
	}

	void page_dumper_handler() {
		semaphore.connect_thread();

		while (!stop_flag) {
			std::pair<std::string, std::string> link_prefix;

			pool.set_inactive(std::this_thread::get_id());
			get_from_buffer(link_prefix);
			CHECK_FOR_STOP(link_prefix)
			pool.set_active(std::this_thread::get_id());

			semaphore.enter(1);
			extract_from_buffer(link_prefix);

			MAKE_REQUEST_WITH_RESPONSE(result, is_unique_page_url, (
				result, 
				string_enc{ link_prefix.first, DEFAULT_ENCODING }
			))
			CHECK_FOR_STOP(link_prefix)

			if (!result.answer ||
				!url_analyzer(link_prefix.first, link_prefix.second).is_valid_url()) {
				semaphore.exit();
				continue;
			}

			if (link_prefix.first == link_prefix.second) {
				MAKE_REQUEST_WITH_RESPONSE(resp, site_recording, (
					resp, 
					string_enc{ link_prefix.first, DEFAULT_ENCODING }
				))
				CHECK_FOR_STOP(link_prefix)
			}

			MAKE_REQUEST_WITH_RESPONSE(page_info, url_to_analyze, (
				page_info, 
				string_enc{ link_prefix.second, DEFAULT_ENCODING },
				string_enc{ link_prefix.first, DEFAULT_ENCODING }
			))
			CHECK_FOR_STOP(link_prefix)

			MAKE_REQUEST_WITH_RESPONSE(record_answer, record_page_info, (
				record_answer, 
				string_enc{ link_prefix.first, DEFAULT_ENCODING },
				page_info.content, 
				page_info.status_code
			))
			CHECK_FOR_STOP(link_prefix)

			semaphore.exit();

			mutex.lock();
			std::cout << link_prefix.first << "\n";
			mutex.unlock();

			SE_LOG(link_prefix.first << " | " << "Size: " << source.size() << " | "
									 << "Concurrency: " << pool.get_active_processes_count() << " | "
									 << "Thread: " << std::this_thread::get_id() << "\n");

			if (!page_info.is_valid)
				continue;

			MAKE_REQUEST(page_indexing_id_resp, page_indexing, (
				string_enc{ link_prefix.first,  DEFAULT_ENCODING },
				string_enc{ link_prefix.second, DEFAULT_ENCODING }
			))

			semaphore.enter(2);
			for (auto s : page_info.linked_urls) {
				se_encoder::encode(s.str, page_info.page_encoding, DEFAULT_ENCODING);
				auto url = url_analyzer(s.str, link_prefix.second).convert_to_url();

				if (cache.find(url) == cache.end()) {
					add_to_buffer({
						url,
						link_prefix.second
					});
					add_to_cache(url);
				}
			}
			semaphore.exit();
		}

		semaphore.disconnect_thread();
	}

protected:
	void setup_base(pi_config* config) override {
		for (auto& url : config->get_sources()) {
			add_to_buffer({ url, url });
		}
	}

	void clear() override {
		se_service<pi_page_dumper_service, pi_config>::clear();
		keys     .clear();
		source   .clear();
		semaphore.clear();
	}

public:
	pi_page_dumper_service() = delete;
	pi_page_dumper_service(size_t id, const fs::path& root, const std::shared_ptr<se_router>& in_router) : 
		se_service(id, root / R"(services)" / get_full_name(get_component_name()), in_router),
		semaphore(2)
	{ }

	std::string get_component_name() const override {
		return std::string("page_dumper_service");
	}
};

template<>
class builder<pi_page_dumper_service> : public abstract_service_builder<pi_page_dumper_service>
{
protected:
	void add_subscriptions(const service_ptr& service) const override {
		service->router->subscribe<url_to_analyze_response>(service);
		service->router->subscribe<record_page_info_response>(service);
		service->router->subscribe<is_unique_page_url_response>(service);
		service->router->subscribe<site_recording_response>(service);
		service->router->subscribe<page_indexing_response>(service);
	}

	void add_power_distribution(const service_ptr& service) const override {
		service->power_distribution.push_back({ &pi_page_dumper_service::page_dumper_handler, 1 });
	}

	void add_request_responders(const service_ptr& service) const override {
		return;
	}

	void add_unused_response_type_names(const service_ptr& service) const override {
		service->unused_response_type_names.insert(typeid(page_indexing_request).name(), nullptr);
	}
};