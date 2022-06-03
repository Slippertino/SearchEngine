#include "include/link_automaton.h"

const std::string link_automaton::default_value;
const std::string link_automaton::forbidden_value;

void link_automaton::update(const GumboNode* node) {
    if (!is_node_element(node))
        return;

    GumboAttribute* attr;
    if (node->v.element.tag == GUMBO_TAG_A &&
        (attr = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
        local_value = std::string(attr->value);
    }
}