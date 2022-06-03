#pragma once

#include <unordered_map>
#include "thread_safe_container.hpp"

template<typename TKey, typename TValue>
class thread_safe_unordered_map : public thread_safe_container<std::unordered_map, TKey, TValue>
{
private:
	using container = std::unordered_map<TKey, TValue>;
	using tsum_const_iterator = typename container::const_iterator;

public:
	thread_safe_unordered_map() = default;
	thread_safe_unordered_map(const thread_safe_unordered_map& map) : data_source(map.data_source)
	{ }
	thread_safe_unordered_map(thread_safe_unordered_map&& map) {
		data_source = std::move(map.data_source);
	}

	thread_safe_unordered_map(const std::initializer_list<std::pair<TKey, TValue>>& init_list) {
		for (auto& p : init_list)
			data_source.insert(p);
	}

	thread_safe_unordered_map& operator=(const thread_safe_unordered_map& tsum) {
		std::lock_guard<std::mutex> locker(mut);
		
		if (this != &tsum)
			data_source = tsum.data_source;

		return *this;
	}

	tsum_const_iterator cbegin() const {
		std::lock_guard<std::mutex> locker(mut);
		return data_source.cbegin();
	}

	tsum_const_iterator cend() const {
		std::lock_guard<std::mutex> locker(mut);
		return data_source.cend();
	}

	tsum_const_iterator find(const TKey& key) const {
		std::lock_guard<std::mutex> locker(mut);
		return data_source.find(key);
	}

	void insert(const TKey& key, const TValue& value) {
		std::lock_guard<std::mutex> locker(mut);
		data_source.insert({ key, value });
		cv.notify_all();
	}

	void set(const TKey& key, const TValue& value) {
		std::lock_guard<std::mutex> locker(mut);
		data_source[key] = value;
	}

	void wait_and_erase(const TKey& key, TValue& value) {
		std::unique_lock<std::mutex> locker(mut);
		cv.wait(locker, [=]() { return !(data_source.find(key) == data_source.end()); });
		value = std::move(data_source[key]);
		data_source.erase(key);
	}

	TValue wait_and_erase(const TKey& key) {
		std::unique_lock<std::mutex> locker(mut);
		cv.wait(locker, [=]() { return !(data_source.find(key) == data_source.end()); });
		value = std::move(data_source[key]);
		data_source.erase(key);
		return value;
	}

	bool try_erase(const TKey& key, TValue& value) {
		std::lock_guard<std::mutex> locker(mut);
		if (data_source.find(key) == data_source.end())
			return false;
		value = std::move(data_source[key]);
		data_source.erase(key);
		return true;
	}

	void wait_and_get(const TKey& key, TValue& value) const {
		std::unique_lock<std::mutex> locker(mut);
		cv.wait(locker, [=]() { return !(data_source.find(key) == data_source.end()); });
		value = data_source.at(key);
	}

	TValue wait_and_get(const TKey& key) const {
		std::unique_lock<std::mutex> locker(mut);
		cv.wait(locker, [=]() { return !(data_source.find(key) == data_source.end()); });
		return data_source.at(key);
	}

	bool try_get(const TKey& key, TValue& value) const {
		std::lock_guard<std::mutex> locker(mut);
		if (data_source.find(key) == data_source.end())
			return false;
		value = data_source.at(key);
		return true;
	}

	bool try_get(TKey& key, std::shared_ptr<TValue>& value) const {
		std::lock_guard<std::mutex> locker(mut);
		if (data_source.find(key) == data_source.end())
			return false;
		value = std::make_shared<TValue>(data_source.at(key));
		return true;
	}

	void clear() {
		std::lock_guard<std::mutex> locker(mut);
		data_source.clear();
	}
};