#pragma once

#include <string>
#include "automaton.h"

class link_automaton final : public automaton<std::string>
{
public:
    link_automaton() = default;

    void update(const GumboNode*) override final;
};