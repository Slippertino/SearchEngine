#pragma once

#include <string>
#include "automaton.hpp"

class text_automaton final : public automaton<std::string>
{
public:
    text_automaton() = default;

    void text_automaton::update(const GumboNode* node) override final {
        if (is_node_text(node)) {
            local_value = std::string(node->v.text.text);
        }
    }
};