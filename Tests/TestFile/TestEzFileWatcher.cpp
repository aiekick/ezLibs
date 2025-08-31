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

// ---------- Local helpers (tests only) ----------

/* Write/overwrite a file with content. */
static void writeFile(const std::string& path, const std::string& content) {
    LogVarLightInfo("== ACTION ON FS ==> Create file %s", path.c_str());
    std::ofstream ofs(path.c_str(), std::ios::binary | std::ios::trunc);
    ofs << content;
    ofs.flush();
}

/* Append to a file to ensure a write event. */
static void appendFile(const std::string& path, const std::string& content) {
    LogVarLightInfo("== ACTION ON FS ==> Modify file %s", path.c_str());
    std::ofstream ofs(path.c_str(), std::ios::binary | std::ios::app);
    ofs << content;
    ofs.flush();
}

/* Remove a file if exists (best-effort). */
static void removeFile(const std::string& path) {
    LogVarLightInfo("== ACTION ON FS ==> Delete file %s", path.c_str());
    std::remove(path.c_str());
}

#ifdef WINDOWS_OS
/* Append to a file to ensure a write event. */
static void renameFile(const std::string& vOldPath, const std::string& vNewPath) {
    LogVarLightInfo("== ACTION ON FS ==> Rename file %s to %s", vOldPath.c_str(), vNewPath.c_str());
    MoveFileA(vOldPath.c_str(), vNewPath.c_str());
}

/* Create a directory if it does not exist. */
static bool makeDir(const std::string& vDir) {
    std::wstring w;
    // minimal UTF8->UTF16 helper using ez::str if available, else ANSI fallback
    // Since ez::str::utf8Decode is in the lib, call it indirectly by relying on watcher header including it.
    // For tests, CreateDirectoryA is enough for ASCII test names.
    LogVarLightInfo("== ACTION ON FS ==> Create dir %s", vDir.c_str());
    return CreateDirectoryA(vDir.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
}

/* Remove directory (must be empty). */
static void removeDir(const std::string& vDir) {
    LogVarLightInfo("== ACTION ON FS ==> Delete dir %s", vDir.c_str());
    RemoveDirectoryA(vDir.c_str());
}
#endif

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
    CTEST_ASSERT_MESSAGE(watcher.stop() == true, "Can stop becasue started");

    return true;
}

bool TestEzFileWatcher_Dir() {
#ifdef WINDOWS_OS
    const std::string dir = "wd_path";
    const std::string file = "toto";
    const std::string renamedFile = "titi";

    // clean an eventual last test
    removeFile(dir + "\\" + file);
    removeFile(dir + "\\" + renamedFile);
    removeDir(dir);

    // create dir + file
    makeDir(dir);

    std::vector<ez::file::Watcher::PathResult> results;
    ez::file::Watcher watcher;
    uint32_t timeout{500};
    std::mutex mutex;

    bool ret_creation = false;
    bool ret_modification = false;
    bool ret_renaming = false;
    bool ret_deletion = false;

    watcher.setCallback([&](const std::set<ez::file::Watcher::PathResult>& vResults) {
        if (!vResults.empty()) {
            std::lock_guard<std::mutex> _{mutex};  // portect acceses on result
            results.push_back(*vResults.begin());
        }
    });

    LogVarLightInfo("== START =============================");

    CTEST_ASSERT_MESSAGE(watcher.start() == true, "Can start because callback is defined");

    CTEST_ASSERT_MESSAGE(watcher.watchDirectory("") == false, "Empty directory");
    CTEST_ASSERT(watcher.watchDirectory(dir) == true);

    {
        LogVarLightInfo("======================================");
        writeFile(dir + "\\" + file, "init");
        ret_creation = ez::time::waitUntil(  //
            [&] {
                bool ret = false;
                {
                    std::lock_guard<std::mutex> _{mutex};  // portect acceses on result
                    for (const auto& result : results) {
                        ret =
                            (result.rootPath == dir &&  //
                             result.newPath == file &&  //
                             result.modifType == ez::file::Watcher::PathResult::ModifType::CREATION);
                        if (ret) {
                            break;
                        }
                    }
                }
                return ret;
            },
            timeout);
        results.clear();
    }

    {
        LogVarLightInfo("======================================");
        appendFile(dir + "\\" + file, "Yihaa");
        ret_modification = ez::time::waitUntil(  //
            [&] {
                bool ret = false;
                {
                    std::lock_guard<std::mutex> _{mutex};  // portect acceses on result
                    for (const auto& result : results) {
                        ret =
                            (result.rootPath == dir &&  //
                             result.newPath == file &&  //
                             result.modifType == ez::file::Watcher::PathResult::ModifType::MODIFICATION);
                        if (ret) {
                            break;
                        }
                    }
                }
                return ret;
            },
            timeout);
        results.clear();
    }

    {
        LogVarLightInfo("======================================");
        renameFile(dir + "\\" + file, dir + "\\" + renamedFile);
        ret_renaming = ez::time::waitUntil(  //
            [&] {
                bool ret = false;
                {
                    std::lock_guard<std::mutex> _{mutex};  // portect acceses on result
                    for (const auto& result : results) {
                        ret =
                            (result.rootPath == dir &&         //
                             result.oldPath == file &&         //
                             result.newPath == renamedFile &&  //
                             result.modifType == ez::file::Watcher::PathResult::ModifType::RENAMED);
                        if (ret) {
                            break;
                        }
                    }
                }
                return ret;
            },
            timeout);
        results.clear();
    }

    {
        LogVarLightInfo("======================================");
        removeFile(dir + "\\" + renamedFile);
        ret_deletion = ez::time::waitUntil(  //
            [&] {
                bool ret = false;
                {
                    std::lock_guard<std::mutex> _{mutex};  // portect acceses on result
                    for (const auto& result : results) {
                        ret =
                            (result.rootPath == dir &&         //
                             result.newPath == renamedFile &&  //
                             result.modifType == ez::file::Watcher::PathResult::ModifType::DELETION);
                        if (ret) {
                            break;
                        }
                    }
                }
                return ret;
            },
            timeout);
        results.clear();
    }

    LogVarLightInfo("== END ===============================");

    CTEST_ASSERT_MESSAGE(watcher.stop() == true, "Can stop");

    removeDir(dir);
    
    bool testStatus = true;
    CTEST_ASSERT_MESSAGE_DELAYED(testStatus, ret_creation, "File " + file + " creation detected");
    CTEST_ASSERT_MESSAGE_DELAYED(testStatus, ret_modification, "File " + file + " modification detected");
    CTEST_ASSERT_MESSAGE_DELAYED(testStatus, ret_renaming, "File " + renamedFile + " renamed to " + file + "renaming detected");
    CTEST_ASSERT_MESSAGE_DELAYED(testStatus, ret_deletion, "File " + file + " deletion detected");
    CTEST_ASSERT_MESSAGE(testStatus, "Test Succeed");
#endif

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
