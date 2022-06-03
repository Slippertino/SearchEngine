#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include "gumbo.h"
#include "language_automaton.h"
#include "encoding_automaton.h"
#include "link_automaton.h"
#include "text_automaton.h"

class html_text_analyzer
{
private:
    friend class language_automaton;
    friend class encoding_automaton;
    friend class link_automaton;
    friend class text_automaton;

public:
    using ratio_type = double;

    struct page_info
    {
        std::string url;
        std::string prefix;
        std::string content;
        encoding_t page_encoding;
        long status_code;
        std::unordered_set<std::string> linked_urls;
        std::vector<std::tuple<std::string, language_t, ratio_type>> text_excerpts;
    };

private:
    static const std::unordered_set<GumboTag> forbidden_tags;
    static const std::unordered_map<GumboTag, ratio_type> tag_text_priority;
    static const ratio_type default_tag_text_priority;

	GumboOutput* output;

private:
    bool is_valid_node(const GumboNode*) const;
    bool is_node_element(const GumboNode*) const;

    ratio_type get_text_ratio(const GumboNode*) const;

    void check_for_link(const link_automaton&, 
                        page_info&);

    void check_for_text(const text_automaton&,
                        const language_automaton&,
                        double,
                        page_info&);

public:
    html_text_analyzer() = delete;
    html_text_analyzer(const std::string&);

    html_text_analyzer operator= (const html_text_analyzer& g_h) = delete;

    void parse(page_info&);

    ~html_text_analyzer();
};