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

// ezNamedPipes is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

/* // Test code

void server() {
    try {
        ez::NamedPipe pipe("MyPipe", true);
        std::cout << "Server: Waiting for client...\n";

        std::string message = pipe.read();
        std::cout << "Server received: " << message << std::endl;

        pipe.write("Hello from server!");
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
    }
}

void client() {
    try {
        ez::NamedPipe pipe("MyPipe", false);
        std::cout << "Client: Sending message to server...\n";

        pipe.write("Hello from client!");

        std::string message = pipe.read();
        std::cout << "Client received: " << message << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Client error: " << e.what() << std::endl;
    }
}

int main() {
    std::thread serverThread(server);
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Give server time to start
    std::thread clientThread(client);
    serverThread.join();
    clientThread.join();
    return 0;
}
*/

#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#endif

namespace ez {

class NamedPipe {
public:
    typedef std::vector<char> DatasBuffer;

private:
    size_t m_lastMessageSize{0};
    std::vector<char> m_buffer;
    std::string m_pipeName;
    bool m_isServer;
#ifdef _WIN32
    HANDLE m_pipeHandle = INVALID_HANDLE_VALUE;
#else
    int32_t m_pipeFd = -1;
#endif

public:
    NamedPipe() = default;
    ~NamedPipe() { unit(); }

    bool initServer(const std::string& pipeName, size_t vBufferSize, int32_t vMaxInstances = 1) {
        m_isServer = true; 
        m_buffer.resize(vBufferSize);
#ifdef _WIN32
        m_pipeName = "\\\\.\\pipe\\" + pipeName;
        m_pipeHandle = CreateNamedPipe(  //
            m_pipeName.c_str(),
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            vMaxInstances,  // Max instances
            vBufferSize,  // Output buffer size
            vBufferSize,  // Input buffer size
            0,  // Default timeout
            nullptr  // Default security attributes
        );
        if (m_pipeHandle == INVALID_HANDLE_VALUE) {
            return false;
        }
#else
        m_pipeName = "/tmp/" + pipeName;
        if (mkfifo(m_pipeName.c_str(), 0666) == -1 && errno != EEXIST) {
            return false;
        }
        m_pipeFd = open(m_pipeName.c_str(), O_RDWR);
        if (m_pipeFd == -1) {
            return false;
        }
#endif
        return true;
    }

    bool initClient(const std::string& pipeName) {
        m_isServer = false;
#ifdef _WIN32
        m_pipeName = "\\\\.\\pipe\\" + pipeName;
        m_pipeHandle = CreateFile(  //
            m_pipeName.c_str(), 
            GENERIC_READ | GENERIC_WRITE,
            0,
            nullptr,
            OPEN_EXISTING,
            0,
            nullptr);
        if (m_pipeHandle == INVALID_HANDLE_VALUE) {
            return false;
        }
#else
        m_pipeName = "/tmp/" + pipeName;
        m_pipeFd = open(m_pipeName.c_str(), O_RDWR);
        if (m_pipeFd == -1) {
            return false;
        }
#endif
        return true;
    }

    void unit() {
#ifdef _WIN32
        if (m_pipeHandle != INVALID_HANDLE_VALUE) {
            CloseHandle(m_pipeHandle);
        }
        m_pipeHandle = INVALID_HANDLE_VALUE;
#else
        if (m_pipeFd != -1) {
            close(m_pipeFd);
            if (m_isServer) {
                unlink(m_pipeName.c_str());
            }
        }
        m_pipeFd = -1;
#endif
    }

    bool write(const DatasBuffer& vMessage) {
#ifdef _WIN32
        DWORD bytesWritten;
        if (!WriteFile(m_pipeHandle, vMessage.data(), vMessage.size(), &bytesWritten, nullptr)) {
            return false;
        }
#else
        ssize_t bytesWritten = ::write(m_pipeFd, message.c_str(), message.size());
        if (bytesWritten == -1) {
            return false;
        }
#endif

        return true;
    }

    bool read() {
#ifdef _WIN32
        DWORD bytesRead;
        if (ReadFile(m_pipeHandle, m_buffer.data(), m_buffer.size(), &bytesRead, nullptr) == TRUE) {
            m_lastMessageSize = bytesRead;
            return true;
        }
        return false;
#else
        char buffer[4096];
        ssize_t bytesRead = ::read(m_pipeFd, buffer, sizeof(buffer) - 1);
        if (bytesRead == -1) {
            throw std::runtime_error("Failed to read from named pipe: " + std::string(strerror(errno)));
        }
        buffer[bytesRead] = '\0';
        return std::string(buffer);
#endif
    }

    DatasBuffer getMessage(size_t& vOutSize) {
        vOutSize = m_lastMessageSize;
        return m_buffer;
    }

    static std::vector<std::string> getActivePipes() {
        std::vector<std::string> pipeNames;
#ifdef _WIN32
        char buffer[4096];

        // Buffer pour les résultats
        WIN32_FIND_DATAA findFileData;
        HANDLE hFind = FindFirstFileA("\\\\.\\pipe\\*", &findFileData);

        if (hFind == INVALID_HANDLE_VALUE) {
            std::cerr << "Failed to list named pipes. Error: " << GetLastError() << std::endl;
            return pipeNames;
        }

        do {
            pipeNames.push_back(findFileData.cFileName);  // Ajoute le nom du pipe à la liste
        } while (FindNextFileA(hFind, &findFileData));  // Continue à chercher

        FindClose(hFind);  // Ferme le handle de recherche
#else
        DIR* dir = opendir(directory.c_str());
        if (!dir) {
            throw std::runtime_error("Failed to open directory: " + directory);
        }
        struct dirent* entry;
        struct stat fileStat;
        while ((entry = readdir(dir)) != nullptr) {
            std::string fullPath = directory + "/" + entry->d_name;
            if (stat(fullPath.c_str(), &fileStat) == -1) {
                std::cerr << "Failed to stat file: " << fullPath << std::endl;
                continue;
            }
            if (S_ISFIFO(fileStat.st_mode)) {
                pipeNames.push_back(fullPath);
            }
        }
        closedir(dir);
#endif
        return pipeNames;
    }
};


}  // namespace ez
