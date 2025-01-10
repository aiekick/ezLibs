#pragma once

#include <array>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <iostream>
#include <functional>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__unix__) || defined(__APPLE__)
#include <sys/inotify.h>
#include <unistd.h>
#else
#error "Platform not supported"
#endif

namespace ez {

class FileWatcher {
public:
    using Callback = std::function<void(const std::vector<std::string>& vFiles)>;

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
        if (m_running) {
            return;
        }
        m_running = true;

#if defined(_WIN32)
        m_thread = std::thread(&FileWatcher::watchWindows, this);
#else
        m_thread = std::thread(&FileWatcher::watchUnix, this);
#endif
    }

    void stop() {
        if (!m_running) {
            return;
        }
        m_running = false;
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

private:
#if defined(_WIN32)
    void watchWindows() {
        auto hDir = CreateFile(
            m_filePathName.c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

        if (hDir == INVALID_HANDLE_VALUE) {
            std::cerr << "Error: Unable to open directory.\n";
            return;
        }

        std::array<char, 1024> buffer;
        DWORD bytesReturned;

        while (m_running) {
            if (ReadDirectoryChangesW(hDir, buffer.data(), buffer.size(), TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE, &bytesReturned, NULL, NULL)) {
             //   m_callback();
            }
        }

        CloseHandle(hDir);
    }
#else
    void watchUnix() {
        int fd = inotify_init();
        if (fd < 0) {
            std::cerr << "Error: Unable to initialize inotify.\n";
            return;
        }

        int wd = inotify_add_watch(fd, path_.c_str(), IN_MODIFY);
        if (wd < 0) {
            std::cerr << "Error: Unable to add watch.\n";
            close(fd);
            return;
        }

        std::array<char, 1024> buffer;
        while (m_running) {
            int length = read(fd, buffer.data(), buffer.size());
            if (length > 0) {
                m_callback();
            }
        }

        inotify_rm_watch(fd, wd);
        close(fd);
    }
#endif
};

}  // namespace ez
