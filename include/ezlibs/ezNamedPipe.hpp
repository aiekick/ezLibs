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

 */
#include <string>
#include <stdexcept>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#endif

namespace ez {

class NamedPipe {
public:
    NamedPipe(const std::string& pipeName, bool isServer) : m_isServer(isServer) {
#ifdef _WIN32
        m_pipeName = "\\\\.\\pipe\\" + pipeName;

        if (isServer) {
            // Create the named pipe
            m_pipeHandle = CreateNamedPipe(
                m_pipeName.c_str(),
                PIPE_ACCESS_DUPLEX,
                PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                1,          // Max instances
                4096,       // Output buffer size
                4096,       // Input buffer size
                0,          // Default timeout
                nullptr     // Default security attributes
            );

            if (m_pipeHandle == INVALID_HANDLE_VALUE) {
                throw std::runtime_error("Failed to create named pipe.");
            }

            // Wait for a client to connect
            if (!ConnectNamedPipe(m_pipeHandle, nullptr)) {
                CloseHandle(m_pipeHandle);
                throw std::runtime_error("Failed to connect named pipe.");
            }
        } else {
            // Open the named pipe as a client
            m_pipeHandle = CreateFile(
                m_pipeName.c_str(),
                GENERIC_READ | GENERIC_WRITE,
                0,
                nullptr,
                OPEN_EXISTING,
                0,
                nullptr
            );

            if (m_pipeHandle == INVALID_HANDLE_VALUE) {
                throw std::runtime_error("Failed to open named pipe.");
            }
        }
#else
        m_pipeName = "/tmp/" + pipeName;

        if (isServer) {
            // Create the named pipe (FIFO)
            if (mkfifo(m_pipeName.c_str(), 0666) == -1 && errno != EEXIST) {
                throw std::runtime_error("Failed to create named pipe: " + std::string(strerror(errno)));
            }

            // Open the pipe for reading and writing
            m_pipeFd = open(m_pipeName.c_str(), O_RDWR);
            if (m_pipeFd == -1) {
                throw std::runtime_error("Failed to open named pipe: " + std::string(strerror(errno)));
            }
        } else {
            // Open the pipe as a client
            m_pipeFd = open(m_pipeName.c_str(), O_RDWR);
            if (m_pipeFd == -1) {
                throw std::runtime_error("Failed to open named pipe: " + std::string(strerror(errno)));
            }
        }
#endif
    }

    ~NamedPipe() {
#ifdef _WIN32
        if (m_pipeHandle != INVALID_HANDLE_VALUE) {
            CloseHandle(m_pipeHandle);
        }
#else
        if (m_pipeFd != -1) {
            close(m_pipeFd);
        }
        if (m_isServer) {
            unlink(m_pipeName.c_str());
        }
#endif
    }

    void write(const std::string& message) {
#ifdef _WIN32
        DWORD bytesWritten;
        if (!WriteFile(m_pipeHandle, message.c_str(), message.size(), &bytesWritten, nullptr)) {
            throw std::runtime_error("Failed to write to named pipe.");
        }
#else
        ssize_t bytesWritten = ::write(m_pipeFd, message.c_str(), message.size());
        if (bytesWritten == -1) {
            throw std::runtime_error("Failed to write to named pipe: " + std::string(strerror(errno)));
        }
#endif
    }

    std::string read() {
#ifdef _WIN32
        char buffer[4096];
        DWORD bytesRead;
        if (!ReadFile(m_pipeHandle, buffer, sizeof(buffer) - 1, &bytesRead, nullptr)) {
            throw std::runtime_error("Failed to read from named pipe.");
        }
        buffer[bytesRead] = '\0';
        return std::string(buffer);
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

private:
    std::string m_pipeName;
    bool m_isServer;

#ifdef _WIN32
    HANDLE m_pipeHandle = INVALID_HANDLE_VALUE;
#else
    int m_pipeFd = -1;
#endif
};


}  // namespace ez
