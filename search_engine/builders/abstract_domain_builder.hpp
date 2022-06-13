#pragma once

#include <memory>

template<typename domain_t>
class abstract_domain_builder
{
protected:
	using domain_ptr = std::shared_ptr<domain_t>;

protected:
	virtual void add_services(const domain_ptr& domain) const = 0;

protected:
	void configure_logger(const std::shared_ptr<se_component>& domain) const {
		auto logger = se_loggers_storage::get_instance()->get_logger(domain->get_id());
		auto name = domain->get_component_name();

		logger->add_file(
			se_logger::get_code(name),
			name + std::string("_process"),
			domain->get_path()
		);
	}

public:
	template<typename... Args>
	domain_ptr build(Args&&... args) const {
		domain_ptr res = std::make_shared<domain_t>(std::forward<Args>(args)...);

		configure_logger(res);
		add_services(res);

		return res;
	}
};