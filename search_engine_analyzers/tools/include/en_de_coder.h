#pragma once

#include <map>
#include <functional>
#include <string>
#include <exception>
#include <windows.h>
#include "converter_to_encoding_t.h"

class en_de_coder
{
private:
	static const UINT internal_code_page;
	static const std::map<encoding_t, std::function<void(const en_de_coder*, std::string&)>> encoding_methods;
	static const std::map<encoding_t, std::function<void(const en_de_coder*, std::string&)>> decoding_methods;
	
	encoding_t external_page_str = encoding_t::UTF_8;

private:
	void to_utf8(std::string&) const;
	void from_utf8(std::string&) const;

	std::wstring to_utf16(std::string&, UINT) const;
	std::string from_utf16(std::wstring&, UINT) const;

public:
	en_de_coder() = default;
	en_de_coder(encoding_t);

	void decode(std::string&) const;
	void encode(std::string&) const;
};