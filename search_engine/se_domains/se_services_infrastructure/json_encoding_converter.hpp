#pragma once

#include <string>
#include <algorithm>
#include <unordered_set>
#include <vector>
#include <tools/se_encoder.hpp>
#include <text_property_types/string_enc.hpp>

#define JSON_ENCODING encoding_t::UTF_8

template<typename T>
class json_encoding_converter {
public:
	void encode(std::reference_wrapper<T> obj) {}
	void decode(std::reference_wrapper<T> obj) {}
};

template<typename T>
class json_encoding_converter<std::reference_wrapper<T>> {
public:
	void encode(std::reference_wrapper<T> obj) {
		json_encoding_converter<T>().encode(obj);
	}

	void decode(std::reference_wrapper<T> obj) {
		json_encoding_converter<T>().decode(obj);
	}
};

template<>
class json_encoding_converter<string_enc> {
public:
	void encode(std::reference_wrapper<string_enc> obj) {
		se_encoder::encode(obj.get().str, obj.get().enc, JSON_ENCODING);
	}

	void decode(std::reference_wrapper<string_enc> obj) {
		se_encoder::encode(obj.get().str, JSON_ENCODING, obj.get().enc);
	}
};

template<typename T>
class json_encoding_converter<std::unordered_set<T>> {
private:
	void exec_encode_wrapper(std::reference_wrapper<std::unordered_set<T>> obj, 
							 const std::function<void(T&)>& op) {
		auto& set = obj.get();

		std::vector<T> buff(
			std::make_move_iterator(set.begin()),
			std::make_move_iterator(set.end())
		);

		std::for_each(buff.begin(), buff.end(), op);

		set = std::unordered_set<T>(
			std::make_move_iterator(buff.begin()),
			std::make_move_iterator(buff.end())
		);
	}

public:
	void encode(std::reference_wrapper<std::unordered_set<T>> obj) {
		exec_encode_wrapper(obj, [&](T& val) {
			json_encoding_converter<T>().encode(std::ref(val));
		});
	}

	void decode(std::reference_wrapper<std::unordered_set<T>> obj) {
		exec_encode_wrapper(obj, [&](T& val) {
			json_encoding_converter<T>().decode(std::ref(val));
		});
	}
};

template<typename T>
class json_encoding_converter<std::vector<T>> {
private:
	void exec_encode_wrapper(std::reference_wrapper<std::vector<T>> obj,
							 const std::function<void(T&)>& op) {
		auto& vec = obj.get();
		std::for_each(vec.begin(), vec.end(), op);
	}

public:
	void encode(std::reference_wrapper<std::vector<T>> obj) {
		exec_encode_wrapper(obj, [&](T& val) {
			json_encoding_converter<T>().encode(std::ref(val));
		});
	}

	void decode(std::reference_wrapper<std::vector<T>> obj) {
		exec_encode_wrapper(obj, [&](T& val) {
			json_encoding_converter<T>().decode(std::ref(val));
		});
	}
};

template<typename T1, typename T2>
class json_encoding_converter<std::pair<T1, T2>> {
public:
	void encode(std::reference_wrapper<std::pair<T1, T2>> obj) {
		json_encoding_converter<T1>().encode(std::ref(obj.get().first));
		json_encoding_converter<T2>().encode(std::ref(obj.get().second));
	}

	void decode(std::reference_wrapper<std::pair<T1, T2>> obj) {
		json_encoding_converter<T1>().decode(std::ref(obj.get().first));
		json_encoding_converter<T2>().decode(std::ref(obj.get().second));
	}
};

template<typename T1, typename T2, typename T3>
class json_encoding_converter<std::tuple<T1, T2, T3>> {
public:
	void encode(std::reference_wrapper<std::tuple<T1, T2, T3>> obj) {
		json_encoding_converter<T1>().encode(std::ref(std::get<0>(obj.get())));
		json_encoding_converter<T2>().encode(std::ref(std::get<1>(obj.get())));
		json_encoding_converter<T3>().encode(std::ref(std::get<2>(obj.get())));
	}

	void decode(std::reference_wrapper<std::tuple<T1, T2, T3>> obj) {
		json_encoding_converter<T1>().decode(std::ref(std::get<0>(obj.get())));
		json_encoding_converter<T2>().decode(std::ref(std::get<1>(obj.get())));
		json_encoding_converter<T3>().decode(std::ref(std::get<2>(obj.get())));
	}
};