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
#ifndef __EMSCRIPTEN__
#include <sys/inotify.h>
#endif
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

    std::string GetFPNE() const { return GetFPNE_WithPathNameExt(path, name, ext); }

    std::string GetFPNE_WithPathNameExt(std::string vPath, const std::string &vName, const std::string &vExt) const {
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
        std::string ext = vExt;
        if (!ext.empty()) {
            ext = '.' + ext;
        }

        if (vPath.empty()) {
            return vName + ext;
        }

        return vPath + EZ_FILE_SLASH_TYPE + vName + ext;
    }

    std::string GetFPNE_WithPath(const std::string &vPath) const { return GetFPNE_WithPathNameExt(vPath, name, ext); }

    std::string GetFPNE_WithPathName(const std::string &vPath, const std::string &vName) const { return GetFPNE_WithPathNameExt(vPath, vName, ext); }

    std::string GetFPNE_WithPathExt(const std::string &vPath, const std::string &vExt) { return GetFPNE_WithPathNameExt(vPath, name, vExt); }

    std::string GetFPNE_WithName(const std::string &vName) const { return GetFPNE_WithPathNameExt(path, vName, ext); }

    std::string GetFPNE_WithNameExt(const std::string &vName, const std::string &vExt) const { return GetFPNE_WithPathNameExt(path, vName, vExt); }

    std::string GetFPNE_WithExt(const std::string &vExt) const { return GetFPNE_WithPathNameExt(path, name, vExt); }
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

#ifndef EMSCRIPTEN_OS

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
    std::string m_appPath;
    Callback m_callback;
    std::thread m_thread;
    std::atomic<bool> m_running{false};

    class Pattern;
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

        bool isPatternMatch(const std::string &vPath) const {
            switch (m_patternType) {
                case PatternType::PATH: {
                    return (!m_fileNameExt.empty()) ? (m_fileNameExt == vPath) : (m_path == vPath);
                }
                case PatternType::GLOB: {
                    return !ez::str::searchForPatternWithWildcards(vPath, m_fileNameExt).empty();
                }
                case PatternType::REGEX: {
                    try {
                        std::regex pattern(m_fileNameExt);
                        return std::regex_match(vPath, pattern);
                    } catch (...) {
                        return false;
                    }
                }
            }
            return false;
        }
    };

    std::mutex m_patternsMutex;
    std::vector<PatternPtr> m_watchPatterns;

    class IBackend {
    public:
        virtual ~IBackend() = default;
        virtual bool onStart(Watcher &owner) = 0;
        virtual void onStop(Watcher &owner) = 0;
        virtual bool addPattern(Watcher &owner, const PatternPtr &pattern) = 0;
        virtual void poll(Watcher &owner, std::set<PathResult> &out) = 0;
    };

    std::unique_ptr<IBackend> m_backend;

    static void m_logPathResult(const PathResult &vPathResult) {
#ifdef _DEBUG
        const char *mode = " ";
        switch (vPathResult.modifType) {
            case PathResult::ModifType::CREATION: mode = "CREATION"; break;
            case PathResult::ModifType::DELETION: mode = "DELETION"; break;
            case PathResult::ModifType::MODIFICATION: mode = "MODIFICATION"; break;
            case PathResult::ModifType::RENAMED: mode = "RENAMED"; break;
            default: break;
        }
        LogVarLightInfo("Event : RP(%s) OP(%s) NP(%s) MODE(%s)", vPathResult.rootPath.c_str(), vPathResult.oldPath.c_str(), vPathResult.newPath.c_str(), mode);
#else
        (void)vPathResult;
#endif
    }

    static void m_emitIfMatch(  //
        const std::string &rootKey,
        const std::vector<PatternWeak> &relatedPatterns,
        const std::string &relNewName,
        PathResult &pr,
        std::set<PathResult> &out) {
        pr.rootPath = rootKey;
        for (const auto &pw : relatedPatterns) {
            if (auto p = pw.lock()) {
                if (p->getPhysicalType() == Pattern::PhysicalType::DIR) {
                    out.emplace(pr);
                    m_logPathResult(pr);
                } else {
                    if (!relNewName.empty() && p->isPatternMatch(relNewName)) {
                        out.emplace(pr);
                        m_logPathResult(pr);
                    }
                }
            }
        }
    }

    std::string m_getAppPath() { return ez::App().getAppPath(); }
    std::string m_removeAppPath(const std::string &vPath) {
        size_t pos = vPath.find(m_appPath);
        if (pos != std::string::npos) {
            return vPath.substr(pos + m_appPath.size());
        }
        return vPath;
    }

