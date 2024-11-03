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

// ezBuildInc is part of the EzLibs project : https://github.com/aiekick/EzLibs.git

#include <string>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <iostream>

namespace ez {

/* File Format 
#pragma once

#define Project_Label "project"
#define Project_BuildNumber 3629
#define Project_MinorNumber 3
#define Project_MajorNumber 0
#define Project_BuildId "0.3.3629"
*/

class BuildInc {
private:
    std::string m_buildFileHeader;
    std::string m_project;
    std::string m_label;
    int32_t m_majorNumber = 0;
    int32_t m_minorNumber = 0;
    int32_t m_buildNumber = 0;

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
             content =  strStream.str();
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
                startLine = endLine+1;
                endLine = content.find('\n', startLine);
            }
        }
        return *this;
    }
    std::string getInfos() {
        std::stringstream project, build_id, file, infos;
        std::string project_str, build_id_str, file_str;
        build_id << "Build Id : " << m_majorNumber << "." << m_minorNumber << "." << m_buildNumber;
        build_id_str = build_id.str();
        size_t row_len = build_id_str.size();
        file << "In file : " << m_buildFileHeader;
        file_str = file.str();
        if (row_len < file_str.size()) {
            row_len = file_str.size();
        }        
        if (!m_project.empty()) {
            project << "Project : " << m_project;
            project_str = project.str();
            if (row_len < project_str.size()) {
                row_len = project_str.size();
            }
        }
        auto spliter = std::string(row_len + 6, '-');  // +6 for '-- ' and ' --'
        infos << spliter << std::endl;
        if (!m_project.empty()) {
            infos << "-- " << project_str << std::string(row_len - project_str.size(), ' ') << " --" << std::endl;
        }
        infos << "-- " << build_id_str << std::string(row_len - build_id_str.size(), ' ') << " --" << std::endl;
        infos << "-- " << file_str << std::string(row_len - file_str.size(), ' ') << " --" << std::endl;
        infos << spliter << std::endl;
        return infos.str();
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
    BuildInc& write() {
        std::stringstream content;
        content << "#pragma once" << std::endl;
        content << std::endl;
        content << "#define " << m_project << "_Label \"" << m_label << "\"" << std::endl;
        content << "#define " << m_project << "_BuildNumber " << m_buildNumber << std::endl;
        content << "#define " << m_project << "_MinorNumber " << m_minorNumber << std::endl;
        content << "#define " << m_project << "_MajorNumber " << m_majorNumber << std::endl;
        content << "#define " << m_project << "_BuildId \"" << m_majorNumber << "." << m_minorNumber << "." << m_buildNumber << "\"" << std::endl;
        std::ofstream configFileWriter(m_buildFileHeader, std::ios::out);
        if (!configFileWriter.bad()) {
            configFileWriter << content.str();
            configFileWriter.close();
        }
        return *this;
    }

private:
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
        int32_t ret = 0; // 0 is the default value
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
