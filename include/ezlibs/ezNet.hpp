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

// ezNet is part od the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <cmath>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#pragma comment(lib, "Ws2_32")
#define WM_SOCKET 0x10000
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#endif

#include <string>
#include <stdexcept>
#include <iostream>

namespace ez {

class Net {
public:
    typedef std::vector<char> DatasBuffer;

private:
    int m_socketFileDescriptor{0};

    public:
        enum class Protocol { TCP, UDP };

        Net() : m_socketFileDescriptor(-1) {
#ifdef _WIN32
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                throw std::runtime_error("Failed to initialize Winsock.");
            }
#endif
        }

        ~Net() {
            closeSocket();
#ifdef _WIN32
            WSACleanup();
#endif
        }

        void createSocket(Protocol protocol) {
            int type = (protocol == Protocol::TCP) ? SOCK_STREAM : SOCK_DGRAM;
            m_socketFileDescriptor = ::socket(AF_INET, type, 0);
            if (m_socketFileDescriptor < 0) {
                throw std::runtime_error("Failed to create socket.");
            }
        }

        void bindSocket(int port) {
            sockaddr_in server_addr = {};
            server_addr.sin_family = AF_INET;
            server_addr.sin_addr.s_addr = INADDR_ANY;
            server_addr.sin_port = htons(port);

            if (::bind(m_socketFileDescriptor, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
                throw std::runtime_error("Failed to bind socket.");
            }
        }

        void listenSocket(int backlog = 5) {
            if (::listen(m_socketFileDescriptor, backlog) < 0) {
                throw std::runtime_error("Failed to listen on socket.");
            }
        }

        int acceptConnection() {
            sockaddr_in client_addr = {};
            socklen_t client_len = sizeof(client_addr);
            int client_fd = ::accept(m_socketFileDescriptor, (struct sockaddr*)&client_addr, &client_len);
            if (client_fd < 0) {
                throw std::runtime_error("Failed to accept connection.");
            }
            return client_fd;
        }

        void connectSocket(const std::string& ip, int port) {
            sockaddr_in server_addr = {};
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(port);
            if (inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) <= 0) {
                throw std::runtime_error("Invalid IP address.");
            }

            if (::connect(m_socketFileDescriptor, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
                throw std::runtime_error("Failed to connect to server.");
            }
        }

        void sendData(const DatasBuffer& data, int client_fd = -1) {
            int fd = (client_fd == -1) ? m_socketFileDescriptor : client_fd;
            if (::send(fd, data.data(), data.size(), 0) < 0) {
                throw std::runtime_error("Failed to send data.");
            }
        }

        DatasBuffer receiveData(int buffer_size = 1024, int client_fd = -1) {
            DatasBuffer buffer;
            buffer.resize(buffer_size);
            int fd = (client_fd == -1) ? m_socketFileDescriptor : client_fd;
            int bytes_received = ::recv(fd, buffer.data(), buffer.size() - 1, 0);
            if (bytes_received < 0) {
                throw std::runtime_error("Failed to receive data.");
            }
            return buffer;
        }

        void closeSocket() {
            if (m_socketFileDescriptor != -1) {
#ifdef _WIN32
                closesocket(m_socketFileDescriptor);
#else
                close(m_socketFileDescriptor);
#endif
                m_socketFileDescriptor = -1;
            }
        }
    };

}  // namespace ez
