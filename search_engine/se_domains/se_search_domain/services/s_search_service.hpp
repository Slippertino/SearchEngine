#include "../../se_service.hpp"

class s_search_service : public se_service<s_search_service> {
	SE_SERVICE(s_search_service)
private:

protected:
	void clear() override {
		se_service<s_search_service>::clear();

	}

public:
	s_search_service() = delete;
	s_search_service(size_t id, const fs::path& root, const std::shared_ptr<se_router>& in_router) :
		se_service(id, root / R"(services)" / get_full_name(get_component_name()), in_router)
	{ }

	std::string get_component_name() const override {
		return std::string("search_service");
	}

	void setup(const configuration& config) override {
		try {
			//TO_DO
			SE_LOG("Successful setup!\n");
		} catch (const std::exception& ex) {
			SE_LOG("Unsuccessful setup! " << ex.what() << "\n");
			throw ex;
		}
	}
};

template<>
class builder<s_search_service> : public abstract_service_builder<s_search_service> {
protected:
	void add_subscriptions(const service_ptr& service) const override {

	}

	void add_power_distribution(const service_ptr& service) const override {

	}

	void add_request_responders(const service_ptr& service) const override {

	}

	void add_unused_response_type_names(const service_ptr& service) const override {

	}
};