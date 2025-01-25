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

#include <vector>
#include <string>
#include <cstdint>
#include <memory>
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

public:
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
        const auto directory = std::string("/tmp");
        DIR* dir = opendir(directory.c_str());
        if (dir) {
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
        }
#endif
        return pipeNames;
    }

private:
    class Backend {
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
        virtual ~Backend() { unit(); }

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

        bool writeBuffer(const DatasBuffer& vMessage) {
#ifdef _WIN32
            DWORD bytesWritten;
            if (!WriteFile(m_pipeHandle, vMessage.data(), vMessage.size(), &bytesWritten, nullptr)) {
                return false;
            }
#else
            auto bytesWritten = ::write(m_pipeFd, vMessage.data(), vMessage.size());
            if (bytesWritten == -1) {
                return false;
            }
#endif
            return true;
        }

        bool writeString(const std::string& vMessage) {
            if (!vMessage.empty()) {
                const ez::NamedPipe::DatasBuffer buffer(vMessage.begin(), vMessage.end());
                return writeBuffer(buffer);
            }
            return false;
        }

        bool isMessageReceived() {
#ifdef _WIN32
            DWORD bytesRead;
            if (ReadFile(m_pipeHandle, m_buffer.data(), m_buffer.size(), &bytesRead, nullptr) == TRUE) {
                m_lastMessageSize = bytesRead;
                return true;
            }
            return false;
#else
            auto bytesRead = ::read(m_pipeFd, m_buffer.data(), m_buffer.size());
            if (bytesRead != -1) {
                m_lastMessageSize = bytesRead;
                return true;
            }
            return false;
#endif
        }

        DatasBuffer readBuffer(size_t& vOutSize) {
            vOutSize = m_lastMessageSize;
            return m_buffer;
        }

        std::string readString() {
            if (m_lastMessageSize) {
                return std::string(m_buffer.data(), m_lastMessageSize);
            }
            return {};
        }

    protected:
        bool m_initServer(const std::string& vPipeName, size_t vBufferSize, int32_t vMaxInstances = 1) {
            m_isServer = true;
            m_buffer.resize(vBufferSize);
#ifdef _WIN32
            m_pipeName = "\\\\.\\pipe\\" + vPipeName;
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

        bool m_initClient(const std::string& vPipeName
#ifdef EZ_TIME
            , size_t vTimeOutInMs = 1000u
#endif
        ) {
            m_isServer = false;
#ifdef _WIN32
            m_pipeName = "\\\\.\\pipe\\" + vPipeName;
#ifdef EZ_TIME
            const auto start = ez::time::getTicks();
#endif
#ifdef EZ_TIME
            while (m_pipeHandle == INVALID_HANDLE_VALUE) {
#endif
                m_pipeHandle = CreateFile(  //
                    m_pipeName.c_str(),
                    GENERIC_READ | GENERIC_WRITE,
                    0,
                    nullptr,
                    OPEN_ALWAYS,
                    0,
                    nullptr);
#ifdef EZ_TIME
                if (m_pipeHandle == INVALID_HANDLE_VALUE && //
                    (ez::time::getTicks() - start) > vTimeOutInMs) {
                    break;
                }
            }
#endif
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
    };

public:
    class Server : public Backend {
    public:
        typedef std::shared_ptr<Server> Ptr;
        typedef std::weak_ptr<Server> Weak;

    public:
        static Ptr create(const std::string& pipeName, size_t vBufferSize, int32_t vMaxInstances = 1) {
            auto ret = std::make_shared<Server>();
            if (!ret->m_initServer(pipeName, vBufferSize, vMaxInstances)) {
                ret.reset();
            }
            return ret;
        }

        ~Server() override { unit(); }
    };


    class Client : public Backend {
    public:
        typedef std::shared_ptr<Client> Ptr;
        typedef std::weak_ptr<Client> Weak;

    public:
        static Ptr create(const std::string& pipeName) {
            auto ret = std::make_shared<Client>();
            if (!ret->m_initClient(pipeName)) {
                ret.reset();
            }
            return ret;
        }

        ~Client() override { unit(); }
    };
};

}  // namespace ez
