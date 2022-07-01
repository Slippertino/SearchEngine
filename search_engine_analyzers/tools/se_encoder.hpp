#pragma once

#include <map>
#include <functional>
#include <string>
#include <exception>
#include <windows.h>
#include "se_pair_extension.hpp"
#include "../text_property_types/se_encoding.hpp"

class se_encoder {
private:
	static const std::map<std::pair<se_encoding, se_encoding>, std::function<void(std::string&)>> encoding_methods;

private:
	static std::wstring to_utf16(std::string& s, UINT code_page) {
		int size = MultiByteToWideChar(code_page, MB_COMPOSITE, s.c_str(), s.length(), nullptr, 0);
		std::wstring utf16_s(size, '\0');

		MultiByteToWideChar(code_page, MB_COMPOSITE, s.c_str(), s.length(), &utf16_s[0], size);

		return utf16_s;
	}

	static std::string from_utf16(std::wstring& s, UINT code_page) {
		int size = WideCharToMultiByte(code_page, 0, s.c_str(), s.length(), nullptr, 0, nullptr, nullptr);
		std::string decoded_s(size, '\0');

		WideCharToMultiByte(code_page, 0, s.c_str(), s.length(), &decoded_s[0], size, nullptr, nullptr);

		return decoded_s;
	}

	static void from_utf8_to_ansi(std::string& s) {
		std::wstring utf16_s = to_utf16(s, CP_UTF8);
		s = from_utf16(utf16_s, CP_ACP);
	}

	static void from_ansi_to_utf8(std::string& s) {
		std::wstring utf16_s = to_utf16(s, CP_ACP);
		s = from_utf16(utf16_s, CP_UTF8);
	}

public:
	static se_encoder& encode(std::string& s, se_encoding from, se_encoding to) {
		try {
			encoding_methods.at({ from, to })(s);
		}
		catch (std::out_of_range) {
			throw std::exception("Unknown encoding type in <en_de_coder>");
		}
	}
};

const std::map<std::pair<se_encoding, se_encoding>, std::function<void(std::string&)>> se_encoder::encoding_methods = {
	{ std::pair(encoding_t::ANSI,    encoding_t::ANSI),     [](std::string&) {}              },
	{ std::pair(encoding_t::ANSI,    encoding_t::UTF_8),    &se_encoder::from_ansi_to_utf8   },
	{ std::pair(encoding_t::UTF_8,   encoding_t::ANSI),     &se_encoder::from_utf8_to_ansi   },
	{ std::pair(encoding_t::UTF_8,   encoding_t::UTF_8),    [](std::string&) {}              },
};