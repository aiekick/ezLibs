#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <iostream>

/*
This class can create file from a template syntax :
ex :
 int main() {
    try {
        ez::Templater tpl;
        if (tpl.load("input.txt")) {
            tpl.useTag("TOTO").useTag("TATA").useTag("TITI", "titre");
            tpl.save("output.txt");
        }
    } catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << "\n";
        return 1;
    }
    return 0;
}
 with input.txt :
[[TOTO]] = new [[TOTO]]

[[TATA
if ([[TATA]]) {
    TITI = [[TITI]]
}
]]

[[TITI
print("hello world");
]]

get :
 TOTO = new TOTO

if (TATA) {
    TITI = titre
}

print("hello world");

 */

namespace ez {

class Templater {
private:
    std::string m_template;
    std::string m_result;
    std::unordered_map<std::string, std::string> m_tagValues;
    std::unordered_set<std::string> m_disabledTags;

public:
    bool loadFromString(const std::string &vText) {
        m_template = vText;
        return m_precheck();
    }

    bool loadFromFile(const std::string &vFilePathName) {
        std::ifstream in(vFilePathName);
        if (in) {
            std::ostringstream ss;
            ss << in.rdbuf();
            return loadFromString(ss.str());
        }
        return false;
    }

   std::string saveToString() {
        m_process();
        return m_result;
    }

    bool saveToFile(const std::string &vFilePathName) {
        if (m_process() && !m_result.empty()) {
            std::ofstream out(vFilePathName);
            if (out) {
                out << m_result;
                return true;
            }
        }
        return false;
    }

    Templater &useTag(const std::string &tagName, const std::string &value = "") {
        m_disabledTags.erase(tagName);
        m_tagValues[tagName] = value.empty() ? tagName : value;
        return *this;
    }

    Templater &unuseTag(const std::string &tagName) {
        m_tagValues.erase(tagName);
        m_disabledTags.insert(tagName);
        return *this;
    }

private:
    bool m_precheck() {
        try {
            auto pos = m_template.find("[[[");
            if (pos != std::string::npos) {
                throw std::runtime_error("Invalid syntax: three consecutive opening brackets at position " + std::to_string(pos));
            }
            pos = m_template.find("]]]");
            if (pos != std::string::npos) {
                throw std::runtime_error("Invalid syntax: three consecutive closing brackets at position " + std::to_string(pos));
            }
            size_t cntOpen = 0, cntClose = 0;
            for (size_t p = 0; (p = m_template.find("[[", p)) != std::string::npos; ++p)
                ++cntOpen;
            for (size_t p = 0; (p = m_template.find("]]", p)) != std::string::npos; ++p)
                ++cntClose;
            if (cntOpen != cntClose) {
                throw std::runtime_error("Unequal number of [[ and ]] tags");
            }
        } catch (const std::runtime_error &e) {
            std::cout << "Error: " << e.what() << std::endl;
            return false;
        }
        return true;
    }

    bool m_process() {
        m_result.clear();
        try {
            m_result = m_parseSegment(m_template);
        } catch (const std::runtime_error &e) {
            std::cout << "Error: " << e.what() << std::endl;
            return false;
        }
        return true;
    }

    // Finds the matching "]]", handles nesting, and reports syntax errors
    size_t m_findMatchingEnd(const std::string &s, size_t startPos) const {
        size_t pos = startPos;
        int depth = 0;
        while (pos + 1 < s.size()) {
            if (s[pos] == '[' && s[pos + 1] == '[') {
                depth++;
                pos += 2;
            } else if (s[pos] == ']' && s[pos + 1] == ']') {
                if (depth == 0) {
                    return pos;
                }
                depth--;
                pos += 2;
            } else {
                ++pos;
            }
        }
        throw std::runtime_error("Unclosed tag");
    }

    // Recursive segment parser
    std::string m_parseSegment(const std::string &seg) const {
        std::string out;
        size_t pos = 0;
        const size_t N = seg.size();

        while (pos < N) {
            auto open = seg.find("[[", pos);
            auto close = seg.find("]]", pos);
            if (close != std::string::npos && (open == std::string::npos || close < open)) {
                // Case: "]]" before "[[" → error
                throw std::runtime_error("Detected ']]' before '[['");
            }
            if (open == std::string::npos) {
                // no more tags
                out.append(seg, pos, N - pos);
                break;
            }

            // copy text before "[["
            out.append(seg, pos, open - pos);

            // find possible closing "]]"
            auto maybeClose = seg.find("]]", open + 2);
            if (maybeClose == std::string::npos) {
                throw std::runtime_error("Missing ']]' to close '[['");
            }

            // is there a ':' before maybeClose?
            auto colon = seg.find('\n', open + 2);
            if (colon == std::string::npos || colon > maybeClose) {
                // simple tag [[TAG]]
                std::string tag = seg.substr(open + 2, maybeClose - (open + 2));
                if (!m_disabledTags.count(tag)) {
                    auto it = m_tagValues.find(tag);
                    if (it != m_tagValues.end())
                        out += it->second;
                }
                pos = maybeClose + 2;
            } else {
                // block [[TAG: ... ]]
                std::string tag = seg.substr(open + 2, colon - (open + 2));
                // find the matching closing "]]"
                size_t endBlock = m_findMatchingEnd(seg, colon + 1);
                // extract content, without initial '\n'
                size_t contentStart = colon + 1;
                std::string content = seg.substr(contentStart, endBlock - contentStart);
                if (!content.empty() && content[0] == '\n')
                    content.erase(0, 1);

                if (!m_disabledTags.count(tag) && m_tagValues.count(tag)) {
                    out += m_parseSegment(content);
                }
                // skip possible newline right after closing "]]"
                pos = endBlock + 2;
                if (pos < N && seg[pos] == '\n')
                    ++pos;
            }
        }

        return out;
    }
};

}  // namespace ez
