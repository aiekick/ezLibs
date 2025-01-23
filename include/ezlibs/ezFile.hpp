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

// ezFile is part od the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <fstream>
#include <iterator>
#include <sstream>
#include <cstdint>
#include <string>
#include <vector>
#include <ctime>

#include "ezStr.hpp"
#include "ezLog.hpp"
#include <sys/stat.h>

#ifndef EZ_FILE_SLASH_TYPE
#ifdef WIN32
#define EZ_FILE_SLASH_TYPE "\\"
#include <Windows.h>
#define stat _stat
#else  // UNIX
#define EZ_FILE_SLASH_TYPE "/"
#endif
#endif  // EZ_FILE_SLASH_TYPE

namespace ez {
namespace file {

inline std::string loadFileToString(const std::string &vFilePathName) {
    std::string ret;
    std::ifstream docFile(vFilePathName, std::ios::in);
    if (docFile.is_open()) {
        std::stringstream strStream;
        strStream << docFile.rdbuf();  // read the file
        ret = strStream.str();
        ez::str::replaceString(ret, "\r\n", "\n");
        ez::str::replaceString(ret, "\r", "\n");
        docFile.close();
    } else {
#ifdef EZ_TOOLS_LOG
        LogVarError("File \"%s\" Not Found !\n", vFilePathName.c_str());
#endif
    }
    return ret;
}

inline bool saveStringToFile(const std::string &vDatas, const std::string &vFilePathName, bool vAddTimeStamp = false) {
    std::string fpn = vFilePathName;
    if (!fpn.empty()) {
        if (vAddTimeStamp) {
            auto dot_p = fpn.find_last_of('.');
            time_t epoch = std::time(nullptr);
            if (dot_p != std::string::npos) {
                fpn = fpn.substr(0, dot_p) + ez::str::toStr("_%llu", epoch) + fpn.substr(dot_p);
            } else {
                fpn += ez::str::toStr("_%llu", epoch);
            }
        }
        std::ofstream configFileWriter(fpn, std::ios::out);
        if (!configFileWriter.bad()) {
            configFileWriter << vDatas;
            configFileWriter.close();
            return true;
        }
    }
    return false;
}

inline std::vector<uint8_t> loadFileToBin(const std::string &vFilePathName) {
    std::vector<uint8_t> ret;
    std::ifstream docFile(vFilePathName, std::ios::in | std::ios::binary);
    if (docFile.is_open()) {
        // int32_t because the internal << used by 'istream_iterator' is not defined for uint8_t
        ret = {std::istream_iterator<char>(docFile), std::istream_iterator<char>()};
        docFile.close();
    } else {
#ifdef EZ_TOOLS_LOG
        LogVarError("File \"%s\" Not Found !\n", vFilePathName.c_str());
#endif
    }
    return ret;
}

inline bool saveBinToFile(const std::vector<uint8_t> &vDatas, const std::string &vFilePathName, bool vAddTimeStamp = false) {
    std::string fpn = vFilePathName;
    if (!fpn.empty()) {
        if (vAddTimeStamp) {
            auto dot_p = fpn.find_last_of('.');
            time_t epoch = std::time(nullptr);
            if (dot_p != std::string::npos) {
                fpn = fpn.substr(0, dot_p) + ez::str::toStr("_%llu", epoch) + fpn.substr(dot_p);
            } else {
                fpn += ez::str::toStr("_%llu", epoch);
            }
        }
        std::ofstream out(fpn, std::ios::out | std::ios::binary);
        if (!out.bad()) {
            out.write(reinterpret_cast<const char *>(vDatas.data()), vDatas.size());
            out.close();
            return true;
        }
    }
    return false;
}

/* correct file path between os and different slash type between window and unix */
inline std::string correctSlashTypeForFilePathName(const std::string &vFilePathName) {
    std::string res = vFilePathName;
    ez::str::replaceString(res, "\\", EZ_FILE_SLASH_TYPE);
    ez::str::replaceString(res, "/", EZ_FILE_SLASH_TYPE);
    return res;
}

struct PathInfos {
    std::string path;
    std::string name;
    std::string ext;
    bool isOk = false;

    PathInfos() {
        isOk = false;
    }

    PathInfos(const std::string &vPath, const std::string &vName, const std::string &vExt) {
        isOk = true;
        path = vPath;
        name = vName;
        ext = vExt;

        if (ext.empty()) {
            const size_t lastPoint = name.find_last_of('.');
            if (lastPoint != std::string::npos) {
                ext = name.substr(lastPoint + 1);
                name = name.substr(0, lastPoint);
            }
        }
    }

    std::string GetFPNE() {
        return GetFPNE_WithPathNameExt(path, name, ext);
    }

