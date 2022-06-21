#pragma once

#include <string>
#include "automaton.hpp"

class link_automaton final : public automaton<std::string>
{
public:
    link_automaton() = default;

    void link_automaton::update(const GumboNode* node) override final {
        if (!is_node_element(node))
            return;

        GumboAttribute* attr;
        if (node->v.element.tag == GUMBO_TAG_A &&
            (attr = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
            local_value = std::string(attr->value);
        }
    }
};