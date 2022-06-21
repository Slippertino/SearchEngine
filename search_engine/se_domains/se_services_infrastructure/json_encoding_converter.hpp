#pragma once

#include <string>
#include <algorithm>
#include <unordered_set>
#include <vector>
#include <tools/en_de_coder.hpp>
#include <text_property_types/string_enc.hpp>


class json_encoding_converter_base
{
protected:
	void encode_base(std::string& s, se_encoding from, se_encoding to) {
		en_de_coder encoder(to), decoder(from);
		decoder.decode(s);
		encoder.encode(s);
	}
};

template <typename T>
class json_encoding_converter : json_encoding_converter_base
{
public:
	void encode(T& obj) {}
	void decode(T& obj) {}
};

template<>
class json_encoding_converter<std::reference_wrapper<string_enc>> : json_encoding_converter_base
{
public:
	void encode(std::reference_wrapper<string_enc> obj) {
		encode_base(obj.get().str, obj.get().enc, encoding_t::UTF_8);
	}

	void decode(std::reference_wrapper<string_enc> obj) {
		encode_base(obj.get().str, encoding_t::UTF_8, obj.get().enc);
	}
};

template<>
class json_encoding_converter<std::reference_wrapper<std::unordered_set<string_enc>>> : json_encoding_converter_base
{
private:
	void exec_encode_wrapper(std::reference_wrapper<std::unordered_set<string_enc>> obj, 
			  const std::function<void(string_enc&)>& op) {
		auto& set = obj.get();

		std::vector<string_enc> buff(
			std::make_move_iterator(set.begin()),
			std::make_move_iterator(set.end())
		);

		std::for_each(buff.begin(), buff.end(), op);

		set = std::unordered_set<string_enc>(
			std::make_move_iterator(buff.begin()),
			std::make_move_iterator(buff.end())
		);
	}

public:
	void encode(std::reference_wrapper<std::unordered_set<string_enc>> obj) {
		exec_encode_wrapper(obj, [&](string_enc& val) {
			encode_base(val.str, val.enc, encoding_t::UTF_8);
		});
	}

	void decode(std::reference_wrapper<std::unordered_set<string_enc>> obj) {
		exec_encode_wrapper(obj, [&](string_enc& val) {
			encode_base(val.str, val.enc, encoding_t::UTF_8);
		});
	}
};