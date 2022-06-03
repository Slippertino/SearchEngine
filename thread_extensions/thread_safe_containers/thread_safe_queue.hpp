#pragma once

#include <queue>
#include "thread_safe_container.hpp"

template<typename T>
class thread_safe_queue : public thread_safe_container<std::queue, T>
{
public:
	thread_safe_queue() = default;

	void add(const T& value) {
		std::lock_guard<std::mutex> locker(mut);
		data_source.push(value);
		cv.notify_one();
	}

	void wait_and_erase(T& value) {
		std::unique_lock<std::mutex> locker(mut);
		cv.wait(locker, [=]() { return !data_source.empty(); });
		value = std::move(data_source.front());
		data_source.pop();
	}

	T wait_and_erase() {
		std::unique_lock<std::mutex> locker(mut);
		cv.wait(locker, [=]() { return !data_source.empty(); });
		T value = std::move(data_source.front());
		data_source.pop();
		return value;
	}

	bool try_erase(T& value) {
		std::lock_guard<std::mutex> locker(mut);
		if (data_source.empty())
			return false;
		value = std::move(data_source.front());
		data_source.pop();
		return true;
	}

	void wait_and_get(T& value) const {
		std::unique_lock<std::mutex> locker(mut);
		cv.wait(locker, [=]() { return !data_source.empty(); });
		value = data_source.front();
	}

	T wait_and_get() const {
		std::unique_lock<std::mutex> locker(mut);
		cv.wait(locker, [=]() { return !data_source.empty(); });
		return data_source.front();
	}

	bool try_get(T& value) const {
		std::lock_guard<std::mutex> locker(mut);
		if (data_source.empty())
			return false;
		value = data_source.front();
		return true;
	}

	void clear() {
		std::lock_guard<std::mutex> locker(mut);
		while (!data_source.empty())
			data_source.pop();
	}
};