#pragma once

#include <thread>
#include <unordered_map>
#include <mutex>

class thread_pool
{
protected:
	enum class work_status
	{
		ACTIVE,
		INACTIVE
	};

protected:
	std::unordered_map<std::thread::id, std::pair<std::unique_ptr<std::thread>, work_status>> threads_table;

	size_t threads_count = 1;

	std::mutex table_mutex;

public:
	thread_pool() = default;
	thread_pool(size_t th_count) : threads_count(th_count)
	{ }

	thread_pool& operator= (const thread_pool& pool) {
		if (this == &pool)
			return *this;

		threads_count = pool.threads_count;
	}

	thread_pool(const thread_pool& pool) = delete;

	bool is_full() const {
		return threads_count == threads_table.size();
	}

	bool is_work_finished() const {
		bool res = true;

		for (auto& cur : threads_table)
			res &= static_cast<bool>(cur.second.second);

		return res;
	}

	size_t get_active_processes_count() const {
		size_t res = 0;

		for (auto& cur : threads_table)
			if (cur.second.second == work_status::ACTIVE)
				res++;

		return res;
	}

	size_t size() const {
		return threads_count;
	}

	void set_active(const std::thread::id& id) {
		std::lock_guard<std::mutex> locker(table_mutex);
		threads_table[id].second = work_status::ACTIVE;
	}

	void set_inactive(const std::thread::id& id) {
		std::lock_guard<std::mutex> locker(table_mutex);
		threads_table[id].second = work_status::INACTIVE;
	}

	template<typename Fn, typename... Args>
	std::thread::id add_task(Fn foo, Args... args) {
		std::lock_guard<std::mutex> locker(table_mutex);
		std::unique_ptr<std::thread> new_th = std::make_unique<std::thread>(foo, args...);

		auto id = new_th.get()->get_id();

		threads_table.insert({ new_th->get_id(), std::make_pair(std::move(new_th), work_status::ACTIVE) });

		return id;
	}

	void clear() {
		std::lock_guard<std::mutex> locker(table_mutex);
		for (auto& cur : threads_table) {
			if (cur.second.first != nullptr) {
				cur.second.first->join();
			}
		}
		threads_table.clear();
	}
};