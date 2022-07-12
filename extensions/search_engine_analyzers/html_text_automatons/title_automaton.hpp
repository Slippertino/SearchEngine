#pragma once

#include <string>
#include "automaton.hpp"

class title_automaton final : public automaton<std::string>
{
public:
    title_automaton() = default;

    void update(const GumboNode* node) override final {
        if (node->v.element.tag == GUMBO_TAG_TITLE) {
            local_value = std::string(node->v.text.text);
        }
    }
};

const std::string title_automaton::default_value = "Unknown title";