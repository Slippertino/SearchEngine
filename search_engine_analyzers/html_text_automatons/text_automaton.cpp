#include "include/text_automaton.h"

const std::string text_automaton::default_value;
const std::string text_automaton::forbidden_value;

void text_automaton::update(const GumboNode* node) {
    if (is_node_text(node))
        local_value = std::string(node->v.text.text);
}