public:
    Watcher() : m_appPath(m_getAppPath()) {}
    ~Watcher() { stop(); }

    void setCallback(Callback vCallback) { m_callback = vCallback; }

    // watch a directory. can be absolute or relative the to the app
    // no widlcards or regex are supported for the directory
    bool watchDirectory(const std::string &vPath) { return m_registerPattern(Pattern::sCreatePath(vPath, Pattern::PatternType::PATH, Pattern::PhysicalType::DIR)); }

    // will watch from a path a filename.
    // vRootPath : no widlcards or regex are supported
    // vFileNameExt : can contain wildcards. regex patterns is authorized but must start with a '(' and end with a ')'
    bool watchFile(const std::string &vRootPath, const std::string &vFileNameExt) {
        if (vRootPath.empty() || vFileNameExt.empty()) {
            return false;
        }
        Pattern::PatternType type = Pattern::PatternType::PATH;
        if (vFileNameExt.front() == '(' && vFileNameExt.back() == ')') {
            type = Pattern::PatternType::REGEX;
        } else if (vFileNameExt.find('*') != std::string::npos) {
            type = Pattern::PatternType::GLOB;
        }
        return m_registerPattern(Pattern::sCreatePathFile(vRootPath, vFileNameExt, type, Pattern::PhysicalType::FILE));
    }

    bool start() {
        if (m_running || m_callback == nullptr) {
            return false;
        }
        m_backend = m_createBackend();
        if (!m_backend) {
            return false;
        }
        if (!m_backend->onStart(*this)) {
            m_backend.reset();
            return false;
        }
        {
            std::lock_guard<std::mutex> lock(m_patternsMutex);
            for (const auto &p : m_watchPatterns) {
                (void)m_backend->addPattern(*this, p);
            }
        }
        m_running = true;
        m_thread = std::thread(&Watcher::m_threadLoop, this);
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
        if (m_backend) {
            m_backend->onStop(*this);
            m_backend.reset();
        }
        return true;
    }

