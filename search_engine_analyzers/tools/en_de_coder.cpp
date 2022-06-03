#include "include/en_de_coder.h"

const UINT en_de_coder::internal_code_page = CP_ACP;

const std::map<encoding_t, std::function<void(const en_de_coder*, std::string&)>> en_de_coder::encoding_methods = {
	{encoding_t::UTF_8, [](const en_de_coder* encoder, std::string& s) { encoder->to_utf8(s); }}
};

const std::map<encoding_t, std::function<void(const en_de_coder*, std::string&)>> en_de_coder::decoding_methods = {
	{encoding_t::UTF_8, [](const en_de_coder* decoder, std::string& s) { decoder->from_utf8(s); }}
};

en_de_coder::en_de_coder(encoding_t encoding) : external_page_str(encoding)
{ }

std::wstring en_de_coder::to_utf16(std::string& s, UINT code_page) const {
	int size = MultiByteToWideChar(code_page, MB_COMPOSITE, s.c_str(), s.length(), nullptr, 0);
	std::wstring utf16_s(size, '\0');

	MultiByteToWideChar(code_page, MB_COMPOSITE, s.c_str(), s.length(), &utf16_s[0], size);

	return utf16_s;
}

std::string en_de_coder::from_utf16(std::wstring& s, UINT code_page) const {
	int size = WideCharToMultiByte(code_page, 0, s.c_str(), s.length(), nullptr, 0, nullptr, nullptr);
	std::string decoded_s(size, '\0');

	WideCharToMultiByte(code_page, 0, s.c_str(), s.length(), &decoded_s[0], size, nullptr, nullptr);

	return decoded_s;
}

void en_de_coder::from_utf8(std::string& utf8_s) const {
	std::wstring utf16_s = to_utf16(utf8_s, CP_UTF8);
	utf8_s = from_utf16(utf16_s, internal_code_page);
}

void en_de_coder::to_utf8(std::string& s) const {
	std::wstring utf16_s = to_utf16(s, internal_code_page);
	s = from_utf16(utf16_s, CP_UTF8);
}

void en_de_coder::decode(std::string& s) const {
	try {
		decoding_methods.at(external_page_str)(this, s);
	}
	catch (std::out_of_range) {
		throw std::exception("Unknown decoding type in <en_de_coder>");
	}
}

void en_de_coder::encode(std::string& s) const {
	try {
		encoding_methods.at(external_page_str)(this, s);
	}
	catch (std::out_of_range) {
		throw std::exception("Unknown encoding type in <en_de_coder>");
	}
}