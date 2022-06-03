#include "include/language_automaton.h"

const language_t language_automaton::default_value   = language_t::ENGLISH;
const language_t language_automaton::forbidden_value = language_t::UNKNOWN;

void language_automaton::update(const GumboNode* node) {
    if (!is_node_element(node))
        return;

    GumboAttribute* attr;
    if (attr = gumbo_get_attribute(&node->v.element.attributes, "lang")) {
        local_value = converter_to_language_t().get_value(std::string(attr->value).substr(0, 2));
    }
}