#pragma once

#include <memory>
#include "se_builder_imp.hpp"

template<class object_t>
class se_builder {
protected:
	using object_ptr = object_t*;

public:
	virtual void build(object_ptr object) = 0;
};