private:
    bool m_registerPattern(const PatternPtr &pat) {
        if (!pat) {
            return false;
        }
        {
            std::lock_guard<std::mutex> lock(m_patternsMutex);
            m_watchPatterns.push_back(pat);
        }
        if (m_backend) {
            return m_backend->addPattern(*this, pat);
        }
        return true;
    }

    void m_threadLoop() {
        std::set<PathResult> files;
        while (m_running) {
            m_backend->poll(*this, files);
            if (!files.empty()) {
                m_callback(files);
                files.clear();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    std::unique_ptr<IBackend> m_createBackend();

    // =============================== Backend Windows ===============================
#if defined(WINDOWS_OS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
    class BackendWindows : public IBackend {
    public:
        typedef std::wstring PathType;
        struct WatchHandle {
            HANDLE hDir{};
            PathType path;
            std::vector<PatternWeak> relatedPatterns;
            HANDLE hEvent{};
            OVERLAPPED ov{};
            std::array<uint8_t, 4096> buffer{};
            bool inFlight{false};
        };

    private:
        std::mutex m_mtx;
        std::unordered_map<std::string, WatchHandle> m_watchHandles;
        std::vector<WatchHandle *> m_handles;
        std::atomic<bool> m_isDirty{false};

    public:
        bool onStart(Watcher &owner) override {
            (void)owner;
            m_isDirty = true;
            return true;
        }

        bool addPattern(Watcher &owner, const PatternPtr &pattern) override {
            if (!pattern) {
                return false;
            }
            const std::string rootKey = owner.m_removeAppPath(pattern->getPath());
            std::lock_guard<std::mutex> lock(m_mtx);
            auto it = m_watchHandles.find(rootKey);
            if (it != m_watchHandles.end()) {
                it->second.relatedPatterns.push_back(pattern);
            } else {
                WatchHandle hnd;
                hnd.path = ez::str::utf8Decode(rootKey);
                hnd.hDir = CreateFileW(
                    hnd.path.c_str(),
                    FILE_LIST_DIRECTORY,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    NULL,
                    OPEN_EXISTING,
                    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                    NULL);
                if (hnd.hDir == INVALID_HANDLE_VALUE) {
                    LogVarError("Err : Unable to open directory : %s", rootKey.c_str());
                    return false;
                }
                hnd.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
                hnd.ov.hEvent = hnd.hEvent;
                ZeroMemory(&hnd.ov, sizeof(hnd.ov));
                hnd.inFlight = false;
                hnd.relatedPatterns.push_back(pattern);
                m_watchHandles[rootKey] = hnd;
                auto &ref = m_watchHandles[rootKey];
                m_postRead(ref);  // *** Arm immediately to avoid missing the first changes ***
            }
            m_isDirty = true;
            return true;
        }

        void onStop(Watcher &owner) override {
            (void)owner;
            std::lock_guard<std::mutex> lock(m_mtx);
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
            m_handles.clear();
        }

        // ---- Your exact functions (content preserved, only owner passed when needed) ----
        void completePathResult(PathResult &voResult, const FILE_NOTIFY_INFORMATION *vpNotify, Watcher &owner) {
            const auto chFile = owner.m_removeAppPath(ez::str::utf8Encode(std::wstring(vpNotify->FileName, vpNotify->FileNameLength / sizeof(WCHAR))));
            // const char *mode = " ";
            switch (vpNotify->Action) {
                case FILE_ACTION_ADDED: {
                    voResult.modifType = PathResult::ModifType::CREATION;
                    voResult.newPath = chFile;
                } break;
                case FILE_ACTION_REMOVED: {
                    voResult.modifType = PathResult::ModifType::DELETION;
                    voResult.newPath = chFile;
                } break;
                case FILE_ACTION_MODIFIED: {
                    voResult.modifType = PathResult::ModifType::MODIFICATION;
                    voResult.newPath = chFile;
                } break;
                case FILE_ACTION_RENAMED_OLD_NAME: {
                    voResult.modifType = PathResult::ModifType::RENAMED;
                    voResult.oldPath = chFile;
                } break;
                case FILE_ACTION_RENAMED_NEW_NAME: {
                    voResult.modifType = PathResult::ModifType::RENAMED;
                    voResult.newPath = chFile;
                } break;
            }
        }

        bool m_postRead(WatchHandle &hnd) {
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

        // In class Watcher::BackendWindows (private)
        void m_pollOneHandle(WatchHandle *vpHandle, std::set<PathResult> &voFiles, Watcher &owner) {
            // Non-blocking poll of a single handle; parse results and re-arm the async read.
            if (vpHandle == nullptr) {
                return;
            }

            // 2) Poll event (non-blocking)
            DWORD wr = WaitForSingleObject(vpHandle->hEvent, 0);
#ifdef _DEBUG
            switch (wr) {
                case WAIT_ABANDONED: {
                    LogVarLightInfo("%s", "WAIT_ABANDONED");
                } break;
                case WAIT_OBJECT_0: {
                    // LogVarLightInfo("%s", "WAIT_OBJECT_0");
                } break;
                case WAIT_TIMEOUT: {
                    //   LogVarLightInfo("%s", "WAIT_TIMEOUT");
                } break;
                case WAIT_FAILED: {
                    LogVarLightInfo("%s", "WAIT_FAILED");
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
                    completePathResult(pr, pNotify, owner);
                    // Only emit when we have a valid newPath (pairs OLD/NEW may come split)
                    if (!pr.newPath.empty()) {
                        // snapshot patterns under lock to avoid concurrent modification
                        std::vector<PatternWeak> relatedCopy;
                        {
                            std::lock_guard<std::mutex> lock(m_mtx);
                            relatedCopy = vpHandle->relatedPatterns;
                        }
                        for (const auto &patW : relatedCopy) {
                            if (auto pPat = patW.lock()) {
                                if (pPat->getPhysicalType() == Pattern::PhysicalType::DIR) {
                                    voFiles.emplace(pr);
                                    m_logPathResult(pr);
                                } else {
                                    if (pPat->isPatternMatch(pr.newPath)) {
                                        voFiles.emplace(pr);
                                        m_logPathResult(pr);
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

        void poll(Watcher &owner, std::set<PathResult> &out) override {
            if (m_isDirty.exchange(false)) {
                std::lock_guard<std::mutex> lock(m_mtx);
                m_handles.clear();
                m_handles.reserve(m_watchHandles.size());
                for (auto &kv : m_watchHandles) {
                    m_handles.push_back(&kv.second);  // store pointer
                }
            }
            for (auto *hnd : m_handles) {
                if (!hnd->inFlight) {
                    m_postRead(*hnd);
                }
                m_pollOneHandle(hnd, out, owner);
            }
        }
    };
#endif  // WINDOWS_OS

    // =============================== Backend Linux ===============================
#if defined(LINUX_OS)
#include <unistd.h>
#include <limits.h>
#include <errno.h>

    class BackendLinux : public IBackend {
    public:
        struct WatchHandle {
            int wd{-1};
            std::string rootKey;
            std::vector<PatternWeak> relatedPatterns;
        };

        bool onStart(Watcher &owner) override {
            (void)owner;
            m_fd = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
            if (m_fd < 0) {
                LogVarError("%s", "Error: Unable to initialize inotify.");
                return false;
            }
            return true;
        }

        void onStop(Watcher &owner) override {
            (void)owner;
            for (auto &kv : m_handles) {
                if (kv.second.wd >= 0) {
                    inotify_rm_watch(m_fd, kv.second.wd);
                    kv.second.wd = -1;
                }
            }
            if (m_fd >= 0) {
                close(m_fd);
                m_fd = -1;
            }
            m_wdToRoot.clear();
            m_pendingRename.clear();
            m_handles.clear();
            m_buf.clear();
        }

        bool addPattern(Watcher &owner, const PatternPtr &pattern) override {
            if (!pattern) {
                return false;
            }
            const std::string rootKey = owner.m_removeAppPath(pattern->getPath());
            auto it = m_handles.find(rootKey);
            if (it != m_handles.end()) {
                it->second.relatedPatterns.push_back(pattern);
            } else {
                const uint32_t mask = IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO;
                int wd = inotify_add_watch(m_fd, rootKey.c_str(), mask);
                if (wd < 0) {
                    LogVarError("Error: inotify_add_watch failed on %s (errno %d)", rootKey.c_str(), errno);
                    return false;
                }
                WatchHandle hnd;
                hnd.wd = wd;
                hnd.rootKey = rootKey;
                hnd.relatedPatterns.push_back(pattern);
                m_handles[rootKey] = hnd;
                m_wdToRoot[wd] = rootKey;
            }
            return true;
        }

        void poll(Watcher &owner, std::set<PathResult> &out) override {
            (void)owner;
            if (m_fd < 0) {
                return;
            }
            if (m_buf.size() < 64 * 1024) {
                m_buf.resize(64 * 1024);
            }

            ssize_t n = read(m_fd, m_buf.data(), m_buf.size());
            if (n <= 0) {
                return;
            }

            ssize_t off = 0;
            while (off < n) {
                const inotify_event *ev = reinterpret_cast<const inotify_event *>(m_buf.data() + off);

                auto itRoot = m_wdToRoot.find(ev->wd);
                if (itRoot != m_wdToRoot.end()) {
                    const std::string &rootKey = itRoot->second;
                    auto itH = m_handles.find(rootKey);
                    if (itH != m_handles.end()) {
                        const bool isDir = (ev->mask & IN_ISDIR) != 0;
                        if (!isDir && ev->len > 0) {
                            const std::string rel(ev->name);
                            PathResult pr{};
                            if (ev->mask & IN_CREATE) {
                                pr.modifType = PathResult::ModifType::CREATION;
                                pr.newPath = rel;
                                m_emitIfMatch(rootKey, itH->second.relatedPatterns, rel, pr, out);
                            } else if (ev->mask & IN_DELETE) {
                                pr.modifType = PathResult::ModifType::DELETION;
                                pr.newPath = rel;
                                m_emitIfMatch(rootKey, itH->second.relatedPatterns, rel, pr, out);
                            } else if (ev->mask & IN_MODIFY) {
                                pr.modifType = PathResult::ModifType::MODIFICATION;
                                pr.newPath = rel;
                                m_emitIfMatch(rootKey, itH->second.relatedPatterns, rel, pr, out);
                            } else if (ev->mask & IN_MOVED_FROM) {
                                m_pendingRename[ev->cookie] = std::make_pair(ev->wd, rel);
                            } else if (ev->mask & IN_MOVED_TO) {
                                auto itCookie = m_pendingRename.find(ev->cookie);
                                if (itCookie != m_pendingRename.end()) {
                                    if (itCookie->second.first == ev->wd) {
                                        pr.modifType = PathResult::ModifType::RENAMED;
                                        pr.oldPath = itCookie->second.second;
                                        pr.newPath = rel;
                                        m_emitIfMatch(rootKey, itH->second.relatedPatterns, rel, pr, out);
                                    } else {
                                        pr.modifType = PathResult::ModifType::CREATION;
                                        pr.newPath = rel;
                                        m_emitIfMatch(rootKey, itH->second.relatedPatterns, rel, pr, out);
                                    }
                                    m_pendingRename.erase(itCookie);
                                } else {
                                    pr.modifType = PathResult::ModifType::CREATION;
                                    pr.newPath = rel;
                                    m_emitIfMatch(rootKey, itH->second.relatedPatterns, rel, pr, out);
                                }
                            }
                        }
                    }
                }

                off += sizeof(inotify_event) + ev->len;
            }
        }

    private:
        int m_fd{-1};
        std::unordered_map<std::string, WatchHandle> m_handles;
        std::unordered_map<int, std::string> m_wdToRoot;
        std::unordered_map<uint32_t, std::pair<int, std::string>> m_pendingRename;
        std::vector<uint8_t> m_buf;
    };
#endif  // LINUX_OS

    // =============================== Backend macOS ===============================
#if defined(APPLE_OS)
    class BackendMac : public IBackend {
    public:
#include <CoreServices/CoreServices.h>

        struct WatchHandle {
            std::string rootKey;
            std::vector<PatternWeak> relatedPatterns;
        };

        bool onStart(Watcher &owner) override {
            (void)owner;
            m_dirty = true;
            return true;
        }

        void onStop(Watcher &owner) override {
            (void)owner;
            if (m_stream) {
                FSEventStreamStop(m_stream);
                FSEventStreamInvalidate(m_stream);
                FSEventStreamRelease(m_stream);
                m_stream = nullptr;
            }
            std::lock_guard<std::mutex> lock(m_qmtx);
            m_queue.clear();
            m_handles.clear();
        }

        bool addPattern(Watcher &owner, const PatternPtr &pattern) override {
            if (!pattern) {
                return false;
            }
            const std::string rootKey = owner.m_removeAppPath(pattern->getPath());
            auto it = m_handles.find(rootKey);
            if (it == m_handles.end()) {
                WatchHandle h;
                h.rootKey = rootKey;
                h.relatedPatterns.push_back(pattern);
                m_handles[rootKey] = std::move(h);
                m_dirty = true;
            } else {
                it->second.relatedPatterns.push_back(pattern);
            }
            return true;
        }

        void poll(Watcher &owner, std::set<PathResult> &out) override {
            (void)owner;

            if (m_dirty) {
                if (m_stream) {
                    FSEventStreamStop(m_stream);
                    FSEventStreamInvalidate(m_stream);
                    FSEventStreamRelease(m_stream);
                    m_stream = nullptr;
                }

                CFMutableArrayRef paths = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
                for (const auto &kv : m_handles) {
                    if (!kv.first.empty()) {
                        CFStringRef s = CFStringCreateWithCString(NULL, kv.first.c_str(), kCFStringEncodingUTF8);
                        CFArrayAddValue(paths, s);
                        CFRelease(s);
                    }
                }

                if (CFArrayGetCount(paths) > 0) {
                    FSEventStreamContext ctx{};
                    ctx.info = this;
                    const FSEventStreamCreateFlags flags = kFSEventStreamCreateFlagFileEvents | kFSEventStreamCreateFlagNoDefer;

                    m_stream = FSEventStreamCreate(NULL, &BackendMac::sCallback, &ctx, paths, kFSEventStreamEventIdSinceNow, 0.1, flags);
                    if (m_stream) {
                        FSEventStreamScheduleWithRunLoop(m_stream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
                        FSEventStreamStart(m_stream);
                    }
                }
                CFRelease(paths);
                m_dirty = false;
            }

            if (m_stream) {
                CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.0, true);
            }

            {
                std::lock_guard<std::mutex> lock(m_qmtx);
                for (const PathResult &pr : m_queue) {
                    out.emplace(pr);
                }
                m_queue.clear();
            }
        }

    private:
        static void
        sCallback(ConstFSEventStreamRef, void *userData, size_t numEvents, void *eventPaths, const FSEventStreamEventFlags *eventFlags, const FSEventStreamEventId *) {
            BackendMac *self = static_cast<BackendMac *>(userData);
            if (!self) {
                return;
            }

            char **paths = static_cast<char **>(eventPaths);

            // Snapshot roots for matching and pattern list
            std::vector<std::pair<std::string, std::vector<PatternWeak>>> roots;
            roots.reserve(self->m_handles.size());
            for (const auto &kv : self->m_handles) {
                roots.emplace_back(kv.first, kv.second.relatedPatterns);
            }

            std::vector<PathResult> local;
            local.reserve(numEvents);

            for (size_t i = 0; i < numEvents; ++i) {
                const std::string changed = paths[i];
                const auto flags = eventFlags[i];

                // Only files
                if ((flags & kFSEventStreamEventFlagItemIsFile) == 0) {
                    continue;
                }

                // Find root; produce relative path
                for (const auto &r : roots) {
                    const std::string &rootKey = r.first;
                    if (changed.size() <= rootKey.size()) {
                        continue;
                    }
                    if (changed.compare(0, rootKey.size(), rootKey) != 0) {
                        continue;
                    }
                    const char sep = changed[rootKey.size()];
                    if (sep != '/' && sep != '\\') {
                        continue;
                    }

                    const std::string rel = changed.substr(rootKey.size() + 1);

                    PathResult pr{};
                    pr.rootPath = rootKey;
                    if (flags & kFSEventStreamEventFlagItemCreated) {
                        pr.modifType = PathResult::ModifType::CREATION;
                        pr.newPath = rel;
                    } else if (flags & kFSEventStreamEventFlagItemRemoved) {
                        pr.modifType = PathResult::ModifType::DELETION;
                        pr.newPath = rel;
                    } else if (flags & kFSEventStreamEventFlagItemRenamed) {
                        pr.modifType = PathResult::ModifType::RENAMED;
                        pr.newPath = rel;  // oldPath not available with FSEvents
                    } else if (
                        flags &
                        (kFSEventStreamEventFlagItemModified | kFSEventStreamEventFlagItemInodeMetaMod | kFSEventStreamEventFlagItemFinderInfoMod |
                         kFSEventStreamEventFlagItemChangeOwner | kFSEventStreamEventFlagItemXattrMod)) {
                        pr.modifType = PathResult::ModifType::MODIFICATION;
                        pr.newPath = rel;
                    } else {
                        continue;
                    }

                    // Pattern filtering (DIR accepts all; FILE filters on newPath)
                    for (const auto &pw : r.second) {
                        if (auto p = pw.lock()) {
                            if (p->getPhysicalType() == Pattern::PhysicalType::DIR) {
                                local.push_back(pr);
                            } else if (!pr.newPath.empty() && p->isPatternMatch(pr.newPath, false)) {
                                local.push_back(pr);
                            }
                        }
                    }
                }
            }

            if (!local.empty()) {
                std::lock_guard<std::mutex> lock(self->m_qmtx);
                for (const auto &pr : local) {
                    self->m_queue.push_back(pr);
                }
            }
        }

        FSEventStreamRef m_stream{nullptr};
        bool m_dirty{false};
        std::unordered_map<std::string, WatchHandle> m_handles;

        std::mutex m_qmtx;
        std::vector<PathResult> m_queue;
    };
#endif  // APPLE_OS

};  // class Watcher

// =============================== Backend factory ===============================
inline std::unique_ptr<Watcher::IBackend> Watcher::m_createBackend() {
#if defined(WINDOWS_OS)
    return std::unique_ptr<IBackend>(new BackendWindows());
#elif defined(LINUX_OS)
    return std::unique_ptr<IBackend>(new BackendLinux());
#elif defined(APPLE_OS)
    return std::unique_ptr<IBackend>(new BackendMac());
#else
    return nullptr;
#endif
}

#endif  // ndef EMSCRIPTEN_OS

}  // namespace file
}  // namespace ez
