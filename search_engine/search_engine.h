#pragma once

#include "se_component.h"
#include "builders/builder.hpp"
#include "se_domains/se_page_indexing_domain/pi_domain.h"
#include "se_logger/se_loggers_storage.h"
#include "se_logger/se_logger.h"
#include <filesystem>

class search_engine : public se_component
{
	friend class builder<search_engine>;

private:
	static const fs::path logs_root;
	static std::atomic_size_t id_automaton;

	std::shared_ptr<se_component> page_indexing_domain;

protected:
	void wait_for_stop() const {
		do {
			std::this_thread::sleep_for(10s);
		} while (status());
	}

public:
	search_engine() : se_component(++id_automaton,
								   logs_root / fs::path(get_full_name(get_component_name() + std::to_string(rand() % 10000) + "_")))
	{ }

	std::string get_component_name() const override {
		return std::string("search_engine");
	}

	void run(size_t threads_count) override {
		try {
			page_indexing_domain->run(threads_count);
			SE_LOG("Engine has been run with " << threads_count << " flows!\n");
			wait_for_stop();
		}
		catch (const std::exception& ex) {
			SE_LOG("Engine error trying to be run with " << threads_count << " flows! " << ex.what() << "\n");
		}
	}

	void stop() override {
		try {
			page_indexing_domain->stop();
			SE_LOG("Engine has been stopped!\n");
		}
		catch (const std::exception& ex) {
			SE_LOG("Engine error trying to be stopped! " << ex.what() << "\n");
		}
	}
	
	void setup(const configuration& config) override {
		try {
			page_indexing_domain->setup(config);
			SE_LOG("Engine has been setup by " << config.get_dump() << " !\n");
		}
		catch (const std::exception& ex) {
			SE_LOG("Engine error trying to be setup! " << ex.what() << "\n");
		}
	}

	void reset() override {
		try {
			page_indexing_domain->reset();
			SE_LOG("Engine has been reset!\n");
		}
		catch (const std::exception& ex) {
			SE_LOG("Engine error trying to be reset! " << ex.what() << "\n");
		}
	}

	bool status() const override {
		try {
			auto status = page_indexing_domain->status();
			SE_LOG("Engine has returned status " << status << "!\n");
			return status;
		}
		catch (const std::exception& ex) {
			SE_LOG("Engine error trying to return status! " << ex.what() << "\n");
		}
	}

	~search_engine() {
		page_indexing_domain->reset();
	}

};
const fs::path search_engine::logs_root = fs::path(R"(..\..\..)") / fs::path(R"(logs)");
std::atomic_size_t search_engine::id_automaton{ 0 };

template<>
class builder<search_engine> : public abstract_engine_builder<search_engine>
{
protected:
	void add_domains(const engine_ptr& engine) const override {
		engine->page_indexing_domain = builder<pi_domain>().build(engine->id, engine->logger_path);
	}
};