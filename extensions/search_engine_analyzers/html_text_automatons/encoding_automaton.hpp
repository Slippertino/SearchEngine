#pragma once

#include <string>
#include <algorithm>
#include "automaton.hpp"
#include "../text_property_types/se_encoding.hpp"

class encoding_automaton final : public automaton<se_encoding>
{
public:
    encoding_automaton() = default;

    void encoding_automaton::update(const GumboNode* node) override final {
        if (!is_node_element(node)) {
            return;
        }

        GumboAttribute* attr;
        if (node->v.element.tag == GUMBO_TAG_META) {
            if (attr = gumbo_get_attribute(&node->v.element.attributes, "charset")) {
                local_value = se_encoding(std::string(attr->value));
            }
            else if (attr = gumbo_get_attribute(&node->v.element.attributes, "content")) {
                auto cont = std::string(attr->value);
                auto key = "charset=";

                std::remove(cont.begin(), cont.end(), ' ');

                if (cont.find(key) != std::string::npos) {
                    local_value = se_encoding(cont.substr(cont.find(key) + strlen(key)));
                }
            }
        }
    }
};

const se_encoding encoding_automaton::default_value   = encoding_t::UTF_8;
const se_encoding encoding_automaton::forbidden_value = encoding_t::UNKNOWN;