#pragma once

#include <string>

template<class domain_t, class config_t>
class se_cfg_validator {
protected:
	using domain_ptr = const domain_t*;
	using config_ptr = const config_t*;

public:
	virtual bool is_valid(domain_ptr domain, config_ptr config, std::string& error_msg) const = 0;
};