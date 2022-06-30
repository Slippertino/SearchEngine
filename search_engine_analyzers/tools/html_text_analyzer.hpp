#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <queue>
#include <libs/gumbo_parser/src/gumbo.h>
#include "../html_text_automatons/language_automaton.hpp"
#include "../html_text_automatons/encoding_automaton.hpp"
#include "../html_text_automatons/link_automaton.hpp"
#include "../html_text_automatons/text_automaton.hpp"
#include "../html_text_automatons/title_automaton.hpp"
#include "se_encoder.hpp"
#include "text_property_types/string_enc.hpp"

class html_text_analyzer {
private:
    friend class language_automaton;
    friend class encoding_automaton;
    friend class link_automaton;
    friend class text_automaton;

public:
    using ratio_type = double;

    struct page_info {
        std::string url;
        std::string prefix;
        std::string title;
        std::string content;
        se_encoding page_encoding;
        long status_code;
        std::unordered_set<string_enc> linked_urls;
        std::vector<std::tuple<std::string, se_language, ratio_type>> text_excerpts;
    };

private:
    static const std::unordered_set<GumboTag> forbidden_tags;
    static const std::unordered_map<GumboTag, ratio_type> tag_text_priority;
    static const ratio_type default_tag_text_priority;
    static const se_encoding internal_enc;

    string_enc content;

private:
    bool is_valid_node(const GumboNode* node) const {
        return node->type == GUMBO_NODE_TEXT ||
               node->type == GUMBO_NODE_ELEMENT && forbidden_tags.find(node->v.element.tag) == forbidden_tags.end();
    }

    bool is_node_element(const GumboNode* node) const {
        return node->type == GUMBO_NODE_ELEMENT;
    }

    html_text_analyzer::ratio_type get_text_ratio(const GumboNode* node) const {
        if (!node)
            return default_tag_text_priority;
        else
            return (tag_text_priority.find(node->v.element.tag) != tag_text_priority.end())
                                     ? tag_text_priority.at(node->v.element.tag)
                                     : default_tag_text_priority;
    }

    void check_for_link(const link_automaton& link, page_info& packet) {
        auto l = link.get();

        if (!l.empty())
            packet.linked_urls.insert({ l, encoding_t::UNKNOWN });
    }

    void check_for_text(const text_automaton& text,
                        const language_automaton& lang,
                        double ratio,
                        page_info& packet) {
        auto temp = text.get();

        if (!temp.empty())
            packet.text_excerpts.push_back({ temp, lang.get(), ratio });
    }

    void crop_content() {
        const auto tags = std::map<std::string, std::string>{ {"<script",   "</script>"  },
                                                              {"<style",    "</style>"   },
                                                              {"<button",   "</button>"  },
                                                              {"<footer",   "</footer>"  },
                                                              {"<noscript", "</noscript>"},
                                                              {"<!--",      "-->"        } };
        auto& s = content.str;

        size_t off = 0;
        while (true) {
            std::pair<std::string, std::string> tag;
            auto bpos = std::string::npos;

            for (auto& p : tags) {
                auto cur = s.find(p.first, off);

                if (cur < bpos) {
                    bpos = cur;
                    tag = p;
                }
            }

            if (bpos != std::string::npos) {
                auto epos = s.find(tag.second, bpos);
                off = bpos;
                s.erase(bpos, epos + tag.second.size() - bpos);
            }
            else
                break;
        }
    }

    void execute_parse(GumboOutput* output, page_info& packet) {
        encoding_automaton encoding;
        text_automaton text;
        title_automaton title;
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
            text    .update(child_node);
            link    .update(child_node);
            lang    .update(child_node);
            title   .update(child_node);

            check_for_link(link, packet);
            check_for_text(text, lang, get_text_ratio(parent_node), packet);

            if (is_node_element(child_node)) {
                GumboVector* children = &child_node->v.element.children;
                for (size_t i = 0; i < children->length; ++i)
                    source.push({ static_cast<GumboNode*>(children->data[i]), child_node, lang });
            }
        }

        packet.title = title.get();
        packet.page_encoding = encoding.get();

        std::vector<string_enc> buff(
            std::make_move_iterator(packet.linked_urls.begin()),
            std::make_move_iterator(packet.linked_urls.end())
        );

        std::for_each(buff.begin(), buff.end(), [&packet](string_enc& val) {
            val.enc = packet.page_encoding;
        });

        packet.linked_urls = std::unordered_set<string_enc>(
            std::make_move_iterator(buff.begin()),
            std::make_move_iterator(buff.end())
        );
    }

public:
    html_text_analyzer() = delete;
    html_text_analyzer(const string_enc& html_str) : content(html_str) 
    { }

    html_text_analyzer operator= (const html_text_analyzer& g_h) = delete;

    void run_parse(page_info& packet) {
        se_encoder::encode(content.str, content.enc, DEFAULT_ENCODING);
        crop_content();
        se_encoder::encode(content.str, DEFAULT_ENCODING, internal_enc);

        GumboOutput* output(gumbo_parse(content.str.c_str()));

        se_encoder::encode(content.str, internal_enc, content.enc);
        packet.content = std::move(content.str);

        execute_parse(output, packet);

        gumbo_destroy_output(&kGumboDefaultOptions, output);
    }
};

const se_encoding html_text_analyzer::internal_enc = encoding_t::UTF_8;

const std::unordered_set<GumboTag> html_text_analyzer::forbidden_tags = {
    GUMBO_TAG_SCRIPT,
    GUMBO_TAG_STYLE,
    GUMBO_TAG_FOOTER,
    GUMBO_TAG_NOSCRIPT,
    GUMBO_TAG_BUTTON,
};

const std::unordered_map<GumboTag, html_text_analyzer::ratio_type> html_text_analyzer::tag_text_priority = {
    {GUMBO_TAG_TITLE, 10.0},
    {GUMBO_TAG_H1,    5.0 },
    {GUMBO_TAG_H2,    5.0 },
    {GUMBO_TAG_H3,    5.0 },
    {GUMBO_TAG_H4,    3.0 },
    {GUMBO_TAG_H5,    3.0 },
    {GUMBO_TAG_H6,    3.0 },
    {GUMBO_TAG_TD,    0.5 },
};

const html_text_analyzer::ratio_type html_text_analyzer::default_tag_text_priority = 1.0;