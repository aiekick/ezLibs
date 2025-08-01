#include <ezlibs/ezFileWatcher.hpp>
#include <ezlibs/ezCTest.hpp>
#include <string>

// Desactivation des warnings de conversion
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)  // Conversion from 'double' to 'float', possible loss of data
#pragma warning(disable : 4305)  // Truncation from 'double' to 'float'
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
bool TestEzFileWatcher_BasicTrigger() {
    using namespace ez;

#ifdef WINDOWS_OS
    const std::wstring fileW = L"watch_me.txt";
    const std::wstring dirW = L".";
    std::atomic<bool> triggered{false};

    {
        std::ofstream ofs("watch_me.txt");
        ofs << "init";
        ofs.flush();
    }

    FileWatcher watcher;
    watcher.watchFile(fileW, [&](const std::vector<std::wstring>& files) {
        for (const auto& f : files) {
            if (f.find(L"watch_me.txt") != std::wstring::npos) {
                triggered = true;
            }
        }
    });

    watcher.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    {
        std::ofstream ofs("watch_me.txt");
        ofs << "changed";
        ofs.flush();
    }

    int waited = 0;
    const int timeout = 3000;
    while (!triggered && waited < timeout) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        waited += 100;
    }

    watcher.stop();
    std::remove("watch_me.txt");

#else  // Unix / macOS
    const std::string file = "watch_me.txt";
    const std::string dir = ".";  // parent dir
    std::atomic<bool> triggered{false};

    {
        std::ofstream ofs(file);
        ofs << "init";
        ofs.flush();
    }

    FileWatcher watcher;
    watcher.watchFile(file, [&](const std::vector<std::string>& files) {
        for (const auto& f : files) {
            if (f.find("watch_me.txt") != std::string::npos) {
                triggered = true;
            }
        }
    });

    watcher.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    {
        std::ofstream ofs(file);
        ofs << "changed";
        ofs.flush();
    }

    int waited = 0;
    const int timeout = 3000;
    while (!triggered && waited < timeout) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        waited += 100;
    }

    watcher.stop();
    std::remove(file.c_str());
#endif

    CTEST_ASSERT(triggered);
    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzFileWatcher(const std::string& vTest) {
    IfTestExist(TestEzFileWatcher_BasicTrigger);
    return false;
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
