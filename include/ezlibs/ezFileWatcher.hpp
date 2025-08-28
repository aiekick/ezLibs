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

// ezFileWatcher is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include "ezOS.hpp"
#include "ezStr.hpp"

#include <array>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <iostream>
#include <functional>

#ifdef WINDOWS_OS
#include <windows.h>
#elif defined(LINUX_OS)
#include <sys/inotify.h>
#include <unistd.h>
#elif defined(MAC_OS)
#include <CoreServices/CoreServices.h>
#else
#error "Platform not supported"
#endif

namespace ez {

class FileWatcher {
public:
#ifdef WINDOWS_OS
    using Callback = std::function<void(const std::vector<std::wstring>&)>;
#else
    using Callback = std::function<void(const std::vector<std::string>&)>;
#endif
private:
#ifdef WINDOWS_OS
    std::wstring m_filePathNameW;
#else
    std::string m_filePathName;
#endif
    Callback m_callback;
    std::thread m_thread;
    std::atomic<bool> m_running;

public:
    FileWatcher() : m_callback(nullptr), m_running(false) {}
    ~FileWatcher() { stop(); }

#ifdef WINDOWS_OS
    void watchFile(const std::wstring& vFilePathName, Callback vCallback) {
        m_filePathNameW = vFilePathName;
        m_callback = vCallback;
    }

    void watchFileUtf8(const std::string& vFilePathNameUtf8, Callback vCallback) { watchFile(ez::str::utf8Decode(vFilePathNameUtf8), vCallback); }
#else
    void watchFile(const std::string& vFilePathName, Callback vCallback) {
        m_filePathName = vFilePathName;
        m_callback = vCallback;
    }
#endif

    void start() {
        if (m_running)
            return;
        m_running = true;

#ifdef WINDOWS_OS
        m_thread = std::thread(&FileWatcher::watchWindows, this);
#elif defined(LINUX_OS)
        m_thread = std::thread(&FileWatcher::watchLinux, this);
#elif defined(MAC_OS)
        m_thread = std::thread(&FileWatcher::watchMacOS, this);
#endif
    }

    void stop() {
        if (!m_running)
            return;
        m_running = false;
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

private:
#ifdef WINDOWS_OS
    std::wstring getParentDirW(const std::wstring& path) const {
        size_t slash = path.find_last_of(L"/\\");
        if (slash == std::wstring::npos)
            return L".";
        return path.substr(0, slash);
    }

    std::wstring getFileNameOnlyW(const std::wstring& path) const {
        size_t slash = path.find_last_of(L"/\\");
        if (slash == std::wstring::npos)
            return path;
        return path.substr(slash + 1);
    }
#else
    std::wstring getParentDir(const std::string& path) const {
        size_t slash = path.find_last_of("/\\");
        if (slash == std::string::npos)
            return L".";
        return path.substr(0, slash);
    }

    std::wstring getFileNameOnly(const std::string& path) const {
        size_t slash = path.find_last_of("/\\");
        if (slash == std::string::npos)
            return path;
        return path.substr(slash + 1);
    }
#endif

#ifdef WINDOWS_OS
    void watchWindows() {
        std::wstring dirPath = getParentDirW(m_filePathNameW);
        std::wstring fileNameOnly = getFileNameOnlyW(m_filePathNameW);

        HANDLE hDir = CreateFileW(
            dirPath.c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

        if (hDir == INVALID_HANDLE_VALUE) {
            std::wcerr << L"Error: Unable to open directory: " << dirPath << std::endl;
            return;
        }

        std::array<char, 4096> buffer;
        DWORD bytesReturned;

        while (m_running) {
            if (ReadDirectoryChangesW(hDir, buffer.data(), static_cast<DWORD>(buffer.size()), FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE, &bytesReturned, NULL, NULL)) {
                DWORD offset = 0;
                do {
                    FILE_NOTIFY_INFORMATION* pNotify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&buffer[offset]);
                    std::wstring changedFile(pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR));

                    if (changedFile == fileNameOnly && m_callback) {
                        // Conversion ici si nécessaire :
                        m_callback({m_filePathNameW});
                    }

                    offset += pNotify->NextEntryOffset;
                } while (offset != 0);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        CloseHandle(hDir);
    }
#elif defined(LINUX_OS)
    void watchLinux() {
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

#elif defined(MAC_OS)
    static void
    fileChangedCallback(ConstFSEventStreamRef, void* userData, size_t numEvents, void* eventPaths, const FSEventStreamEventFlags*, const FSEventStreamEventId*) {
        FileWatcher* watcher = static_cast<FileWatcher*>(userData);
        if (!watcher->m_running)
            return;

        char** paths = static_cast<char**>(eventPaths);
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

    void watchMacOS() {
        std::string dir = getParentDir(m_filePathName);
        CFStringRef path = CFStringCreateWithCString(NULL, dir.c_str(), kCFStringEncodingUTF8);
        CFArrayRef pathsToWatch = CFArrayCreate(NULL, (const void**)&path, 1, NULL);

        FSEventStreamContext context = {0, this, NULL, NULL, NULL};
        FSEventStreamRef stream =
            FSEventStreamCreate(NULL, &fileChangedCallback, &context, pathsToWatch, kFSEventStreamEventIdSinceNow, 0.5, kFSEventStreamCreateFlagNone);

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

}  // namespace ez
