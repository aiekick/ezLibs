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

// EzDll is part od the EzLibs project : https://github.com/aiekick/EzLibs.git

#include <fstream>
#include <iterator>
#include <sstream>
#include <cstdint>
#include <string>
#include <vector>
#include <ctime>

#include "ezLog.hpp"

#ifdef WIN32
#include "Windows.h"
#else
#include <dlfcn.h>
#endif

namespace ez::plugin {

template <class T>
class Loader {
private:
#ifdef WIN32
    HMODULE _handle = nullptr;
#else // UNIX
    void *_handle = nullptr;
#endif // WIN32 / UNIX

    std::string _pathToLib;
    std::string _allocClassSymbol;
    std::string _deleteClassSymbol;
    bool _isAPlugin = true;

public:
    Loader() = default;
    Loader(std::string const &pathToLib, std::string const &allocClassSymbol = "allocator", std::string const &deleteClassSymbol = "deleter")
        : _handle(nullptr), _pathToLib(pathToLib), _allocClassSymbol(allocClassSymbol), _deleteClassSymbol(deleteClassSymbol) {}
    ~Loader() = default;

    bool isValid() const  { return (_handle != nullptr); }

    bool isAPlugin() const  { return _isAPlugin; }

    void dlOpenLib() {
#ifdef WIN32
        _handle = LoadLibraryExA(
            _pathToLib.c_str(),
            NULL,  //
            LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
        if (!isValid()) {
            const auto &dw = GetLastError();
            if (dw == ERROR_MOD_NOT_FOUND) {
                LogVarError("Can't open and load %s with error : ERROR_MOD_NOT_FOUND", _pathToLib.c_str());
            } else {
                LogVarError("Can't open and load %s", _pathToLib.c_str());
            }
        }
#else  // UNIX
        _handle = dlopen(_pathToLib.c_str(), RTLD_NOW | RTLD_LAZY);
        if (!isValid()) {
            LogVarError("%s", dlerror());
        }
#endif  // WIN32 / UNIX
    }

    std::shared_ptr<T> dlGetInstance() {
        using allocClass = T *(*)();
        using deleteClass = void (*)(T *);
        if (isValid()) {
#ifdef WIN32
            auto allocFunc = reinterpret_cast<allocClass>(GetProcAddress(_handle, _allocClassSymbol.c_str()));
            auto deleteFunc = reinterpret_cast<deleteClass>(GetProcAddress(_handle, _deleteClassSymbol.c_str()));
#else  // UNIX
            auto allocFunc = reinterpret_cast<allocClass>(dlsym(_handle, _allocClassSymbol.c_str()));
            auto deleteFunc = reinterpret_cast<deleteClass>(dlsym(_handle, _deleteClassSymbol.c_str()));
#endif  // WIN32 / UNIX
            if (!allocFunc || !deleteFunc) {
                _isAPlugin = false;
                dlCloseLib();
                // LogVarDebugError("Can't find allocator or deleter symbol in %s", _pathToLib.c_str());
                return nullptr;
            }
            return std::shared_ptr<T>(allocFunc(), [deleteFunc](T *p) { deleteFunc(p); });
        }
        return nullptr;
    }

    void dlCloseLib() {
#ifdef WIN32
        if (isValid() && (FreeLibrary(_handle) == 0)) {
#else  // UNIX
        if (isValid() && (dlclose(_handle) != 0)) {
#endif  // WIN32 / UNIX
        // LogVarDebugError("Can't close %s", _pathToLib.c_str());
        }
    }
};

inline std::string getDLLExtention() {
#ifdef WIN32
    return "dll";
#elif defined(__linux__)
    return "so";
#elif defined(__APPLE__)
        return "dylib";
#elif
        return "";
#endif
}

template<class T_PluginInterface>
class PluginInstance {
public:
    enum class PluginReturnMsg { LOADING_SUCCEED = 1, LOADING_FAILED = 0, NOT_A_PLUGIN = -1 };
    typedef std::shared_ptr<T_PluginInterface> PluginInterfacePtr;
    typedef std::weak_ptr<T_PluginInterface> PluginInterfaceWeak;

private:
    Loader<T_PluginInterface> m_Loader;
    PluginInterfacePtr m_PluginInstance = nullptr;
    std::string m_Name;

public:
    PluginInstance() = default;
    ~PluginInstance() { unit(); }

    PluginReturnMsg init(const std::string &vName, const std::string &vFilePathName) {
        m_Name = vName;
        m_Loader = Loader<T_PluginInterface>(vFilePathName);
        m_Loader.dlOpenLib();
        m_PluginInstance = m_Loader.dlGetInstance();
        if (m_Loader.isAPlugin()) {
            if (m_Loader.isValid()) {
                if (m_PluginInstance) {
                    if (!m_PluginInstance->Init()) {
                        m_PluginInstance.reset();
                    } else {
                        return PluginReturnMsg::LOADING_SUCCEED;
                    }
                }
            }
            return PluginReturnMsg::LOADING_FAILED;
        }
        return PluginReturnMsg::NOT_A_PLUGIN;
    }

    void unit() {
        m_PluginInstance->Unit();
        m_PluginInstance.reset();
        m_Loader.dlCloseLib();
    }

    PluginInterfaceWeak get() const { return m_PluginInstance; }
};

}  // namespace ez::dll
