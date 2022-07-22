#pragma once

#include <core/se_service.hpp>
#include <core/builders/se_service_builder.hpp>
#include <search_engine_analyzers/tools/se_encoder.hpp>
#include <search_engine_analyzers/tools/url_analyzer.hpp>
#include <thread_extensions/thread_safe_containers/thread_safe_queue.hpp>
#include <thread_extensions/thread_safe_containers/thread_safe_unordered_map.hpp>
#include <thread_extensions/semaphore.hpp>
#include "../messages/pi_internal_messages.hpp"
#include "../pi_config.hpp"

class pi_page_dumper_service : public se_service<pi_page_dumper_service, pi_config> {
private:
	SE_SERVICE(pi_page_dumper_service, page_dumper_service)

	#define CHECK_FOR_STOP(link_prefix) if (check_for_stop(link_prefix)) continue;

private:
	indexing_modes ind_mode;

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
		ind_mode = config->get_indexing_mode();

		for (auto& url : config->get_sources()) {

			auto enc = config->get_encoding();

			url_analyzer u_a(enc);

			if (u_a.is_valid_url(url)) {
				auto root = u_a.get_root(url);

				se_encoder::encode(url,  enc, DEFAULT_ENCODING);
				se_encoder::encode(root, enc, DEFAULT_ENCODING);

				add_to_buffer({ url, root });
			}
		}
	}

	void clear() override {
		se_service<pi_page_dumper_service, pi_config>::clear();
		keys     .clear();
		source   .clear();
		semaphore.clear();
	}
};

template<>
class se_builder_imp<pi_page_dumper_service> : public se_service_builder<pi_page_dumper_service>
{
protected:
	void configure_own_network(const object_ptr& service) const override final {
		service
			->subscribe(service->internal_switch, typeid(page_info_delete_request).name())
			->subscribe(service->internal_switch, typeid(url_to_analyze_request).name())
			->subscribe(service->internal_switch, typeid(record_page_info_request).name())
			->subscribe(service->internal_switch, typeid(is_unique_page_url_request).name())
			->subscribe(service->internal_switch, typeid(site_url_recording_request).name())
			->subscribe(service->internal_switch, typeid(page_indexing_request).name());
	}

	void configure_switches_network(const object_ptr& service) const override final {
		service->internal_switch
			->subscribe(service, typeid(page_info_delete_response).name())
			->subscribe(service, typeid(url_to_analyze_response).name())
			->subscribe(service, typeid(record_page_info_response).name())
			->subscribe(service, typeid(is_unique_page_url_response).name())
			->subscribe(service, typeid(site_url_recording_response).name())
			->subscribe(service, typeid(page_indexing_response).name());
	}

	void add_request_responders(const object_ptr& service) const override final {
		return;
	}

	void add_unused_response_type_names(const object_ptr& service) const override final {
		service->unused_response_type_names = {
			{ typeid(page_indexing_request).name(), nullptr },
		};
	}

	void add_power_distribution(const object_ptr& service) const override final {
		service->power_distribution = {
			{ &pi_page_dumper_service::page_dumper_handler, 1 }
		};
	}
};