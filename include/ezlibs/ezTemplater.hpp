#pragma once

#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

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
        std::string result;
        m_process(result);
        return result;
    }

    bool saveToFile(const std::string &vFilePathName) {
        const auto& result = saveToString();
        if (!result.empty()) {
            std::ofstream out(vFilePathName);
            if (out) {
                out << result;
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

    std::string getUsedTagInfos(const std::string& vPrefix) {
        std::string ret;
        std::map<std::string, std::string> ordered_map;
        for(const auto& tag : m_tagValues) {
            ordered_map[tag.first] = tag.second;
        }
        for(const auto& tag : ordered_map) {
            if (!ret.empty()) {
                ret += '\n';
            }
            ret += vPrefix + " [[" + tag.first + "]] => '";
            // multiline value ?
            if (tag.second.find("\n") != std::string::npos) {
                ret += '\n';
            }
            ret += tag.second + "'";
        }
        return ret;
    }

private:
    bool m_precheck() {
        try {
            /*size_t cntOpen = 0, cntClose = 0;
            for (size_t p = 0; (p = m_template.find("[[", p)) != std::string::npos; ++p)
                ++cntOpen;
            for (size_t p = 0; (p = m_template.find("]]", p)) != std::string::npos; ++p)
                ++cntClose;
            if (cntOpen != cntClose) {
                throw std::runtime_error("Unequal number of [[ and ]] tags");
            }*/
        } catch (const std::runtime_error &e) {
            std::cout << "Error: " << e.what() << std::endl;
            return false;
        }
        return true;
    }

    bool m_process(std::string& voResult) {
        try {
            voResult = m_parseSegment(m_template);
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
        const size_t segment_size = seg.size();

        while (pos < segment_size) {
            auto open = seg.find("[[", pos);
            auto close = seg.find("]]", pos);
            if (close != std::string::npos && (open == std::string::npos || close < open)) {
                // Case: "]]" before "[[" → error
                throw std::runtime_error("Detected ']]' before '[['");
            }
            if (open == std::string::npos) {
                // no more tags
                out.append(seg, pos, segment_size - pos);
                break;
            }

            // copy text before "[["
            out.append(seg, pos, open - pos);

            // find possible closing "]]"
            auto maybeClose = seg.find("]]", open + 2);
            if (maybeClose == std::string::npos) {
                throw std::runtime_error("Missing ']]' to close '[['");
            }

            // is there a '\n' before maybeClose?
            auto line_end = seg.find('\n', open + 2);
            if (line_end == std::string::npos || line_end > maybeClose) {
                // simple line tag [[TAG]]
                std::string tag = seg.substr(open + 2, maybeClose - (open + 2));
                if (!m_disabledTags.count(tag)) {
                    auto it = m_tagValues.find(tag);
                    if (it != m_tagValues.end()) {
                        // if tag content is multi line.
                        if (it->second.find('\n') < it->second.size()) {
                            std::string offset_string_from_start_line;
                            auto startLinePos = seg.rfind('\n', open);
                            if (startLinePos != std::string::npos) {
                                ++startLinePos;
                                offset_string_from_start_line = seg.substr(startLinePos, open - startLinePos);
                            }
                            if (!offset_string_from_start_line.empty()) {
                                std::istringstream stream(it->second);
                                std::string row;
                                size_t idx = 0;
                                while (std::getline(stream, row)) {
                                    if (idx++ != 0) {
                                        out.append("\n").append(offset_string_from_start_line);
                                    } 
                                    out.append(row);
                                }
                            } else {
                                out.append(it->second);
                            }
                        } else {
                            out.append(it->second);
                        }
                    }
                }
                pos = maybeClose + 2;
            } else {
                // multi line tag 
                // [[TAG 
                // ...
                // ]]
                open += 2;
                std::string tag = seg.substr(open, line_end - open);
                // find the matching closing "]]"
                size_t endBlock = m_findMatchingEnd(seg, line_end + 1);
                // extract content, without initial '\n'
                size_t contentStart = line_end + 1;
                std::string content = seg.substr(contentStart, endBlock - contentStart);
                if (!content.empty() && content[0] == '\n') {
                    content.erase(0, 1);
                }
                if (!m_disabledTags.count(tag) && m_tagValues.count(tag)) {
                    out += m_parseSegment(content);
                }
                // skip possible newline right after closing "]]"
                pos = endBlock + 2;
                if (pos < segment_size && seg[pos] == '\n') {
                    ++pos;
                }
            }
        }

        return out;
    }
};

}  // namespace ez
