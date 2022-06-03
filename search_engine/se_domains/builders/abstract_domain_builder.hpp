#pragma once

#include <memory>

template<typename domain_t>
class abstract_domain_builder
{
protected:
	using domain_ptr = std::shared_ptr<domain_t>;

protected:
	virtual void add_services(const domain_ptr& service) const = 0;

public:
	template<typename... Args>
	domain_ptr build(Args&&... args) const {
		domain_ptr res = std::make_shared<domain_t>(std::forward<Args>(args)...);

		add_services(res);

		return res;
	}
};