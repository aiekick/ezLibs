#include <ezlibs/ezCTest.hpp>
#include <ezlibs/ezFile.hpp>
#include <ezlibs/ezTime.hpp>

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
// 
// Join using forward slash so Watcher sees relative paths consistently.
static std::string pathJoin(const std::string& dir, const std::string& name) {
    if (dir.empty()) {
        return name;
    }
    if (name.empty()) {
        return dir;
    }
    if (dir.back() == '/') {
        return dir + name;
    }
    return dir + "/" + name;
}

// Write/overwrite a file with content.
static void writeFile(const std::string& path, const std::string& content) {
    std::ofstream ofs(path.c_str(), std::ios::binary | std::ios::trunc);
    ofs << content;
    ofs.flush();
}

// Append to a file.
static void appendFile(const std::string& path, const std::string& content) {
    std::ofstream ofs(path.c_str(), std::ios::binary | std::ios::app);
    ofs << content;
    ofs.flush();
}

// Remove a file if it exists (best-effort).
static void removeFile(const std::string& path) {
    std::remove(path.c_str());
}

// Rename a file (best-effort). Returns true on success.
static bool renameFile(const std::string& oldPath, const std::string& newPath) {
#if defined(WINDOWS_OS)
    return MoveFileA(oldPath.c_str(), newPath.c_str()) != 0;
#else
    return std::rename(oldPath.c_str(), newPath.c_str()) == 0;
#endif
}

// Create a directory if it does not exist. Returns true if created or already exists.
static bool makeDir(const std::string& dirPath) {
#if defined(WINDOWS_OS)
    return CreateDirectoryA(dirPath.c_str(), NULL) != 0 || GetLastError() == ERROR_ALREADY_EXISTS;
#else
    return (mkdir(dirPath.c_str(), 0755) == 0) || (errno == EEXIST);
#endif
}

