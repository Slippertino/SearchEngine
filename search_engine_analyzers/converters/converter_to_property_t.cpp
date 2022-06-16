#include "include/converter_to_property_t.h"

template<class TTextProperty>
const std::map<std::string, TTextProperty> converter_to_property_t<TTextProperty>::pattern_to_value_interpreter;

template<class TTextProperty>
const std::map<TTextProperty, std::string> converter_to_property_t<TTextProperty>::value_to_name_interpreter;

template<class TTextProperty>
const TTextProperty converter_to_property_t<TTextProperty>::default_value;