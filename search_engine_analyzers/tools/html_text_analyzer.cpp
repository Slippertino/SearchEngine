#include "include/html_text_analyzer.h"

const std::unordered_set<GumboTag> html_text_analyzer::forbidden_tags = {
    GUMBO_TAG_SCRIPT,
    GUMBO_TAG_STYLE,
    GUMBO_TAG_FOOTER,
    GUMBO_TAG_NOSCRIPT,
    GUMBO_TAG_BUTTON,
};

const std::unordered_map<GumboTag, html_text_analyzer::ratio_type> html_text_analyzer::tag_text_priority = {
    {GUMBO_TAG_TITLE, 10.0},
    {GUMBO_TAG_H1,    5.0},
    {GUMBO_TAG_H2,    5.0},
    {GUMBO_TAG_H3,    5.0},
    {GUMBO_TAG_H4,    3.0},
    {GUMBO_TAG_H5,    3.0},
    {GUMBO_TAG_H6,    3.0},
    {GUMBO_TAG_TD,    0.5}
};

const html_text_analyzer::ratio_type html_text_analyzer::default_tag_text_priority = 1.0;

html_text_analyzer::html_text_analyzer(const std::string& html_str) : output(gumbo_parse(html_str.c_str()))
{ }

bool html_text_analyzer::is_valid_node(const GumboNode* node) const {
    return node->type == GUMBO_NODE_TEXT ||
           node->type == GUMBO_NODE_ELEMENT && forbidden_tags.find(node->v.element.tag) == forbidden_tags.end();
}

bool html_text_analyzer::is_node_element(const GumboNode* node) const {
    return node->type == GUMBO_NODE_ELEMENT;
}

html_text_analyzer::ratio_type html_text_analyzer::get_text_ratio(const GumboNode* node) const {
    if (!node)
        return default_tag_text_priority;
    else
        return (tag_text_priority.find(node->v.element.tag) != tag_text_priority.end())
                                    ? tag_text_priority.at(node->v.element.tag)
                                    : default_tag_text_priority;
}

void html_text_analyzer::check_for_link(const link_automaton& link, page_info& packet) {
    auto l = link.get();

    if (!l.empty())
        packet.linked_urls.insert(l);
}

void html_text_analyzer::check_for_text(const text_automaton& text, const language_automaton& lang, double ratio, page_info& packet) {
    auto temp = text.get();

    if (!temp.empty())
        packet.text_excerpts.push_back({ temp, lang.get(), ratio  });
}

void html_text_analyzer::parse(page_info& packet) {
    encoding_automaton encoding;
    text_automaton text;
    link_automaton link;

    std::queue<std::tuple<GumboNode*, GumboNode*, language_automaton>> source;

    source.push({ output->root, nullptr, language_automaton() });

    while (!source.empty()) {
        auto temp = source.front();
        source.pop();

        auto& child_node = std::get<0>(temp);
        auto& parent_node = std::get<1>(temp);
        auto& lang = std::get<2>(temp);

        if (!is_valid_node(child_node))
            continue;

        encoding.update(child_node);
        text.update(child_node);
        link.update(child_node);
        lang.update(child_node);

        check_for_link(link, packet);
        check_for_text(text, lang, get_text_ratio(parent_node), packet);

        if (is_node_element(child_node)) {
            GumboVector* children = &child_node->v.element.children;
            for (size_t i = 0; i < children->length; ++i)
                source.push({ static_cast<GumboNode*>(children->data[i]), child_node, lang });
        }
    }

    packet.page_encoding = encoding.get();
}

html_text_analyzer::~html_text_analyzer() {
    gumbo_destroy_output(&kGumboDefaultOptions, output);
}