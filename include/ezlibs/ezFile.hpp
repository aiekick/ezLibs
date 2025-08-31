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

// ezFile is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <map>
#include <set>
#include <array>
#include <regex>
#include <mutex>
#include <ctime>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <memory>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <iterator>
#include <iostream>
#include <functional>
#include <unordered_map>

#include "ezOS.hpp"
#include "ezApp.hpp"
#include "ezStr.hpp"
#include "ezLog.hpp"
#include <sys/stat.h>

#ifdef WINDOWS_OS
#ifndef EZ_FILE_SLASH_TYPE
#define EZ_FILE_SLASH_TYPE "\\"
#endif  // EZ_FILE_SLASH_TYPE
#include <Windows.h>
#include <shellapi.h>  // ShellExecute
#define stat _stat
#else                // UNIX_OS
#include <unistd.h>  // rmdir
#ifndef EZ_FILE_SLASH_TYPE
#define EZ_FILE_SLASH_TYPE "/"
#endif  // EZ_FILE_SLASH_TYPE
#ifdef APPLE_OS
#include <CoreServices/CoreServices.h>
#else
#include <sys/inotify.h>
#endif
#endif

namespace ez {
namespace file {

inline std::string loadFileToString(const std::string &vFilePathName, bool vVerbose = true) {
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
        if (vVerbose) {
#ifdef EZ_TOOLS_LOG
            LogVarError("File \"%s\" Not Found !\n", vFilePathName.c_str());
#endif
        }
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
    std::ifstream file(vFilePathName, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        std::streamsize size = file.tellg();  // taille du fichier
        if (size <= 0) {
            return ret;
        }
        ret.resize(static_cast<size_t>(size));
        file.seekg(0, std::ios::beg);
        if (!file.read(reinterpret_cast<char *>(&ret[0]), size)) {
            ret.clear();
        }
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
        std::ofstream out(fpn, std::ios::binary | std::ios::trunc);
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

    PathInfos() { isOk = false; }

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

    std::string GetFPNE() { return GetFPNE_WithPathNameExt(path, name, ext); }

    std::string GetFPNE_WithPathNameExt(std::string vPath, const std::string &vName, const std::string &vExt) {
        if (vPath[0] == EZ_FILE_SLASH_TYPE[0]) {
#ifdef WINDOWS_OS
            // if it happening on window this seem that this path msut be a relative path but with an error
            vPath = vPath.substr(1);  // bad formated path go relative
#endif
        } else {
#ifdef UNIX_OS
            vPath = "/" + vPath;  // make it absolute
#endif
        }

        if (vPath.empty())
            return vName + "." + vExt;

        return vPath + EZ_FILE_SLASH_TYPE + vName + "." + vExt;
    }

    std::string GetFPNE_WithPath(const std::string &vPath) { return GetFPNE_WithPathNameExt(vPath, name, ext); }

    std::string GetFPNE_WithPathName(const std::string &vPath, const std::string &vName) { return GetFPNE_WithPathNameExt(vPath, vName, ext); }

    std::string GetFPNE_WithPathExt(const std::string &vPath, const std::string &vExt) { return GetFPNE_WithPathNameExt(vPath, name, vExt); }

    std::string GetFPNE_WithName(const std::string &vName) { return GetFPNE_WithPathNameExt(path, vName, ext); }

    std::string GetFPNE_WithNameExt(const std::string &vName, const std::string &vExt) { return GetFPNE_WithPathNameExt(path, vName, vExt); }

    std::string GetFPNE_WithExt(const std::string &vExt) { return GetFPNE_WithPathNameExt(path, name, vExt); }
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
    std::string path = correctSlashTypeForFilePathName(vFilePath);
    std::vector<std::string> parts;
    std::stringstream ss(path);
    std::string item;

    while (std::getline(ss, item, EZ_FILE_SLASH_TYPE[0])) {
        if (item == "" || item == ".")
            continue;
        if (item == "..") {
            if (!parts.empty())
                parts.pop_back();
            // sinon on est au-dessus de root => on ignore
        } else {
            parts.push_back(item);
        }
    }

    std::string result;
    for (size_t i = 0; i < parts.size(); ++i) {
        result += parts[i];
        if (i < parts.size() - 1)
            result += EZ_FILE_SLASH_TYPE;
    }
    return result;
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
        if (stat(dir.c_str(), &sb) == 0) {
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

inline bool destroyDir(const std::string &vPath) {
    if (!vPath.empty()) {
        if (isDirectoryExist(vPath)) {
#ifdef WINDOWS_OS
            return (RemoveDirectoryA(vPath.c_str()) != 0);
#elif defined(UNIX_OS)
            return (rmdir(vPath.c_str()) == 0);
#endif
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
#ifdef WINDOWS_OS
            if (CreateDirectory(filePathName.c_str(), nullptr) == 0) {
                res = true;
            }
#elif defined(UNIX_OS)
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
#if defined(WINDOWS_OS)
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
#elif defined(LINUX_OS)
    int pid = fork();
    if (pid == 0) {
        execl("/usr/bin/xdg-open", "xdg-open", file.c_str(), (char *)0);
    }
#elif defined(APPLE_OS)
    std::string cmd = "open " + file;
    std::system(cmd.c_str());
#endif
}

// will open the url in the related browser
inline void openUrl(const std::string &vUrl) {
    const auto url = correctSlashTypeForFilePathName(vUrl);
#ifdef WINDOWS_OS
    ShellExecute(nullptr, nullptr, url.c_str(), nullptr, nullptr, SW_SHOW);
#elif defined(LINUX_OS)
    auto cmd = ez::str::toStr("<mybrowser> %s", url.c_str());
    std::system(cmd.c_str());
#elif defined(APPLE_OS)
    // std::string sCmdOpenWith = "open -a Firefox " + vUrl;
    std::string cmd = "open " + url;
    std::system(cmd.c_str());
#endif
}

// will open the current file explorer and will select the file
inline void selectFile(const std::string &vFileToSelect) {
    const auto fileToSelect = correctSlashTypeForFilePathName(vFileToSelect);
#ifdef WINDOWS_OS
    if (!fileToSelect.empty()) {
        const std::string sCmdOpenWith = "explorer /select," + fileToSelect;
        std::system(sCmdOpenWith.c_str());
    }
#elif defined(LINUX_OS)
    // todo : is there a similar cmd on linux ?
    assert(nullptr);
#elif defined(APPLE_OS)
    if (!fileToSelect.empty()) {
        std::string cmd = "open -R " + fileToSelect;
        std::system(cmd.c_str());
    }
#endif
}

inline std::vector<std::string> getDrives() {
    std::vector<std::string> res;
#ifdef WINDOWS_OS
    const DWORD mydrives = 2048;
    char lpBuffer[2048 + 1];
    const DWORD countChars = GetLogicalDriveStrings(mydrives, lpBuffer);
    if (countChars > 0) {
        std::string var = std::string(lpBuffer, (size_t)countChars);
        ez::str::replaceString(var, "\\", "");
        res = ez::str::splitStringToVector(var, "\0");
    }
#elif defined(UNIX_OS)
    assert(nullptr);
#endif
    return res;
}

class Watcher {
public:
    struct PathResult {
        // path are relative to rootDir
        std::string rootPath;   // the watched dir
        std::string oldPath;    // before rneaming
        std::string newPath;    // after renaming, creation, deletion or modification
        enum class ModifType {  // type of change
            NONE = 0,
            MODIFICATION,
            CREATION,
            DELETION,
            RENAMED
        } modifType = ModifType::NONE;
        void clear() { *this = {}; }
        // Needed for std::set, defines strict weak ordering
        bool operator<(const PathResult &other) const {
            if (newPath != other.newPath) {
                return newPath < other.newPath;
            }
            return modifType < other.modifType;
        }
    };
    using Callback = std::function<void(const std::set<PathResult> &)>;

private:
    bool m_verbose{};
    std::string m_appPath;
    Callback m_callback;
    std::mutex m_mutex;
    std::thread m_thread;
    std::atomic<bool> m_running;

#ifdef WINDOWS_OS
    typedef std::wstring PathType;
#else
    typedef std::string PathType;
#endif
    struct Pattern;
    using PatternPtr = std::shared_ptr<Pattern>;
    using PatternWeak = std::weak_ptr<Pattern>;
    class Pattern {
    public:
        enum class PatternType {  //
            PATH = 0,
            GLOB,  // glob is pattern with wildcard : ex toto_*_tata_*_.txt
            REGEX
        };
        enum class PhysicalType {  //
            DIR = 0,               // directory watcher
            FILE                   // file watcher
        };

    public:
        static std::shared_ptr<Pattern> sCreatePath(  //
            const std::string &vPath,
            PatternType vPatternType,
            PhysicalType vPhysicalType) {
            if (vPath.empty()) {
                return nullptr;
            }
            auto pRet = std::make_shared<Pattern>();
            pRet->m_path = vPath;
            pRet->m_patternType = vPatternType;
            pRet->m_physicalType = vPhysicalType;
            return pRet;
        }
        static std::shared_ptr<Pattern> sCreatePathFile(  //
            const std::string &vRootPath,
            const std::string &vFileNameExt,
            PatternType vPatternType,
            PhysicalType vPhysicalType) {
            if (vRootPath.empty() || vFileNameExt.empty()) {
                return nullptr;
            }
            auto pRet = std::make_shared<Pattern>();
            pRet->m_path = vRootPath;
            pRet->m_fileNameExt = vFileNameExt;
            pRet->m_patternType = vPatternType;
            pRet->m_physicalType = vPhysicalType;
            return pRet;
        }

    private:
        std::string m_path;
        std::string m_fileNameExt;
        PatternType m_patternType = PatternType::PATH;
        PhysicalType m_physicalType = PhysicalType::DIR;

    public:
        const std::string &getPath() const { return m_path; }
        const std::string &getFileNameExt() const { return m_fileNameExt; }
        PatternType getPatternType() const { return m_patternType; }
        PhysicalType getPhysicalType() const { return m_physicalType; }
        bool isPatternMatch(const std::string &vPath, bool vVerbose) const {
            bool ret = false;
            switch (m_patternType) {
                case PatternType::PATH: {
                    ret = (m_path == vPath);
                } break;
                case PatternType::GLOB: {
                    return m_isGlobPatternMatch(vPath);
                } break;
                case PatternType::REGEX: {
                    return m_isRegexPatternMatch(vPath, vVerbose);
                } break;
                default: break;
            }
            return ret;
        }

    private:
        bool m_isGlobPatternMatch(const std::string &vPath) const { return !ez::str::searchForPatternWithWildcards(vPath, m_fileNameExt).empty(); }
        bool m_isRegexPatternMatch(const std::string &vPath, bool vVerbose) const {
            try {
                std::regex pattern(m_fileNameExt);
                return std::regex_match(vPath, pattern);
            } catch (const std::regex_error &ex) {
                if (vVerbose) {
#ifdef EZ_TOOLS_LOG
                    LogVarError("Err : regex not match : %s", ex.what());
#endif
                }
            }
            return false;
        }
    };
    std::vector<PatternPtr> m_watchPatterns;

    struct WatchHandle {
        HANDLE hDir;
        PathType path;
        std::vector<PatternWeak> relatedPatterns;
        HANDLE hEvent{};
        OVERLAPPED ov{};
        std::array<uint8_t, 4096> buffer;
        bool inFlight{false};
    };
    std::unordered_map<std::string, WatchHandle> m_watchHandles;
    std::atomic<bool> m_isHandlesDirty{false};

public:
    Watcher() : m_callback(nullptr), m_running(false), m_appPath(m_getAppPath()) {}
    ~Watcher() { stop(); }

    void setVerbose(const bool vVerbose) { m_verbose = vVerbose; }
    void setCallback(Callback vCallback) { m_callback = vCallback; }

    // watch a directory. can be absolute or relative the to the app
    // no widlcards or regex are supported for the directory
    bool watchDirectory(const std::string &vPath) {  //
        return m_watchPattern(Pattern::sCreatePath(vPath, Pattern::PatternType::PATH, Pattern::PhysicalType::DIR));
    }

    // will watch from a path a filename.
    // vRootPath : no widlcards or regex are supported
    // vFileNameExt : can contain wildcards. regex patterns is authorized but must start with a '(' and end with a ')'
    bool watchFile(const std::string &vRootPath, const std::string &vFileNameExt) {  //
        if (!vFileNameExt.empty()) {
            if (vFileNameExt.front() == '(' && vFileNameExt.back() == ')') {
                return m_watchPattern(Pattern::sCreatePathFile(vRootPath, vFileNameExt, Pattern::PatternType::REGEX, Pattern::PhysicalType::FILE));
            } else if (vFileNameExt.find('*') != std::string::npos) {
                return m_watchPattern(Pattern::sCreatePathFile(vRootPath, vFileNameExt, Pattern::PatternType::GLOB, Pattern::PhysicalType::FILE));
            } else {
                return m_watchPattern(Pattern::sCreatePathFile(vRootPath, vFileNameExt, Pattern::PatternType::PATH, Pattern::PhysicalType::FILE));
            }
        }
        return false;
    }

    bool start() {
        if (m_running || m_callback == nullptr) {
            return false;
        }
        m_running = true;

#ifdef WINDOWS_OS
        m_thread = std::thread(&Watcher::m_watchWindows, this);
#elif defined(LINUX_OS)
        m_thread = std::thread(&Watcher::m_watchLinux, this);
#elif defined(APPLE_OS)
        m_thread = std::thread(&Watcher::m_watchMacOS, this);
#endif
        return true;
    }
    bool stop() {
        if (!m_running) {
            return false;
        }
        m_running = false;

        if (m_thread.joinable()) {
            m_thread.join();
        }

        // Cancel pending I/O and close handles
        for (auto &kv : m_watchHandles) {
            auto &h = kv.second;
            if (h.inFlight) {
                CancelIoEx(h.hDir, &h.ov);  // safe if no I/O too
            }
            if (h.hEvent) {
                CloseHandle(h.hEvent);
                h.hEvent = nullptr;
            }
            if (h.hDir && h.hDir != INVALID_HANDLE_VALUE) {
                CloseHandle(h.hDir);
                h.hDir = INVALID_HANDLE_VALUE;
            }
        }
        m_watchHandles.clear();
        return true;
    }

private:
    std::string m_getAppPath() { return ez::App().getAppPath(); }
    bool m_watchPattern(const PatternPtr &vPatternPtr) {
        if (vPatternPtr != nullptr) {
            if (m_createHandleIfNeeded(vPatternPtr)) {
                {
                    m_isHandlesDirty = true;
                    std::lock_guard<std::mutex> _{m_mutex};  // protect access to m_watchPatterns
                    m_watchPatterns.push_back(vPatternPtr);
                }
                return true;
            }
        }
        return false;
    }
    std::string m_removeAppPath(const std::string &vPath) {
        size_t pos = vPath.find(m_appPath);
        if (pos != std::string::npos) {
            return vPath.substr(pos + m_appPath.size());
        }
        return vPath;
    }

    bool m_createHandleIfNeeded(const PatternWeak &vPattern) {
        bool ret = false;
        auto ptr = vPattern.lock();
        if (ptr != nullptr) {
            const auto &path = m_removeAppPath(ptr->getPath());
            std::lock_guard<std::mutex> _{m_mutex};  // protect access to m_watchHandles
            auto it = m_watchHandles.find(path);
            if (it != m_watchHandles.end()) {
                it->second.relatedPatterns.push_back(vPattern);
                ret = true;
            } else {
#ifdef WINDOWS_OS
                WatchHandle hnd;
                hnd.path = ez::str::utf8Decode(path);
                hnd.hDir = CreateFileW(  //
                    hnd.path.c_str(),
                    FILE_LIST_DIRECTORY,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    NULL,
                    OPEN_EXISTING,
                    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                    NULL);
                if (hnd.hDir != INVALID_HANDLE_VALUE) {
                    hnd.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
                    hnd.ov.hEvent = hnd.hEvent;
                    ZeroMemory(&hnd.ov, sizeof(hnd.ov));
                    hnd.inFlight = false;
                    hnd.relatedPatterns.push_back(vPattern);
                    m_watchHandles[path] = hnd;
                    // *** Arm immediately to avoid missing the first changes ***
                    auto &ref = m_watchHandles[path];
                    m_postRead(ref);
                    ret = true;
                } else {
#ifdef EZ_TOOLS_LOG
                    LogVarError("Err : Unable to open directory : %s", path.c_str());
#endif
                }
#elif defined(LINUX_OS)
                // todo
#elif defined(APPLE_OS)
                // todo
#endif
            }
        }
        return ret;
    }

    static std::string m_getParentDir(const std::string &path) {
        size_t slash = path.find_last_of("/\\");
        if (slash == std::string::npos) {
            return ".";
        }
        return path.substr(0, slash);
    }

    static std::string m_getFileNameExtOnly(const std::string &path) {
        size_t slash = path.find_last_of("/\\");
        if (slash == std::string::npos) {
            return path;
        }
        return path.substr(slash + 1);
    }

#ifdef WINDOWS_OS
    void completePathResult(PathResult &voResult, const FILE_NOTIFY_INFORMATION *vpNotify) {
        const auto chFile = m_removeAppPath(ez::str::utf8Encode(std::wstring(vpNotify->FileName, vpNotify->FileNameLength / sizeof(WCHAR))));
        const char *mode = " ";
        switch (vpNotify->Action) {
            case FILE_ACTION_ADDED: {
                voResult.modifType = PathResult::ModifType::CREATION;
                voResult.newPath = chFile;
#ifdef _DEBUG
                mode = "CREATION";
#endif
            } break;
            case FILE_ACTION_REMOVED: {
                voResult.modifType = PathResult::ModifType::DELETION;
                voResult.newPath = chFile;
#ifdef _DEBUG
                mode = "DELETION";
#endif
            } break;
            case FILE_ACTION_MODIFIED: {
                voResult.modifType = PathResult::ModifType::MODIFICATION;
                voResult.newPath = chFile;
#ifdef _DEBUG
                mode = "MODIFICATION";
#endif
            } break;
            case FILE_ACTION_RENAMED_OLD_NAME: {
                voResult.modifType = PathResult::ModifType::RENAMED;
                voResult.oldPath = chFile;
#ifdef _DEBUG
                mode = "RENAMED";
#endif
            } break;
            case FILE_ACTION_RENAMED_NEW_NAME: {
                voResult.modifType = PathResult::ModifType::RENAMED;
                voResult.newPath = chFile;
#ifdef _DEBUG
                mode = "RENAMED";
#endif
            } break;
        }
#ifdef _DEBUG
        LogVarLightInfo("completePathResult : RP(%s) OP(%s) NP(%s) MODE(%s)", voResult.rootPath.c_str(), voResult.oldPath.c_str(), voResult.newPath.c_str(), mode);
        #endif
    }
    bool m_postRead(WatchHandle &hnd) {
        // thread running
        /*if (!m_running) {
            return false;
        }*/
        // Repost only when no I/O is pending
        if (hnd.inFlight) {
            return true;
        }

        // Reset event & OVERLAPPED before issuing a new read
        ResetEvent(hnd.hEvent);
        ZeroMemory(&hnd.ov, sizeof(hnd.ov));
        hnd.ov.hEvent = hnd.hEvent;

        const DWORD kMask = FILE_NOTIFY_CHANGE_FILE_NAME |  // file create/delete/rename in root dir
            FILE_NOTIFY_CHANGE_LAST_WRITE;                  // file content modifications

        // In OVERLAPPED mode, lpBytesReturned MUST be nullptr
        BOOL ok = ReadDirectoryChangesW(
            hnd.hDir,
            hnd.buffer.data(),
            static_cast<DWORD>(hnd.buffer.size()),
            FALSE,  // do NOT watch subtree
            kMask,
            nullptr,  // must be null for OVERLAPPED
            &hnd.ov,
            nullptr);

        if (!ok) {
            DWORD err = GetLastError();
            if (err != ERROR_IO_PENDING) {
                LogVarWarning("ReadDirectoryChangesW(OVERLAPPED) failed: %u", err);
                return false;
            }
        }

        hnd.inFlight = true;
        return true;
    }
    // In class Watcher (private)
    void m_pollOneHandle(WatchHandle *vpHandle, std::set<PathResult> &voFiles) {
        // Non-blocking poll of a single handle; parse results and re-arm the async read.
        if (vpHandle == nullptr) {
            return;
        }

        // 2) Poll event (non-blocking)
        DWORD wr = WaitForSingleObject(vpHandle->hEvent, 0);
#ifdef _DEBUG
        switch (wr) {
            case WAIT_ABANDONED: {
                LogVarLightInfo("WAIT_ABANDONED");
            } break;
            case WAIT_OBJECT_0: {
               // LogVarLightInfo("WAIT_OBJECT_0");
            } break;
            case WAIT_TIMEOUT: {
             //   LogVarLightInfo("WAIT_TIMEOUT");
            } break;
            case WAIT_FAILED: {
                LogVarLightInfo("WAIT_FAILED");
            } break;
        }
#endif
        if (wr != WAIT_OBJECT_0) {
            // Nothing ready for this handle
            return;
        }

        // I/O finished
        DWORD bytes = 0;
        if (GetOverlappedResult(vpHandle->hDir, &vpHandle->ov, &bytes, FALSE)) {
            // Parse buffer [0..bytes)
            DWORD offset = 0;
            PathResult pr{};
            pr.clear();
            pr.rootPath = ez::str::utf8Encode(vpHandle->path);

            while (offset < bytes) {
                const auto *pNotify = reinterpret_cast<const FILE_NOTIFY_INFORMATION *>(vpHandle->buffer.data() + offset);
                completePathResult(pr, pNotify);
                // Only emit when we have a valid newPath (pairs OLD/NEW may come split)
                if (!pr.newPath.empty()) {
                    for (const auto &patW : vpHandle->relatedPatterns) {
                        if (auto pPat = patW.lock()) {
                            if (pPat->getPhysicalType() == Pattern::PhysicalType::DIR) {
                                voFiles.emplace(pr);
#ifdef _DEBUG
                                const char *mode = " ";
                                switch (pr.modifType) {
                                    case PathResult::ModifType::CREATION: mode = "CREATION"; break;
                                    case PathResult::ModifType::DELETION: mode = "DELETION"; break;
                                    case PathResult::ModifType::MODIFICATION: mode = "MODIFICATION"; break;
                                    case PathResult::ModifType::RENAMED: mode = "RENAMED"; break;
                                }
                                LogVarLightInfo("Event : RP(%s) OP(%s) NP(%s) MODE(%s)", pr.rootPath.c_str(), pr.oldPath.c_str(), pr.newPath.c_str(), mode);
#endif
                            } else {
                                if (pPat->isPatternMatch(pr.newPath, m_verbose)) {
                                    voFiles.emplace(pr);
                                }
                            }
                        }
                    }
                }
                if (pNotify->NextEntryOffset == 0) {
                    break;
                }
                offset += pNotify->NextEntryOffset;
            }
        } else {
            LogVarWarning("GetOverlappedResult failed: %u", GetLastError());
        }

        // Mark as not in-flight and immediately re-arm
        vpHandle->inFlight = false;
        (void)m_postRead(*vpHandle);
    }
    void m_watchWindows() {
        std::set<PathResult> files;
        PathResult pr;

        // Persistent snapshot of handle pointers (avoids copying WatchHandle)
        std::vector<WatchHandle *> handles;

        while (m_running) {
            // Refresh handle list only when dirty
            if (m_isHandlesDirty.exchange(false)) {
                std::lock_guard<std::mutex> _{m_mutex};
                handles.clear();
                handles.reserve(m_watchHandles.size());
                for (auto &kv : m_watchHandles) {
                    handles.push_back(&kv.second);  // store pointer
                }
            }

            // For each handle: post async read if not in-flight, then poll
            for (auto *hnd : handles) {
                // 1) Arm async read if none in flight
                if (!hnd->inFlight) {
                    m_postRead(*hnd);
                }

                // 2) Poll event (non-blocking)
                m_pollOneHandle(hnd, files);
            }

            if (!files.empty()) {
                m_callback(files);
                files.clear();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
#elif defined(LINUX_OS)
    void m_watchLinux() {
        int fd = inotify_init1(IN_NONBLOCK);
        if (fd < 0) {
            std::cerr << "Error: Unable to initialize inotify.\n";
            return;
        }

        int wd = inotify_add_watch(fd, m_filePathName.c_str(), IN_MODIFY);
        if (wd < 0) {
            std::cerr << "Error: Unable to add watch on " << m_filePathName << "\n";
            close(fd);
            return;
        }

        std::array<char, 4096> buffer;

        while (m_running) {
            int length = read(fd, buffer.data(), buffer.size());
            if (length > 0 && m_callback) {
                m_callback({m_filePathName});
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        inotify_rm_watch(fd, wd);
        close(fd);
    }

#elif defined(APPLE_OS)
    static void
    m_fileChangedCallback(ConstFSEventStreamRef, void *userData, size_t numEvents, void *eventPaths, const FSEventStreamEventFlags *, const FSEventStreamEventId *) {
        FileWatcher *watcher = static_cast<FileWatcher *>(userData);
        if (!watcher->m_running)
            return;

        char **paths = static_cast<char **>(eventPaths);
        std::vector<std::string> changedFiles;
        for (size_t i = 0; i < numEvents; ++i) {
            std::string changedPath = paths[i];
            if (changedPath == watcher->m_filePathName) {
                changedFiles.push_back(changedPath);
            }
        }

        if (!changedFiles.empty() && watcher->m_callback) {
            watcher->m_callback(changedFiles);
        }
    }

    void m_watchMacOS() {
        std::string dir = getParentDir(m_filePathName);
        CFStringRef path = CFStringCreateWithCString(NULL, dir.c_str(), kCFStringEncodingUTF8);
        CFArrayRef pathsToWatch = CFArrayCreate(NULL, (const void **)&path, 1, NULL);

        FSEventStreamContext context = {0, this, NULL, NULL, NULL};
        FSEventStreamRef stream =
            FSEventStreamCreate(NULL, &m_fileChangedCallback, &context, pathsToWatch, kFSEventStreamEventIdSinceNow, 0.5, kFSEventStreamCreateFlagNone);

        if (!stream) {
            std::cerr << "Error: Unable to create FSEventStream.\n";
            CFRelease(path);
            CFRelease(pathsToWatch);
            return;
        }

        FSEventStreamScheduleWithRunLoop(stream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
        FSEventStreamStart(stream);

        while (m_running) {
            CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.5, true);
        }

        FSEventStreamStop(stream);
        FSEventStreamInvalidate(stream);
        FSEventStreamRelease(stream);
        CFRelease(path);
        CFRelease(pathsToWatch);
    }
#endif
};

}  // namespace file
}  // namespace ez
