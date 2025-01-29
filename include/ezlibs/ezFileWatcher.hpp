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
    using Callback = std::function<void(const std::vector<std::string>&)>;

private:
    std::string m_filePathName;
    Callback m_callback{nullptr};
    std::thread m_thread;
    std::atomic<bool> m_running{false};

public:
    FileWatcher() = default;
    ~FileWatcher() { stop(); }

    void watchFile(const std::string& vFilePathName, Callback vCallback) {
        m_filePathName = vFilePathName;
        m_callback = vCallback;
    }

    void start() {
        if (m_running)
            return;
        m_running = true;

#ifdef _WIN32
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
#ifdef _WIN32
    void watchWindows() {
        auto hDir = CreateFile(
            m_filePathName.c_str(),
            FILE_LIST_DIRECTORY,
            FILE_SHARE_READ |  //
                FILE_SHARE_WRITE |  //
                FILE_SHARE_DELETE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS,
            NULL);

        if (hDir == INVALID_HANDLE_VALUE) {
            std::cerr << "Error: Unable to open directory.\n";
            return;
        }

        std::array<char, 1024> buffer;
        DWORD bytesReturned;

        while (m_running) {
            if (ReadDirectoryChangesW(hDir, buffer.data(), buffer.size(), TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE, &bytesReturned, NULL, NULL)) {
                // Exécuter le callback
                if (m_callback) {
                    m_callback({m_filePathName});
                }
            }
        }

        CloseHandle(hDir);
    }

#elif defined(LINUX_OS)
    void watchLinux() {
        int fd = inotify_init();
        if (fd < 0) {
            std::cerr << "Error: Unable to initialize inotify.\n";
            return;
        }

        int wd = inotify_add_watch(fd, m_filePathName.c_str(), IN_MODIFY);
        if (wd < 0) {
            std::cerr << "Error: Unable to add watch.\n";
            close(fd);
            return;
        }

        std::array<char, 1024> buffer;
        while (m_running) {
            int length = read(fd, buffer.data(), buffer.size());
            if (length > 0) {
                if (m_callback) {
                    m_callback({m_filePathName});
                }
            }
        }

        inotify_rm_watch(fd, wd);
        close(fd);
    }

#elif defined(MAC_OS)
    static void fileChangedCallback(
        ConstFSEventStreamRef streamRef,
        void* userData,
        size_t numEvents,
        void* eventPaths,
        const FSEventStreamEventFlags* eventFlags,
        const FSEventStreamEventId* eventIds) {
        auto* watcher = static_cast<FileWatcher*>(userData);
        if (!watcher->m_running)
            return;

        char** paths = static_cast<char**>(eventPaths);
        std::vector<std::string> changedFiles;
        for (size_t i = 0; i < numEvents; i++) {
            changedFiles.emplace_back(paths[i]);
        }

        if (watcher->m_callback) {
            watcher->m_callback(changedFiles);
        }
    }

    void watchMacOS() {
        CFStringRef path = CFStringCreateWithCString(NULL, m_filePathName.c_str(), kCFStringEncodingUTF8);
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