    std::string GetFPNE_WithPathNameExt(std::string vPath, const std::string &vName, const std::string &vExt) {
        if (vPath[0] == EZ_FILE_SLASH_TYPE[0]) {
#ifdef WIN32
            // if it happening on window this seem that this path msut be a relative path but with an error
            vPath = vPath.substr(1);  // bad formated path go relative
#endif
        } else {
#ifdef UNIX
            vPath = "/" + vPath;  // make it absolute
#endif
        }

        if (vPath.empty()) return vName + "." + vExt;

        return vPath + EZ_FILE_SLASH_TYPE + vName + "." + vExt;
    }

    std::string GetFPNE_WithPath(const std::string &vPath) {
        return GetFPNE_WithPathNameExt(vPath, name, ext);
    }

    std::string GetFPNE_WithPathName(const std::string &vPath, const std::string &vName) {
        return GetFPNE_WithPathNameExt(vPath, vName, ext);
    }

    std::string GetFPNE_WithPathExt(const std::string &vPath, const std::string &vExt) {
        return GetFPNE_WithPathNameExt(vPath, name, vExt);
    }

    std::string GetFPNE_WithName(const std::string &vName) {
        return GetFPNE_WithPathNameExt(path, vName, ext);
    }

    std::string GetFPNE_WithNameExt(const std::string &vName, const std::string &vExt) {
        return GetFPNE_WithPathNameExt(path, vName, vExt);
    }

