#pragma once

#include <array>
#include <string>
#include <vector>
#include <cstdint>
#include <sstream>
#include <fstream>
#include <iostream>
#include <unordered_map>

#include "ezLog.hpp"

namespace ez {

class FigFont {
private:
	bool m_isValid{false};
    struct Header {
        uint8_t endChar{};
        uint8_t hardblank{};  // Filling char
        int32_t height{};  // char height
        int32_t baseline{};  // base line from top
        int32_t maxLength{};  // max width of achar
        int32_t oldLayout{}; // Old Layout
        int32_t commentLines{};  // Comments line count just after header line
        int32_t printDirection{};  // printing direction
        int32_t fullLayout{};  // Full Layout
        int32_t codetagCount{};  // codeTagCount (optional)
        std::vector<std::string> commentBlock;
    } m_header;
    struct Glyph {
        std::string desc;
        std::vector<std::string> rows;
    };
    std::unordered_map<uint8_t, Glyph> m_glyphs;
	
public:
    FigFont() = default;
    FigFont(const std::string& vFilePathName) : m_isValid(m_load(vFilePathName)) {}
    ~FigFont() { m_unit(); }
    bool isValid() { return m_isValid; }
    FigFont& load(const std::string& vFilePathName) {
        m_isValid = m_load(vFilePathName);
        return *this;
	}
    std::string printString(const std::string& vPattern) {
        return m_printString(vPattern);
    }
	
private:
	bool m_load(const std::string& vFilePathName) {
        std::ifstream file(vFilePathName);
        if (!file.is_open()) {
            LogVarError("Failed to open the file %s", vFilePathName.c_str());
            return false;
        }

        std::string header;
        std::getline(file, header);
        std::istringstream headerStream(header);
        std::string magicNumber;
        headerStream >> magicNumber;
        if (magicNumber.substr(0, 5) != "flf2a") {
            LogVarError("Not a valid FIGfont file");
            return false;
        }

        /*
          flf2a$ 6 5 20 15 3 0 143 229    NOTE: The first five characters in
            |  | | | |  |  | |  |   |     the entire file must be "flf2a".			
           /  /  | | |  |  | |  |    \		   
  Signature  /  /  | |  |  | |   \   Codetag_Count
    Hardblank  /  /  |  |  |  \   Full_Layout*
         Height  /   |  |   \  Print_Direction
         Baseline   /    \   Comment_Lines
          Max_Length      Old_Layout*
        */
        m_header.hardblank = magicNumber[5];
        headerStream >>  //
            m_header.height >>  //
            m_header.baseline >>  //
            m_header.oldLayout >>  //
            m_header.maxLength >>  //
            m_header.commentLines >>  //
            m_header.printDirection >>  //
            m_header.fullLayout >>  //
            m_header.codetagCount;

        std::string line;
        m_header.commentBlock.reserve(m_header.commentLines);
        for (int i = 0; i < m_header.commentLines; ++i) {
            std::getline(file, line);
            m_header.commentBlock.push_back(line);
        }
        size_t idx = 0;
        static constexpr uint8_t baseChar = 32;
        static constexpr size_t required_chars_count = 127 - baseChar;
        static constexpr size_t additionnal_chars_count = 7;
        std::array<uint8_t, additionnal_chars_count> additionnal_chars{196, 214, 220, 228, 246, 252, 223};
        char cChar = baseChar;
        while (file) {
            std::string line;
            std::vector<std::string> asciiArt;
            if (idx < required_chars_count) {
                cChar = idx + baseChar;
                if (!m_parseChar(file, cChar)) {
                    return false;
                }
            } else if (idx - required_chars_count < additionnal_chars_count) {
                cChar = additionnal_chars.at(idx - required_chars_count);
                if (!m_parseChar(file, cChar)) {
                    return false;
                }
            } else {
                if (!m_parseChar(file, 0)) {
                }
            }
            ++idx;
        }
        return true;
	}

    bool m_parseChar(std::ifstream& vFile, const uint8_t vChar) {
        std::string row;
        uint8_t charCode = vChar;
        Glyph glyph;
        if (charCode < 32) {
            std::getline(vFile, row);
            size_t spacePos = row.find(' ');
            if (spacePos != std::string::npos) {
                const auto numStr = row.substr(0, spacePos);
                const auto descStr = row.substr(spacePos + 1);
                charCode = static_cast<uint8_t>(std::stoi(numStr));
            } else {
                return false;
            }
        }
        glyph.rows.reserve(m_header.height);
        for (int i = 0; i < m_header.height; ++i) {
            std::getline(vFile, row);
            if (!row.empty()) {
                while (row.back() == '@') {
                    row.pop_back();
                }
                if (row.front() == ' ') {
                    row = row.substr(1);
                }
                for (auto& c : row) {
                    if (c == static_cast<char>(m_header.hardblank)) {
                        c = ' ';
                    }
                }
            }
            glyph.rows.push_back(row);
        }
        m_glyphs[charCode] = glyph;
        return true;
    }

	void m_unit() {
	
	}

	std::string m_printString(const std::string& vPattern) {
        std::stringstream ret;
        std::vector<std::string> rows;
        rows.resize(m_header.height);
        size_t rwo_idx = 0;
        for (auto& row : rows) {
            for (const auto c : vPattern) {
                row += m_getCharRow(c, rwo_idx);
            }
            ++rwo_idx;
            ret << row << std::endl;
        }
        return ret.str();
	}

    bool m_getCharRows(uint8_t vC, std::vector<std::string>& vOutRows) {
        vOutRows.clear();
        vOutRows.reserve(m_header.height);
        auto it = m_glyphs.find(vC);
        if (it != m_glyphs.end()) {
            for (const auto& row : it->second.rows) {
                vOutRows.push_back(row);
            }
        } else {
            return false;
        }
        return true;
    }

    std::string m_getCharRow(uint8_t vC, uint8_t vRowIdx) {
        auto it = m_glyphs.find(vC);
        if (it != m_glyphs.end()) {
            if (vRowIdx < it->second.rows.size()) {
                return it->second.rows.at(vRowIdx);
            }
        } 
        return {};
    }
};

}