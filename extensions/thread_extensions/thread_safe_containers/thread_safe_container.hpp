#pragma once

#include <mutex>
#include <condition_variable>

template<template<typename... c_types> class cont, typename... types>
class thread_safe_container
{
protected:
	mutable std::mutex mut;
	mutable std::condition_variable cv;

	cont<types...> data_source;

public:
	thread_safe_container() = default;
	thread_safe_container(const thread_safe_container& cont) : data_source(cont.data_source)
	{ }

	virtual bool empty() const {
		std::lock_guard<std::mutex> locker(mut);
		return data_source.empty();
	}

	virtual size_t size() const {
		std::lock_guard<std::mutex> locker(mut);
		return data_source.size();
	}
};