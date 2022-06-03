#pragma once

#include <string>
#include <algorithm>
#include "converter_to_encoding_t.h"
#include "automaton.h"

class encoding_automaton final : public automaton<encoding_t>
{
public:
    encoding_automaton() = default;

    void update(const GumboNode*) override final;
};

