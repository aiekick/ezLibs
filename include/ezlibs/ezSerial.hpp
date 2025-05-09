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

// ezSerial is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include "ezOS.hpp"
#include <chrono>
#include <cstdio>
#include <string>
#include <thread>
#include <atomic>
#include <vector>
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <functional>

#ifdef WINDOWS_OS
#define NOMINMAX
#include <windows.h>
#include <setupapi.h>
#pragma comment(lib, "setupapi.lib")
#else
#include <glob.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/select.h>
#endif

namespace ez {
namespace net {

class SerialPort {
private:
    bool m_isOpen = false;

public:
    SerialPort() {}
    ~SerialPort() { close(); }

    SerialPort(const SerialPort&) = delete;
    SerialPort& operator=(const SerialPort&) = delete;

    /* ------------------------ open / close ------------------------------- */
    void open(const std::string& portName, uint32_t baud) {
#ifdef WINDOWS_OS
        std::string fullName = (portName.rfind("COM", 0) == 0 && portName.size() > 4) ? std::string("\\\\.\\") + portName : portName;
        handle_ = CreateFileA(fullName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
        if (handle_ == INVALID_HANDLE_VALUE) {
            const DWORD err = GetLastError();
            throw std::runtime_error("open(" + portName + ") failed, err=" + std::to_string(err));
        }
        COMMTIMEOUTS t = {0, 50, 10, 50, 10};
        SetCommTimeouts(handle_, &t);
        DCB dcb = {0};
        dcb.DCBlength = sizeof(dcb);
        if (!GetCommState(handle_, &dcb)) {
            throw std::runtime_error("GetCommState failed");
        }
        dcb.BaudRate = baud;
        dcb.ByteSize = 8;
        dcb.StopBits = ONESTOPBIT;
        dcb.Parity = NOPARITY;
        if (!SetCommState(handle_, &dcb)) {
            throw std::runtime_error("SetCommState failed");
        }
#else
        fd_ = ::open(portName.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (fd_ < 0) {
            throw std::runtime_error(std::string("open(") + portName + "): " + ::strerror(errno));
        }
        struct termios tty;
        ::tcgetattr(fd_, &tty);
        ::cfmakeraw(&tty);
        tty.c_cflag |= CLOCAL | CREAD | CS8;
        const speed_t s = translateBaud(baud);
        ::cfsetispeed(&tty, s);
        ::cfsetospeed(&tty, s);
        tty.c_cc[VMIN] = 0;
        tty.c_cc[VTIME] = 1;
        if (::tcsetattr(fd_, TCSANOW, &tty) != 0) {
            throw std::runtime_error("tcsetattr failed");
        }
#endif
        m_isOpen = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }

    void close() {
        if (!m_isOpen) {
            return;
        }
#ifdef WINDOWS_OS
        ::PurgeComm(handle_, PURGE_TXCLEAR | PURGE_RXCLEAR);
        ::CloseHandle(handle_);
        handle_ = INVALID_HANDLE_VALUE;
#else
        ::tcflush(fd_, TCIOFLUSH);
        ::close(fd_);
        fd_ = -1;
#endif
        m_isOpen = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    /* --------------------------- I/O ------------------------------------- */
    void writeLine(const std::string& line, bool verbose = false) {
        if (!m_isOpen) {
            throw std::runtime_error("Port not open");
        }
        const std::string data = line + "\n";
#ifdef WINDOWS_OS
        DWORD written = 0;
        if (!WriteFile(handle_, data.data(), (DWORD)data.size(), &written, nullptr)) {
            throw std::runtime_error("WriteFile failed");
        }
#else
        if (::write(fd_, data.data(), data.size()) < 0) {
            throw std::runtime_error("write failed");
        }
#endif
        if (verbose) {
            std::cout << "Sent: " << line << '\n';
        }
    }

    bool readLine(std::string& out, uint32_t timeoutMs = 1000) {
        if (!m_isOpen) {
            throw std::runtime_error("Port not open");
        }
        out.clear();
        const auto start = std::chrono::steady_clock::now();
        char ch;
        while (elapsedMs(start) < timeoutMs) {
            if (readByte(ch)) {
                if (ch == '\n') {
                    return true;
                }
                if (ch != '\r') {
                    out.push_back(ch);
                }
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
        return false;
    }

    bool isConnected() const { return m_isOpen; }

    /* ---------------------- static helpers ------------------------------ */
    static std::vector<std::string> listPorts() {
        std::vector<std::string> v;
#ifdef WINDOWS_OS
        for (int i = 1; i <= 256; ++i) {
            const std::string n = "COM" + std::to_string(i);
            HANDLE h = CreateFileA(("\\\\." + n).c_str(), 0, 0, nullptr, OPEN_EXISTING, 0, nullptr);
            if (h != INVALID_HANDLE_VALUE) {
                v.push_back(n);
                CloseHandle(h);
            }
        }
#else
        auto globAdd = [&v](const char* pattern) {
            glob_t g;
            if (!::glob(pattern, 0, nullptr, &g)) {
                for (size_t i = 0; i < g.gl_pathc; ++i) {
                    v.push_back(g.gl_pathv[i]);
                }
                ::globfree(&g);
            }
        };
        globAdd("/dev/ttyACM*");
        globAdd("/dev/ttyUSB*");
        globAdd("/dev/cu.usbmodem*");
        globAdd("/dev/cu.usbserial*");
#endif
        return v;
    }

    static std::string autoDetect(uint32_t baud, const std::string& handshake = "PING", const std::string& expectedReply = "PONG", uint32_t timeoutMs = 300) {
        std::string ret;
        const std::vector<std::string> ports = listPorts();
        for (size_t i = 0; i < ports.size(); ++i) {
            try {
                SerialPort sp;
                sp.open(ports[i], baud);
                std::string tmp;
                while (sp.readLine(tmp, 50)) {
                }
                sp.writeLine(handshake);
                std::string r;
                const bool ok = sp.readLine(r, timeoutMs);
                sp.close();
                if (ok && r == expectedReply) {
                    ret = ports[i];
                    break;
                }
            } catch (...) {
                /* continue */
            }
        }
        return ret;
    }

private:
#ifdef WINDOWS_OS
    HANDLE handle_ = INVALID_HANDLE_VALUE;
    bool readByte(char& ch) {
        DWORD read = 0;
        return ::ReadFile(handle_, &ch, 1, &read, nullptr) && read == 1;
    }
#else
    int fd_ = -1;
    bool readByte(char& ch) { return ::read(fd_, &ch, 1) == 1; }
    static speed_t translateBaud(uint32_t baud) {
        switch (baud) {
            case 9600: return B9600;
            case 19200: return B19200;
            case 38400: return B38400;
            case 57600: return B57600;
            case 115200: return B115200;
            default: throw std::invalid_argument("Unsupported baud rate");
        }
    }
#endif

    static uint32_t elapsedMs(const std::chrono::steady_clock::time_point& start) {
        using namespace std::chrono;
        return static_cast<uint32_t>(duration_cast<milliseconds>(steady_clock::now() - start).count());
    }
};

class SerialDevice {
public:
    typedef std::function<void(bool /*connected*/, const std::string& /*port*/)> Callback;

    SerialDevice(uint32_t baud = 115200, const std::string& handshake = "PING", const std::string& expected = "PONG", uint16_t vid = 0, uint16_t pid = 0)
        : m_baud(baud), m_handshake(handshake), m_expected(expected), m_vid(vid), m_pid(pid) {}

    ~SerialDevice() {
        stopWatchdog();
        disconnect();
    }

    bool connect() {
        std::string port;
        if (!autoDetect(port)) {
            return false;
        }
        return connect(port);
    }

    bool connect(const std::string& port) {
        if (m_port.isConnected()) {
            disconnect();
        }
        try {
            m_port.open(port, m_baud);
            m_portName = port;
            return true;
        } catch (...) {
            m_portName.clear();
            return false;
        }
    }

    void disconnect() {
        m_port.close();
        m_portName.clear();
    }

    bool isConnected() const { return !m_portName.empty(); }
    const std::string& portName() const { return m_portName; }
    uint32_t baudRate() const { return m_baud; }

    SerialPort& port() { return m_port; }

    void startWatchdog(uint32_t periodMs = 1000, Callback cb = Callback()) {
        if (m_running) {
            return;
        }
        m_cb = cb;
        m_running = true;
        m_thread = std::thread([this, periodMs]() { this->watchLoop(periodMs); });
    }

    void stopWatchdog() {
        if (!m_running) {
            return;
        }
        m_running = false;
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

    /* ---------------------- static helpers ---------------------------- */
    static std::vector<std::string> listPorts(uint16_t vid = 0, uint16_t pid = 0) {
        if (vid == 0 && pid == 0) {
            return SerialPort::listPorts();
        }
        return listPortsByVidPid(vid, pid);
    }

private:
    SerialPort m_port;
    uint32_t m_baud;
    std::string m_handshake, m_expected;
    uint16_t m_vid, m_pid;
    std::string m_portName;  // empty == not connected

    std::atomic<bool> m_running{false};
    std::thread m_thread;
    Callback m_cb;

    bool autoDetect(std::string& outPort) {
        if (m_vid || m_pid) {
            const std::vector<std::string> ports = listPortsByVidPid(m_vid, m_pid);
            for (size_t i = 0; i < ports.size(); ++i) {
                if (testHandshake(ports[i])) {
                    outPort = ports[i];
                    return true;
                }
            }
        }
        outPort = SerialPort::autoDetect(m_baud, m_handshake, m_expected);
        return !outPort.empty();
    }

    bool testHandshake(const std::string& port) {
        try {
            SerialPort sp;
            sp.open(port, m_baud);
            std::string dump;
            while (sp.readLine(dump, 50)) {
            }
            sp.writeLine(m_handshake);
            std::string r;
            const bool ok = sp.readLine(r, 300);
            sp.close();
            return ok && r == m_expected;
        } catch (...) {
            return false;
        }
    }

    /* ----------------------- watchdog loop --------------------------- */
    void watchLoop(uint32_t periodMs) {
        bool prevConnected = isConnected();
        while (m_running) {
            bool nowConnected = prevConnected;
            if (prevConnected) {
                const std::vector<std::string> ports = SerialPort::listPorts();
                if (std::find(ports.begin(), ports.end(), m_portName) == ports.end()) {
                    disconnect();
                    nowConnected = false;
                }
            } else {
                if (connect()) {
                    nowConnected = true;
                }
            }
            if (nowConnected != prevConnected && m_cb) {
                m_cb(nowConnected, m_portName);
            }
            prevConnected = nowConnected;
            std::this_thread::sleep_for(std::chrono::milliseconds(periodMs));
        }
    }

    static std::vector<std::string> listPortsByVidPid(uint16_t vid, uint16_t pid) {
        std::vector<std::string> out;
#ifdef WINDOWS_OS
        std::vector<std::string> out;
        const GUID gCom = {0x86E0D1E0, 0x8089, 0x11D0, {0x9C, 0xE4, 0x08, 0x00, 0x3E, 0x30, 0x1F, 0x73}};
        HDEVINFO info = SetupDiGetClassDevs(&gCom, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
        if (info == INVALID_HANDLE_VALUE) {
            return out;
        }
        SP_DEVICE_INTERFACE_DATA ifData;
        ifData.cbSize = sizeof(ifData);
        for (DWORD i = 0; SetupDiEnumDeviceInterfaces(info, NULL, &gCom, i, &ifData); ++i) {
            DWORD req = 0;
            SetupDiGetDeviceInterfaceDetail(info, &ifData, NULL, 0, &req, NULL);
            std::vector<char> buf(req);
            SP_DEVICE_INTERFACE_DETAIL_DATA* detail = reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA*>(buf.data());
            detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
            SP_DEVINFO_DATA devData;
            devData.cbSize = sizeof(devData);
            if (SetupDiGetDeviceInterfaceDetail(info, &ifData, detail, req, NULL, &devData)) {
                std::string devPath = detail->DevicePath;
                char pattern[32];
                sprintf(pattern, "VID_%04X&PID_%04X", vid, pid);
                if (devPath.find(pattern) != std::string::npos) {
                    HKEY hKey = SetupDiOpenDevRegKey(info, &devData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
                    if (hKey != INVALID_HANDLE_VALUE) {
                        char name[256];
                        DWORD type = REG_SZ;
                        DWORD len = sizeof(name);
                        if (RegQueryValueExA(hKey, "PortName", NULL, &type, reinterpret_cast<LPBYTE>(name), &len) == ERROR_SUCCESS) {
                            out.push_back(name);  // « COM7 », « COM15 », …
                        }
                        RegCloseKey(hKey);
                    }
                }
            }
        }
        SetupDiDestroyDeviceInfoList(info);
#else
        /* ---- POSIX implementation via sysfs ------------------------- */
        const std::vector<std::string> ttys = SerialPort::listPorts();
        for (size_t i = 0; i < ttys.size(); ++i) {
            const std::string& tty = ttys[i];
            size_t pos = tty.find_last_of('/');
            const std::string base = (pos == std::string::npos) ? tty.substr(5) : tty.substr(pos + 1);
            std::string sys = "/sys/class/tty/" + base + "/device/../";
            FILE* fv = fopen((sys + "idVendor").c_str(), "r");
            FILE* fp = fopen((sys + "idProduct").c_str(), "r");
            if (fv && fp) {
                unsigned v = 0, p = 0;
                fscanf(fv, "%x", &v);
                fscanf(fp, "%x", &p);
                fclose(fv);
                fclose(fp);
                if ((vid == 0 || v == vid) && (pid == 0 || p == pid)) {
                    out.push_back(tty);
                }
            }
        }
#endif
        return out;
    }
};

}  // namespace net
}  // namespace ez
