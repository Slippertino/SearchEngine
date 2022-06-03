#pragma once

#include <string>
#include "converter_to_language_t.h"
#include "automaton.h"

class language_automaton final : public automaton<language_t>
{
public:
	language_automaton() = default;

	void update(const GumboNode*) override final;
};