#pragma once

#include <memory>

template<typename engine_t>
class abstract_engine_builder
{
protected:
	using engine_ptr = std::shared_ptr<engine_t>;

protected:
	virtual void add_domains(const engine_ptr& engine) const = 0;

protected:
	void configure_logger(const std::shared_ptr<se_component>& engine) const {
		auto logger = std::make_shared<se_logger>(engine->get_path(), ".txt");
		auto name = engine->get_component_name();

		logger->add_file(
			se_logger::get_code(name),
			name + std::string("_process"),
			engine->get_path()
		);
		se_loggers_storage::get_instance()->set_logger(engine->get_id(), logger);
	}

public:
	template<typename... Args>
	engine_ptr build(Args&&... args) const {
		engine_ptr res = std::make_shared<engine_t>(std::forward<Args>(args)...);

		configure_logger(res);
		add_domains(res);

		return res;
	}
};