    std::string GetFPNE_WithExt(const std::string &vExt) {
        return GetFPNE_WithPathNameExt(path, name, vExt);
    }
};

inline PathInfos parsePathFileName(const std::string &vPathFileName) {
    PathInfos res;
    if (!vPathFileName.empty()) {
        const std::string pfn = correctSlashTypeForFilePathName(vPathFileName);
        if (!pfn.empty()) {
            const size_t lastSlash = pfn.find_last_of(EZ_FILE_SLASH_TYPE);
            if (lastSlash != std::string::npos) {
                res.name = pfn.substr(lastSlash + 1);
                res.path = pfn.substr(0, lastSlash);
                res.isOk = true;
            }
            const size_t lastPoint = pfn.find_last_of('.');
            if (lastPoint != std::string::npos) {
                if (!res.isOk) {
                    res.name = pfn;
                    res.isOk = true;
                }
                res.ext = pfn.substr(lastPoint + 1);
                ez::str::replaceString(res.name, "." + res.ext, "");
            }
            if (!res.isOk) {
                res.name = pfn;
                res.isOk = true;
            }
        }
    }
    return res;
}

inline std::string simplifyFilePath(const std::string &vFilePath) {
    std::string newPath = correctSlashTypeForFilePathName(vFilePath);
    // the idea is to simplify a path where there is some ..
    // by ex : script\\kifs\\../space3d.glsl => can be simplified in /script/space3d.glsl
    size_t dpt = 0;
    while ((dpt = newPath.find("..")) != std::string::npos) {
        ez::str::replaceString(newPath, "\\", EZ_FILE_SLASH_TYPE);
        ez::str::replaceString(newPath, "/", EZ_FILE_SLASH_TYPE);
        size_t sl = newPath.rfind(EZ_FILE_SLASH_TYPE, dpt);
        if (sl != std::string::npos) {
            if (sl > 0) {
                sl = newPath.rfind(EZ_FILE_SLASH_TYPE, sl - 1);
                if (sl != std::string::npos) {
                    std::string str = newPath.substr(sl, dpt + 2 - sl);
                    ez::str::replaceString(newPath, str, "");
                }
            }
        } else {
            break;
        }
    }
    return newPath;
}

inline std::string composePath(const std::string &vPath, const std::string &vFileName, const std::string &vExt) {
    const auto path = correctSlashTypeForFilePathName(vPath);
    std::string res = path;
    if (!vFileName.empty()) {
        if (!path.empty()) {
            res += EZ_FILE_SLASH_TYPE;
        }
        res += vFileName;
        if (!vExt.empty()) {
            res += "." + vExt;
        }
    }
    return res;
}

inline bool isFileExist(const std::string &name) {
    auto fileToOpen = correctSlashTypeForFilePathName(name);
    ez::str::replaceString(fileToOpen, "\"", "");
    ez::str::replaceString(fileToOpen, "\n", "");
    ez::str::replaceString(fileToOpen, "\r", "");
    std::ifstream docFile(fileToOpen, std::ios::in);
    if (docFile.is_open()) {
        docFile.close();
        return true;
    }
    return false;
}

inline bool isDirectoryExist(const std::string &name) {
    if (!name.empty()) {
        const std::string dir = correctSlashTypeForFilePathName(name);
        struct stat sb;
        if (stat(dir.c_str(), &sb)) {
            return (sb.st_mode & S_IFDIR);
        }
    }
    return false;
}

inline bool destroyFile(const std::string &vFilePathName) {
    if (!vFilePathName.empty()) {
        const auto filePathName = correctSlashTypeForFilePathName(vFilePathName);
        if (isFileExist(filePathName)) {
            if (remove(filePathName.c_str()) == 0) {
                return true;
            }
        }
    }
    return false;
}

inline bool createDirectoryIfNotExist(const std::string &name) {
    bool res = false;
    if (!name.empty()) {
        const auto filePathName = correctSlashTypeForFilePathName(name);
        if (!isDirectoryExist(filePathName)) {
            res = true;
#ifdef WIN32
            CreateDirectory(filePathName.c_str(), nullptr);
#elif defined(UNIX)
            auto cmd = ez::str::toStr("mkdir -p %s", filePathName.c_str());
            const int dir_err = std::system(cmd.c_str());
            if (dir_err == -1) {
                LogVarError("Error creating directory %s", filePathName.c_str());
                res = false;
            }
#endif
        }
    }
    return res;
}

inline bool createPathIfNotExist(const std::string &vPath) {
    bool res = false;
    if (!vPath.empty()) {
        auto path = correctSlashTypeForFilePathName(vPath);
        if (!isDirectoryExist(path)) {
            res = true;
            ez::str::replaceString(path, "/", "|");
            ez::str::replaceString(path, "\\", "|");
            auto arr = ez::str::splitStringToVector(path, "|");
            std::string fullPath;
            for (auto it = arr.begin(); it != arr.end(); ++it) {
                fullPath += *it;
                res &= createDirectoryIfNotExist(fullPath);
                fullPath += EZ_FILE_SLASH_TYPE;
            }
        }
    }
    return res;
}

// will open the file is the associated app
inline void openFile(const std::string &vFile) {
    const auto file = correctSlashTypeForFilePathName(vFile);
#if defined(WIN32)
    auto *result = ShellExecute(nullptr, "", file.c_str(), nullptr, nullptr, SW_SHOW);
    if (result < (HINSTANCE)32) {  //-V112
        // try to open an editor
        result = ShellExecute(nullptr, "edit", file.c_str(), nullptr, nullptr, SW_SHOW);
        if (result == (HINSTANCE)SE_ERR_ASSOCINCOMPLETE || result == (HINSTANCE)SE_ERR_NOASSOC) {
            // open associating dialog
            const std::string sCmdOpenWith = "shell32.dll,OpenAs_RunDLL \"" + file + "\"";
            result = ShellExecute(nullptr, "", "rundll32.exe", sCmdOpenWith.c_str(), nullptr, SW_NORMAL);
        }
        if (result < (HINSTANCE)32) {  // open in explorer //-V112
            const std::string sCmdExplorer = "/select,\"" + file + "\"";
            ShellExecute(
                nullptr, "", "explorer.exe", sCmdExplorer.c_str(), nullptr, SW_NORMAL);  // ce serait peut etre mieu d'utilsier la commande system comme dans SelectFile
        }
    }
#elif defined(LINUX)
    int pid = fork();
    if (pid == 0) {
        execl("/usr/bin/xdg-open", "xdg-open", file.c_str(), (char *)0);
    }
#elif defined(APPLE)
    std::string cmd = "open " + file;
    std::system(cmd.c_str());
#endif
}

// will open the url in the related browser
inline void openUrl(const std::string &vUrl) {
    const auto url = correctSlashTypeForFilePathName(vUrl);
#ifdef WIN32
    ShellExecute(nullptr, nullptr, url.c_str(), nullptr, nullptr, SW_SHOW);
#elif defined(LINUX)
    auto cmd = ez::str::toStr("<mybrowser> %s", url.c_str());
    std::system(cmd.c_str());
#elif defined(APPLE)
    // std::string sCmdOpenWith = "open -a Firefox " + vUrl;
    std::string cmd = "open " + url;
    std::system(cmd.c_str());
#endif
}

// will open the current file explorer and will select the file
inline void selectFile(const std::string &vFileToSelect) {
    const auto fileToSelect = correctSlashTypeForFilePathName(vFileToSelect);
#ifdef WIN32
    if (!fileToSelect.empty()) {
        const std::string sCmdOpenWith = "explorer /select," + fileToSelect;
        std::system(sCmdOpenWith.c_str());
    }
#elif defined(LINUX)
    // todo : is there a similar cmd on linux ?
    assert(nullptr);
#elif defined(APPLE)
    if (!fileToSelect.empty()) {
        std::string cmd = "open -R " + fileToSelect;
        std::system(cmd.c_str());
    }
#endif
}

inline std::vector<std::string> getDrives() {
    std::vector<std::string> res;
#ifdef WIN32
    const DWORD mydrives = 2048;
    char lpBuffer[2048];
    const DWORD countChars = GetLogicalDriveStrings(mydrives, lpBuffer);
    if (countChars > 0) {
        std::string var = std::string(lpBuffer, (size_t)countChars);
        ez::str::replaceString(var, "\\", "");
        res = ez::str::splitStringToVector(var, "\0");
    }
#else
    assert(nullptr);
#endif
    return res;
}

}  // namespace file
}  // namespace ez