// Remove a directory (must be empty). Returns true on success.
static bool removeDir(const std::string& dirPath) {
#if defined(WINDOWS_OS)
    return RemoveDirectoryA(dirPath.c_str()) != 0;
#else
    return rmdir(dirPath.c_str()) == 0;
#endif
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
bool TestEzFileWatcher_start_stop() {
    ez::file::Watcher watcher;

    CTEST_ASSERT_MESSAGE(watcher.start() == false, "Cannot start because callback is not defined");
    CTEST_ASSERT_MESSAGE(watcher.stop() == false, "Cannot stop because not started");

    std::atomic<bool> triggered{false};
    watcher.setCallback([&](const std::set<ez::file::Watcher::PathResult>& files) {
        if (!files.empty()) {
            triggered = true;
        }
    });

    CTEST_ASSERT_MESSAGE(watcher.start() == true, "Can start because callback is defined");
    CTEST_ASSERT_MESSAGE(watcher.start() == false, "Cannot start again because already started");
    CTEST_ASSERT_MESSAGE(watcher.stop() == true, "Can stop because started");

    (void)triggered;  // not asserted here; just sanity callback wiring
    return true;
}

bool TestEzFileWatcher_Dir() {
    const std::string dir = "wd_path";
    const std::string fileName = "toto";
    const std::string renamedName = "titi";

    // Clean any previous artifacts
    removeFile(pathJoin(dir, fileName));
    removeFile(pathJoin(dir, renamedName));
    removeDir(dir);

    // Create directory
    makeDir(dir);

    std::vector<ez::file::Watcher::PathResult> results;
    ez::file::Watcher watcher;
    uint32_t timeoutMs{1000};  // give some headroom across platforms/CI
    std::mutex resultsMutex;

    bool retCreation = false;
    bool retModification = false;
    bool retRenaming = false;
    bool retDeletion = false;

    watcher.setCallback([&](const std::set<ez::file::Watcher::PathResult>& callbackResults) {
        if (!callbackResults.empty()) {
            std::lock_guard<std::mutex> lock(resultsMutex);
            results.push_back(*callbackResults.begin());
        }
    });

    LogVarLightInfo("== START =============================");

    CTEST_ASSERT_MESSAGE(watcher.start() == true, "Can start because callback is defined");

    CTEST_ASSERT_MESSAGE(watcher.watchDirectory("") == false, "Empty directory");
    CTEST_ASSERT(watcher.watchDirectory(dir) == true);

    // Creation
    {
        LogVarLightInfo("======================================");
        writeFile(pathJoin(dir, fileName), "init");
        retCreation = ez::time::waitUntil(
            [&] {
                std::lock_guard<std::mutex> lock(resultsMutex);
                for (const auto& result : results) {
                    const bool match =
                        (result.rootPath == dir &&      //
                         result.newPath == fileName &&  //
                         result.modifType == ez::file::Watcher::PathResult::ModifType::CREATION);
                    if (match) {
                        return true;
                    }
                }
                return false;
            },
            timeoutMs);
        {
            std::lock_guard<std::mutex> lock(resultsMutex);
            results.clear();
        }
    }

    // Modification
    {
        LogVarLightInfo("======================================");
        appendFile(pathJoin(dir, fileName), "Yihaa");
        retModification = ez::time::waitUntil(
            [&] {
                std::lock_guard<std::mutex> lock(resultsMutex);
                for (const auto& result : results) {
                    const bool match =
                        (result.rootPath == dir &&      //
                         result.newPath == fileName &&  //
                         result.modifType == ez::file::Watcher::PathResult::ModifType::MODIFICATION);
                    if (match) {
                        return true;
                    }
                }
                return false;
            },
            timeoutMs);
        {
            std::lock_guard<std::mutex> lock(resultsMutex);
            results.clear();
        }
    }

    // Rename
    {
        LogVarLightInfo("======================================");
        (void)renameFile(pathJoin(dir, fileName), pathJoin(dir, renamedName));
        retRenaming = ez::time::waitUntil(
            [&] {
                std::lock_guard<std::mutex> lock(resultsMutex);
                for (const auto& result : results) {
                    const bool match =
                        (result.rootPath == dir &&         //
#ifndef APPLE_OS
                         result.oldPath == fileName &&     //
#endif
                         result.newPath == renamedName &&  //
                         result.modifType == ez::file::Watcher::PathResult::ModifType::RENAMED);

                    if (match) {
                        return true;
                    }
                }
                return false;
            },
            timeoutMs);
        {
            std::lock_guard<std::mutex> lock(resultsMutex);
            results.clear();
        }
    }

    // Deletion
    {
        LogVarLightInfo("======================================");
        removeFile(pathJoin(dir, renamedName));
        retDeletion = ez::time::waitUntil(
            [&] {
                std::lock_guard<std::mutex> lock(resultsMutex);
                for (const auto& result : results) {
                    const bool match =
                        (result.rootPath == dir &&         //
                         result.newPath == renamedName &&  //
                         result.modifType == ez::file::Watcher::PathResult::ModifType::DELETION);
                    if (match) {
                        return true;
                    }
                }
                return false;
            },
            timeoutMs);
        {
            std::lock_guard<std::mutex> lock(resultsMutex);
            results.clear();
        }
    }

    LogVarLightInfo("== END ===============================");

    CTEST_ASSERT_MESSAGE(watcher.stop() == true, "Can stop");

    removeDir(dir);

    bool testStatus = true;
    CTEST_ASSERT_MESSAGE_DELAYED(testStatus, retCreation, "File " + fileName + " creation detected");
    CTEST_ASSERT_MESSAGE_DELAYED(testStatus, retModification, "File " + fileName + " modification detected");
    CTEST_ASSERT_MESSAGE_DELAYED(testStatus, retRenaming, "File " + renamedName + " renamed event detected");
    CTEST_ASSERT_MESSAGE_DELAYED(testStatus, retDeletion, "File " + renamedName + " deletion detected");
    CTEST_ASSERT_MESSAGE(testStatus, "Test Succeed");

    return true;
}

bool TestEzFileWatcher_File_PATH() {
    /*using namespace ez;

#ifdef WINDOWS_OS
    const std::string path = ".";
    const std::string file = "watch_me.txt";
    std::atomic<bool> triggered{false};

    write_file(file, "init");

    FileWatcher watcher;
    watcher.setCallback([&](const std::set<std::string>& files) {
        // Callback only includes matched files; verify our target is present
        for (const auto& f : files) {
            if (f.find("watch_me.txt") != std::string::npos) {  // use std::string::npos
                triggered = true;
            }
        }
    });

    watcher.watchFile(path, file);
    watcher.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    append_file(file, " changed");

    const bool ok = wait_until([&]() { return triggered.load(); }, 3000);
    watcher.stop();
    remove_file_silent(file);

    CTEST_ASSERT(ok);
#else
    // Non-Windows: not implemented in this version; keep test as trivially true
    CTEST_ASSERT(true);
#endif*/
    return true;
}

bool TestEzFileWatcher_File_GLOB() {
    /*using namespace ez;

#ifdef WINDOWS_OS
    const std::string f1 = "watch_glob_1.txt";
    const std::string f2 = "note.txt";
    write_file(f1, "x");
    write_file(f2, "x");

    std::atomic<bool> seen_f1{false};
    std::atomic<bool> seen_f2{false};  // should remain false
    FileWatcher watcher;

    watcher.setCallback([&](const std::set<std::string>& files) {
        if (files.count("watch_glob_1.txt"))
            seen_f1 = true;
        if (files.count("note.txt"))
            seen_f2 = true;  // should never be reported
    });

    watcher.watchFileGlob("watch_*.txt");
    watcher.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    append_file(f1, "!");
    append_file(f2, "!");  // must NOT be reported, pattern mismatch

    const bool ok = wait_until([&]() { return seen_f1.load(); }, 3000);
    watcher.stop();

    remove_file_silent(f1);
    remove_file_silent(f2);

    CTEST_ASSERT(ok);
    CTEST_ASSERT(!seen_f2.load());
#else
    CTEST_ASSERT(true);
#endif*/
    return true;
}

bool TestEzFileWatcher_File_REGEX() {
    /*using namespace ez;

#ifdef WINDOWS_OS
    const std::string fgood = "watch_123.log";
    const std::string fbad = "watch_a.log";
    write_file(fgood, "x");
    write_file(fbad, "x");

    std::atomic<bool> seen_good{false};
    std::atomic<bool> seen_bad{false};  // should remain false

    FileWatcher watcher;
    watcher.setCallback([&](const std::set<std::string>& files) {
        if (files.count(fgood))
            seen_good = true;
        if (files.count(fbad))
            seen_bad = true;  // should not appear
    });

    watcher.watchFileRegex(R"(watch_\d+\.log)");
    watcher.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    append_file(fgood, "!");
    append_file(fbad, "!");

    const bool ok = wait_until([&]() { return seen_good.load(); }, 3000);
    watcher.stop();

    remove_file_silent(fgood);
    remove_file_silent(fbad);

    CTEST_ASSERT(ok);
    CTEST_ASSERT(!seen_bad.load());
#else
    CTEST_ASSERT(true);
#endif*/
    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzFileWatcher(const std::string& vTest) {
    IfTestExist(TestEzFileWatcher_start_stop);
    else IfTestExist(TestEzFileWatcher_Dir);
    else IfTestExist(TestEzFileWatcher_File_PATH);
    else IfTestExist(TestEzFileWatcher_File_GLOB);
    else IfTestExist(TestEzFileWatcher_File_REGEX);
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
