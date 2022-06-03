#pragma once

#include "gumbo.h"

template<class T>
class automaton
{
protected:
    static const T default_value;
    static const T forbidden_value;

protected:
    T local_value = forbidden_value;

public:
    automaton() = default;

    bool is_node_text(const GumboNode* node) const {
        return node->type == GUMBO_NODE_TEXT;
    }

    bool is_node_element(const GumboNode* node) const {
        return node->type == GUMBO_NODE_ELEMENT;
    }

    T get() const {
        return (local_value == forbidden_value)
            ? default_value
            : local_value;
    }

    virtual void update(const GumboNode*) = 0;

    virtual ~automaton() = default;
};