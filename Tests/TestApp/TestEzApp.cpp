#include <TestEzApp.h>
#include <ezlibs/ezApp.hpp>
#include <ezlibs/ezCTest.hpp>

#include <exception>
#include <iostream>
#include <string>
#include <array>

// Desactivation des warnings de conversion
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)  // Conversion from 'double' to 'float', possible loss of data
#pragma warning(disable : 4305)  // Truncation from 'double' to 'float'
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wwrite-strings"
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzApp_DefaultConstructor() {
    ez::App app;
    // Default constructor leaves m_AppPath empty, but getAppPath() auto-populates it
    CTEST_ASSERT(!app.getAppPath().empty());
    return true;
}

bool TestEzApp_SetAppPath() {
    ez::App app;
    app.setAppPath("/path/to/my/app");
    CTEST_ASSERT(app.getAppPath() == "/path/to/my");
    return true;
}

bool TestEzApp_SetAppPathWithBackslash() {
    ez::App app;
    app.setAppPath("C:\\path\\to\\my\\app.exe");
    CTEST_ASSERT(app.getAppPath() == "C:\\path\\to\\my");
    return true;
}

bool TestEzApp_SetAppPathEmpty() {
    ez::App app;
    app.setAppPath("");
    // setAppPath() with empty string doesn't modify m_AppPath, so getAppPath() auto-populates it
    CTEST_ASSERT(!app.getAppPath().empty());
    return true;
}

bool TestEzApp_GetCurDirectory() {
    std::string curDir = ez::App::getCurDirectory();
    CTEST_ASSERT(!curDir.empty());
    return true;
}

bool TestEzApp_SetCurDirectory() {
    std::string originalDir = ez::App::getCurDirectory();
    CTEST_ASSERT(!originalDir.empty());

    #ifdef WINDOWS_OS
    const std::string testDir = "C:\\";
    #else
    const std::string testDir = "/tmp";
    #endif

    CTEST_ASSERT(ez::App::setCurDirectory(testDir));
    std::string newDir = ez::App::getCurDirectory();
    CTEST_ASSERT(!newDir.empty());

    ez::App::setCurDirectory(originalDir);
    return true;
}

bool TestEzApp_MiniFunction() {
    ez::App app;
    CTEST_ASSERT(app.mini(5, 10) == 5);
    CTEST_ASSERT(app.mini(10, 5) == 5);
    CTEST_ASSERT(app.mini(7, 7) == 7);
    CTEST_ASSERT(app.mini(-5, 3) == -5);
    return true;
}

bool TestEzApp_ConstructorWithArgv() {
    char arg0[] = "/path/to/executable";
    char* argv[] = {arg0};
    ez::App app(1, argv, false);
    CTEST_ASSERT(app.getAppPath() == "/path/to");
    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzApp(const std::string& vTest) {
    IfTestExist(TestEzApp_DefaultConstructor);
    else IfTestExist(TestEzApp_SetAppPath);
    else IfTestExist(TestEzApp_SetAppPathWithBackslash);
    else IfTestExist(TestEzApp_SetAppPathEmpty);
    else IfTestExist(TestEzApp_GetCurDirectory);
    else IfTestExist(TestEzApp_SetCurDirectory);
    else IfTestExist(TestEzApp_MiniFunction);
    else IfTestExist(TestEzApp_ConstructorWithArgv);
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
