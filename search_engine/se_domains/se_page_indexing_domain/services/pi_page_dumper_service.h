#pragma once

#include <en_de_coder.h>
#include <thread_safe_containers/thread_safe_queue.hpp>
#include <thread_safe_containers/thread_safe_unordered_map.hpp>
#include <semaphore.hpp>
#include "../../se_service.hpp"
#include "../messages.h"
#include "../../se_services_infrastructure/se_services_communication.hpp"
#include <thread_pool.hpp>

class pi_page_dumper_service : public se_service<pi_page_dumper_service>
{
	SE_SERVICE(pi_page_dumper_service)

	#define CHECK_FOR_STOP(link_prefix) if (check_for_stop(link_prefix)) continue;

private:

	thread_safe_queue<std::string> keys;
	thread_safe_unordered_map<std::string, std::pair<std::string, std::string>> source;

	semaphore semaphore;

	std::mutex cout_locker;

	std::fstream errors;

private:
	void add_to_buffer(const std::pair<std::string, std::string>& link_prefix) {
		static std::mutex record_locker;
		std::pair<std::string, std::string> l_p;
		
		if (!source.try_get(link_prefix.first, l_p) && record_locker.try_lock()) {
			if (link_prefix.first.empty()) {
				record_locker.unlock();
				return;
			}

			keys.add(link_prefix.first);

			if (!link_prefix.second.empty()) {
				source.insert(link_prefix.first, link_prefix);
			}

			record_locker.unlock();
		}
	}

	void get_from_buffer(std::pair<std::string, std::string>& link_prefix) {
		std::string key;

		extract_from_ts_queue(keys, key);
		extract_from_ts_unordered_map(source, key, link_prefix);
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

			MAKE_REQUEST_WITH_RESPONSE(result, is_unique_page_url, 
				(result, link_prefix.first)) 
			CHECK_FOR_STOP(link_prefix)

			if (!result.answer || !is_valid_link(link_prefix.second, link_prefix.first)) {
				semaphore.exit();
				continue;
			}

			if (link_prefix.first == link_prefix.second) {
				MAKE_REQUEST_WITH_RESPONSE(resp, site_recording, 
					(resp, link_prefix.first))
				CHECK_FOR_STOP(link_prefix)
			}

			MAKE_REQUEST_WITH_RESPONSE(page_info, url_to_analyze, 
				(page_info, link_prefix.second, link_prefix.first))
			CHECK_FOR_STOP(link_prefix)

			en_de_coder coder;
			coder.encode(link_prefix.first);

			MAKE_REQUEST_WITH_RESPONSE(record_answer, record_page_info, 
				(record_answer, link_prefix.first, page_info.content, page_info.status_code))
			CHECK_FOR_STOP(link_prefix)

			if (record_answer.status.status == runtime_status::FAIL)
			{
				cout_locker.lock();
				errors << link_prefix.first << std::endl
					   << record_answer.status.message << std::endl
					   << page_info.content << std::endl;
				cout_locker.unlock();
			}

			semaphore.exit();

			cout_locker.lock();
			std::cout << link_prefix.first << " | "
				<< "Size: " << source.size() << " | "
				<< "Concurrency: " << pool.get_active_processes_count() << " | "
				<< "Thread: " << std::this_thread::get_id() << std::endl;
			cout_locker.unlock();

			if (!page_info.is_valid)
				continue;

			semaphore.enter(2);
			for (auto s : page_info.linked_urls) {
				coder.decode(s);
				to_url(link_prefix.second, s);
				add_to_buffer({ s, link_prefix.second });
			}
			semaphore.exit();
		}

		semaphore.disconnect_thread();
	}

	std::string get_protocol(const std::string& url) const {
		return url.substr(0, url.find("://"));
	}

	void to_url(const std::string& prefix, std::string& link) const {
		if (link.find(get_protocol(prefix))) {
			auto p = link.find('/');
 
			if (p || link.size() == 1)
				link = "";

			link = prefix + link;
		}
	}

	bool is_valid_link(const std::string& prefix, std::string& link) {
		const std::string forbidden_descriptors = "#?=&@%";

		for (auto& ch : link)
			if (forbidden_descriptors.find(ch) != std::string::npos)
				return false;

		if (link.find(prefix) != 0)
			return false;

		return true;
	}

	void load_urls(const std::vector<std::string>& urls) {
		for (auto& url : urls) 
			add_to_buffer({ url, url });
	}

protected:
	void clear() override {
		keys     .clear();
		source   .clear();
		semaphore.clear();
	}

public:
	pi_page_dumper_service() = delete;
	pi_page_dumper_service(const std::shared_ptr<se_router>& in_router) : se_service(in_router),
															              semaphore(2),
																		  errors("D:\\Skillbox\\C++ dev\\SearchEngine\\SearchEngine\\search_engine\\se_domains\\se_page_indexing_domain\\services\\errors.txt")
	{ }

	void setup(const configuration& config) override {
		load_urls(config.get_sources());
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
	}

	void add_power_distribution(const service_ptr& service) const override {
		service->power_distribution.push_back({ &pi_page_dumper_service::page_dumper_handler, 1 });
	}

	void add_request_responders(const service_ptr& service) const override {
		return;
	}
};