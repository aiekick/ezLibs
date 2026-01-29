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

// ezBuildInc is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <string>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <iostream>

#include "ezFmt.hpp"

// you msut include ezFigFont.hpp before this include
// if you want to enable the FigFont Label Generation

namespace ez {

/* File Format
#pragma once

#define Project_Label "project"
#define Project_BuildNumber 3629
#define Project_MinorNumber 3
#define Project_MajorNumber 0
#define Project_BuildId "0.3.3629"
#define Project_FigFontLabel "..." // Optionnal
*/

class BuildInc {
private:
    bool m_lastWriteStatus = false;
    bool m_lastWriteJsFileStatus = false;
    std::string m_JsFilePath;
    std::string m_buildFileHeader;
    std::string m_project;
    std::string m_label;
    int32_t m_majorNumber = 0;
    int32_t m_minorNumber = 0;
    int32_t m_buildNumber = 0;
#ifdef EZ_FIG_FONT
    class FigFontGenerator {
        friend class BuildInc;

    private:
        ez::FigFont m_generator;
        bool m_useLabel = true;         // will use the label or not for the FigFont label
        bool m_useBuildNumber = false;  // will use the buildNumber or not for the FigFont label
    public:
        bool isValid() { return m_generator.isValid(); }
        FigFontGenerator& useLabel(const bool vFlag) {
            m_useLabel = vFlag;
            return *this;
        }
        FigFontGenerator& useBuildNumber(const bool vFlag) {
            m_useBuildNumber = vFlag;
            return *this;
        }
    } m_figFontGenerator;
#endif  // EZ_FIG_FONT

public:
    BuildInc(const std::string& vBuildFileHeader) {
        m_buildFileHeader = vBuildFileHeader;
        read();
    }
    BuildInc& read() {
        std::string content;
        std::ifstream docFile(m_buildFileHeader, std::ios::in);
        if (docFile.is_open()) {
            std::stringstream strStream;
            strStream << docFile.rdbuf();
            content = strStream.str();
            docFile.close();
        }
        if (!content.empty()) {
            size_t startLine = 0;
            size_t endLine = content.find('\n', startLine);
            std::string line;
            std::string project, key, value;
            while (endLine != std::string::npos) {
                line = content.substr(startLine, endLine - startLine);
                if (m_parseDefine(line, project, key, value)) {
                    m_project = project;  // overwrote each time but its the same for each
                    if (key == "Label") {
                        m_label = value;
                    } else if (key == "MajorNumber") {
                        m_majorNumber = m_toNumber(value);
                    } else if (key == "MinorNumber") {
                        m_minorNumber = m_toNumber(value);
                    } else if (key == "BuildNumber") {
                        m_buildNumber = m_toNumber(value);
                    }
                }
                startLine = endLine + 1;
                endLine = content.find('\n', startLine);
            }
        }
        return *this;
    }
    std::string getBuildIdInt() {
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2) << m_majorNumber << std::setfill('0') << std::setw(2) << m_minorNumber << m_buildNumber;
        return ss.str();
    }
    std::string getBuildIdStr() {
        std::stringstream ss;
        ss << m_majorNumber << "." << m_minorNumber << "." << m_buildNumber;
        return ss.str();
    }
    std::string getInfos() {
        ez::TableFormatter tbl({"Desc", "Result"});
        if (!m_project.empty()) {
            tbl.addRow({"Project", m_project});
        }
        tbl.addRow({"Build Id", getBuildIdStr() + " / " + getBuildIdInt()});
        if (m_lastWriteStatus) {
            tbl.addRow({"In C/C++ file", m_buildFileHeader});
        } else {
            tbl.addRow({"failed to write to", m_buildFileHeader});
        }
        if (!m_JsFilePath.empty()) {
            if (m_lastWriteJsFileStatus) {
                tbl.addRow({"In JS file", m_JsFilePath});
            } else {
                tbl.addRow({"failed to write to", m_JsFilePath});
            }
        }
        std::stringstream ss;
        tbl.print("", ss);
        return ss.str();
    }
    BuildInc& printInfos() {
        std::cout << getInfos();
        return *this;
    }
    const std::string& getProject() { return m_project; }
    const std::string& getLabel() { return m_label; }
    int32_t getMajor() { return m_majorNumber; }
    int32_t getMinor() { return m_minorNumber; }
    int32_t getBuildNumber() { return m_buildNumber; }
    BuildInc& setProject(const std::string& vProject) {
        m_project = vProject;
        return *this;
    }
    BuildInc& setLabel(const std::string& vLabel) {
        m_label = vLabel;
        return *this;
    }
    BuildInc& setMajor(const int32_t vMajorNumber) {
        m_majorNumber = vMajorNumber;
        return *this;
    }
    BuildInc& setMinor(const int32_t vMinorNumber) {
        m_minorNumber = vMinorNumber;
        return *this;
    }
    BuildInc& setBuildNumber(const int32_t vBuildNumber) {
        m_buildNumber = vBuildNumber;
        return *this;
    }
    BuildInc& incBuildNumber() {
        ++m_buildNumber;
        return *this;
    }
    BuildInc& setJsFilePath(const std::string& vJsFilePath) {
        m_JsFilePath = vJsFilePath;
        return *this;
    }
#ifdef EZ_FIG_FONT
    FigFontGenerator& setFigFontFile(const std::string& vFigFontFile) {
        m_figFontGenerator.m_generator.load(vFigFontFile);
        return m_figFontGenerator;
    }
#endif  // EZ_FIG_FONT
    BuildInc& write() {
        m_lastWriteStatus = false;
        std::ofstream configFileWriter(m_buildFileHeader, std::ios::out);
        if (!configFileWriter.bad()) {
            std::stringstream content;
            content << "#pragma once" << std::endl;
            content << std::endl;
            content << "#define " << m_project << "_Label \"" << m_label << "\"" << std::endl;
            content << "#define " << m_project << "_BuildNumber " << m_buildNumber << std::endl;
            content << "#define " << m_project << "_MinorNumber " << m_minorNumber << std::endl;
            content << "#define " << m_project << "_MajorNumber " << m_majorNumber << std::endl;
            content << "#define " << m_project << "_BuildId \"" << getBuildIdStr() << "\"" << std::endl;
            content << "#define " << m_project << "_BuildIdNum " << getBuildIdInt() << std::endl;
#ifdef EZ_FIG_FONT
            if (m_figFontGenerator.isValid()) {
                std::stringstream version;
                if (m_figFontGenerator.m_useLabel) {
                    version << m_label << " ";
                }
                version << "v" << m_majorNumber << "." << m_minorNumber;
                if (m_figFontGenerator.m_useBuildNumber) {
                    version << "." << m_buildNumber;
                }
                content << "#define " << m_project << "_FigFontLabel u8R\"(" << m_figFontGenerator.m_generator.printString(version.str()) << ")\"" << std::endl;
            }
#endif  // EZ_FIG_FONT
            configFileWriter << content.str();
            configFileWriter.close();
            m_lastWriteStatus = true;
        }
        m_writeJsFile();  // if m_JsFilePath not empty
        return *this;
    }

private:
    BuildInc& m_writeJsFile() {
        m_lastWriteJsFileStatus = false;
        if (!m_JsFilePath.empty()) {
            std::ofstream configFileWriter(m_JsFilePath, std::ios::out);
            if (!configFileWriter.bad()) {
                std::stringstream content;
                content << std::endl;
                content << "const " << m_project << "_Label = \"" << m_label << "\";" << std::endl;
                content << "const " << m_project << "_BuildNumber = " << m_buildNumber << ";" << std::endl;
                content << "const " << m_project << "_MinorNumber = " << m_minorNumber << ";" << std::endl;
                content << "const " << m_project << "_MajorNumber = " << m_majorNumber << ";" << std::endl;
                content << "const " << m_project << "_BuildId = \"" << getBuildIdStr() << "\";" << std::endl;
                content << "const " << m_project << "_BuildIdNum = " << getBuildIdInt() << ";" << std::endl;
                configFileWriter << content.str();
                configFileWriter.close();
                m_lastWriteJsFileStatus = true;
            }
        }
        return *this;
    }
    // will parse a line '#define [PROJECT]_[KEY] [VALUE]'
    // return true is succeed, false if the format is not recognized
    bool m_parseDefine(const std::string& vRowContent, std::string& vOutProject, std::string& vOutKey, std::string& vOutValue) {
        if (!vRowContent.empty()) {
            size_t def_pos = vRowContent.find("#define ");
            if (def_pos != std::string::npos) {
                def_pos += 8;  // offset for '#define '
                size_t underScore_pos = vRowContent.find('_', def_pos);
                if (underScore_pos != std::string::npos) {
                    vOutProject = vRowContent.substr(def_pos, underScore_pos - def_pos);
                    ++underScore_pos;  // offset for '_'
                    size_t space_pos = vRowContent.find(' ', underScore_pos);
                    if (space_pos != std::string::npos) {
                        vOutKey = vRowContent.substr(underScore_pos, space_pos - underScore_pos);
                        ++space_pos;  // offset for ' '
                        vOutValue = m_trim(vRowContent.substr(space_pos));
                        return true;
                    }
                }
            }
        }
        return false;
    }
    int32_t m_toNumber(const std::string& vNum) {
        int32_t ret = 0;  // 0 is the default value
        try {
            ret = std::stoi(vNum);
        } catch (...) {
        }
        return ret;
    }
    // will remove quotes
    std::string m_trim(const std::string& vValue) {
        std::string ret;
        for (auto& c : vValue) {
            if (c != '\"') {
                ret += c;
            }
        }
        return ret;
    }
};

}  // namespace ez
