#pragma once

#include <string>
#include "automaton.h"

class text_automaton final : public automaton<std::string>
{
public:
    text_automaton() = default;

    void update(const GumboNode*) override final;
};