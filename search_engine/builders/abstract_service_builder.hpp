#pragma once

#include <memory>

template<typename service_t>
class abstract_service_builder
{
protected:
	using service_ptr = std::shared_ptr<service_t>;

protected:
	virtual void add_subscriptions(const service_ptr& service) const = 0;
	virtual void add_power_distribution(const service_ptr& service) const = 0;
	virtual void add_request_responders(const service_ptr& service) const = 0;
	virtual void configure_logger(const service_ptr& service) const = 0;

public:
	template<typename... Args>
	service_ptr build(Args&&... args) const {
		service_ptr res = std::make_shared<service_t>(std::forward<Args>(args)...);

		add_subscriptions(res);
		add_power_distribution(res);
		add_request_responders(res);
		configure_logger(res);

		return res;
	}
};