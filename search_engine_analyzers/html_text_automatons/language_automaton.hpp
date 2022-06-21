#pragma once

#include <string>
#include "automaton.hpp"
#include "text_property_types/se_language.hpp"

class language_automaton final : public automaton<se_language>
{
public:
    language_automaton() = default;

    void update(const GumboNode* node) override final {
        if (!is_node_element(node))
            return;
        auto r = se_language(language_t::ENGLISH);
        GumboAttribute* attr;
        if (attr = gumbo_get_attribute(&node->v.element.attributes, "lang")) {
            local_value = se_language(std::string(attr->value).substr(0, 2));
        }
    }
};

const se_language language_automaton::default_value = language_t::ENGLISH;
const se_language language_automaton::forbidden_value = language_t::UNKNOWN;