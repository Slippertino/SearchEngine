#pragma once

#include <map>
#include <functional>
#include <string>
#include <exception>
#include <windows.h>
#include "../text_property_types/se_encoding.hpp"

class en_de_coder
{
private:
	static const se_encoding internal_encoding;
	static const std::map<se_encoding, std::function<void(const en_de_coder*, std::string&)>> encoding_methods;
	static const std::map<se_encoding, std::function<void(const en_de_coder*, std::string&)>> decoding_methods;

	se_encoding external_encoding;

private:
	std::wstring to_utf16(std::string& s, UINT code_page) const {
		int size = MultiByteToWideChar(code_page, MB_COMPOSITE, s.c_str(), s.length(), nullptr, 0);
		std::wstring utf16_s(size, '\0');

		MultiByteToWideChar(code_page, MB_COMPOSITE, s.c_str(), s.length(), &utf16_s[0], size);

		return utf16_s;
	}

	std::string from_utf16(std::wstring& s, UINT code_page) const {
		int size = WideCharToMultiByte(code_page, 0, s.c_str(), s.length(), nullptr, 0, nullptr, nullptr);
		std::string decoded_s(size, '\0');

		WideCharToMultiByte(code_page, 0, s.c_str(), s.length(), &decoded_s[0], size, nullptr, nullptr);

		return decoded_s;
	}

	void from_utf8(std::string& utf8_s) const {
		std::wstring utf16_s = to_utf16(utf8_s, CP_UTF8);
		utf8_s = from_utf16(utf16_s, CP_ACP);
	}

	void to_utf8(std::string& s) const {
		std::wstring utf16_s = to_utf16(s, CP_ACP);
		s = from_utf16(utf16_s, CP_UTF8);
	}

public:
	en_de_coder() = delete;
	en_de_coder(se_encoding encoding) : external_encoding(encoding)
	{ }

	void decode(std::string& s) const {
		try {
			decoding_methods.at(external_encoding)(this, s);
		} catch (std::out_of_range) {
			throw std::exception("Unknown decoding type in <en_de_coder>");
		}
	}

	void encode(std::string& s) const {
		try {
			encoding_methods.at(external_encoding)(this, s);
		} catch (std::out_of_range) {
			throw std::exception("Unknown encoding type in <en_de_coder>");
		}
	}
};

const se_encoding en_de_coder::internal_encoding = encoding_t::ANSI;

const std::map<se_encoding, std::function<void(const en_de_coder*, std::string&)>> en_de_coder::encoding_methods = {
	{encoding_t::UTF_8,    [](const en_de_coder* encoder, std::string& s) { encoder->to_utf8(s); }},
	{encoding_t::ANSI,     [](const en_de_coder* encoder, std::string& s) {}},
	{encoding_t::UNKNOWN,  [](const en_de_coder* encoder, std::string& s) {}},
};

const std::map<se_encoding, std::function<void(const en_de_coder*, std::string&)>> en_de_coder::decoding_methods = {
	{encoding_t::UTF_8,    [](const en_de_coder* decoder, std::string& s) { decoder->from_utf8(s); }},
	{encoding_t::ANSI,     [](const en_de_coder* decoder, std::string& s) {}},
	{encoding_t::UNKNOWN,  [](const en_de_coder* decoder, std::string& s) {}},
};