#pragma once

#include <mutex>
#include <sstream>

#undef max()

class message_id_generator {
private:
	std::mutex mutex;
	size_t unique_id = 0;

private:
	void change_id() {
		std::lock_guard<std::mutex> locker(mutex);
		if (unique_id == std::numeric_limits<size_t>::max())
			unique_id = 0;
		else
			++unique_id;
	}

public:
	message_id_generator() = default;

	auto generate(const std::initializer_list<std::string>& attrs)  {
		std::ostringstream ostr;

		for (auto& cur : attrs)
			ostr << cur;

		ostr << std::to_string(unique_id);

		change_id();

		return ostr.str();
	}
};