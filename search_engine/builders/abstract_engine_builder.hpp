#pragma once

#include <memory>

template<typename engine_t>
class abstract_engine_builder
{
protected:
	using engine_ptr = std::shared_ptr<engine_t>;

protected:
	virtual void add_domains(const engine_ptr& engine) const = 0;
	virtual void configure_logger(const engine_ptr& engine) const = 0;

public:
	template<typename... Args>
	engine_ptr build(Args&&... args) const {
		engine_ptr res = std::make_shared<engine_t>(std::forward<Args>(args)...);

		configure_logger(res);
		add_domains(res);

		return res;
	}
};