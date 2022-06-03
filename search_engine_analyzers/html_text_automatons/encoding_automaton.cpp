#include "include/encoding_automaton.h"

const encoding_t encoding_automaton::default_value   = encoding_t::UTF_8;
const encoding_t encoding_automaton::forbidden_value = encoding_t::UNKNOWN;

void encoding_automaton::update(const GumboNode* node) {
    if (!is_node_element(node))
        return;

    GumboAttribute* attr;
    if (node->v.element.tag == GUMBO_TAG_META) {
        if (attr = gumbo_get_attribute(&node->v.element.attributes, "charset")) {
            local_value = converter_to_encoding_t().get_value(std::string(attr->value));
        }
        else if (attr = gumbo_get_attribute(&node->v.element.attributes, "content")) {
            auto cont = std::string(attr->value);
            auto key = "charset=";

            std::remove(cont.begin(), cont.end(), ' ');

            if (cont.find(key) != std::string::npos)
                local_value = converter_to_encoding_t().get_value(cont.substr(cont.find(key) + strlen(key)));
        }
    }
}