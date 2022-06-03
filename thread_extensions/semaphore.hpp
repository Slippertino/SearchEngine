#pragma once

#include <thread>
#include <mutex>
#include <chrono>
#include <unordered_map>

using namespace std::chrono_literals;

class semaphore
{
	enum class ModificationType 
	{
		ADD,
		ADD_REQUEST,
		REMOVE,
		REMOVE_REQUEST
	};

private:
	static const size_t neutral_zone_id = 0;

	size_t zones_count;
	size_t current_zone = neutral_zone_id;

	std::unordered_map<std::thread::id, size_t> thread_zone_id;
	std::unordered_map<size_t, size_t> zone_id_requests_count;
	std::unordered_map<size_t, size_t> zone_id_count_threads;

	mutable std::mutex mutex;

private:
	void initialize() {
		for (auto i = 1; i <= zones_count; ++i)
			zone_id_requests_count.insert({ i, 0 });

		for (auto i = 1; i <= zones_count; ++i)
			zone_id_count_threads.insert({ i, 0 });
	}

	void edit_zone(ModificationType type, size_t wished_zone = neutral_zone_id) {
		auto id = thread_zone_id[std::this_thread::get_id()];

		switch (type)
		{
		case semaphore::ModificationType::ADD:
			zone_id_count_threads[current_zone]++;
			thread_zone_id[std::this_thread::get_id()] = current_zone;
			break;
		case semaphore::ModificationType::ADD_REQUEST:
			zone_id_requests_count[wished_zone]++;
			break;
		case semaphore::ModificationType::REMOVE:
			zone_id_count_threads[id]--;
			thread_zone_id[std::this_thread::get_id()] = neutral_zone_id;
			break;
		case semaphore::ModificationType::REMOVE_REQUEST:
			zone_id_requests_count[wished_zone]--;
			break;
		default:
			break;
		}
	}

	void analyze_zones_distribution() {
		if (current_zone != neutral_zone_id) {
			auto count = zone_id_count_threads[current_zone];

			if (!count)
				current_zone = neutral_zone_id;
			else
				return;
		}

		size_t new_zone = neutral_zone_id, max = 0;
		for (auto& cur : zone_id_requests_count)
			if (cur.second > max) {
				new_zone = cur.first;
				max = cur.second;
			}

		current_zone = new_zone;
	}

public:
	semaphore() = delete;
	semaphore(size_t z_count) : zones_count(z_count) {
		initialize();
	}

	void connect_thread() {
		std::lock_guard<std::mutex> locker(mutex);
		if (thread_zone_id.find(std::this_thread::get_id()) == thread_zone_id.end())
			thread_zone_id.insert({ std::this_thread::get_id(), neutral_zone_id });
	}

	void disconnect_thread() {
		std::lock_guard<std::mutex> locker(mutex);
		thread_zone_id.erase(std::this_thread::get_id());
	}

	bool is_connected() const {
		std::lock_guard<std::mutex> locker(mutex);
		return thread_zone_id.find(std::this_thread::get_id()) != thread_zone_id.end();
	}

	void enter(size_t zone_id) {
		if (!is_connected()) {
			return;
		}

		mutex.lock();
		if (current_zone == zone_id) {
			edit_zone(ModificationType::ADD);
		}
		else {
			edit_zone(ModificationType::ADD_REQUEST, zone_id);
			analyze_zones_distribution();
			mutex.unlock();

			while (current_zone != zone_id || !mutex.try_lock()) {
				std::this_thread::sleep_for(1ms);
			}

			edit_zone(ModificationType::ADD);

			edit_zone(ModificationType::REMOVE_REQUEST, zone_id);
		}
		mutex.unlock();
	}

	void exit() {
		if (!is_connected()) {
			return;
		}

		std::lock_guard<std::mutex> locker(mutex);
		edit_zone(ModificationType::REMOVE);
		analyze_zones_distribution();
	}

	void clear() {
		std::lock_guard<std::mutex> locker(mutex);
		thread_zone_id        .clear();
		zone_id_requests_count.clear();
		zone_id_count_threads .clear();
	}
};