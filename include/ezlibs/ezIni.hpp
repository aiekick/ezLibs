#pragma once

/*
MIT License

Copyright (c) 2014-2024 Stephane Cuillerdier (aka aiekick)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// EzIni is part od the EzLibs project : https://github.com/aiekick/EzLibs.git

#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace ez {

class Ini {
public:
    using Sections = std::unordered_map<std::string, std::string>;
    using IniDatas = std::unordered_map<std::string, Sections>;
	
private:
    IniDatas m_Datas;
	
public:
    Ini() = default;

    Ini(const std::string& vFilename) {
        load(vFilename);
    }

    void load(const std::string& vFilename) {
        std::ifstream file(vFilename);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + vFilename);
        }
        std::string line, section;
        while (std::getline(file, line)) {
            line = m_Trim(line);

            if (line.empty() || line[0] == ';' || line[0] == '#') {
                continue;
            }
            if (line[0] == '[' && line.back() == ']') {
                section = line.substr(1, line.size() - 2);
                section = m_Trim(section);
                m_Datas[section] = Sections();
            } else {
                auto pos = line.find('=');
                if (pos == std::string::npos) {
                    continue;
                }

                std::string key = m_Trim(line.substr(0, pos));
                std::string value = m_Trim(line.substr(pos + 1));

                m_Datas[section][key] = value;
            }
        }
    }

    void save(const std::string& vFilename) const {
        std::ofstream file(vFilename);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + vFilename);
        }
        for (const auto& [section, entries] : m_Datas) {
            file << "[" << section << "]\n";
            for (const auto& [key, value] : entries) {
                file << key << " = " << value << "\n";
            }
            file << "\n";
        }
    }

    std::string get(const std::string& vSection, const std::string& vKey, const std::string& vDefaultValue = "") const {
        auto sectionIt = m_Datas.find(vSection);
        if (sectionIt != m_Datas.end()) {
            auto keyIt = sectionIt->second.find(vKey);
            if (keyIt != sectionIt->second.end()) {
                return keyIt->second;
            }
        }
        return vDefaultValue;
    }

    void set(const std::string& vSection, const std::string& vKey, const std::string& vValue) {
        m_Datas[vSection][vKey] = vValue;
    }

private:
    std::string m_Trim(const std::string& vStr) const {
        const char* whitespace = " \t\n\r\f\v";
        size_t start = vStr.find_first_not_of(whitespace);
        size_t end = vStr.find_last_not_of(whitespace);
        if (start == std::string::npos || end == std::string::npos) {
            return {};
        }
        return vStr.substr(start, end - start + 1);
    }
};

} // namespace ez
