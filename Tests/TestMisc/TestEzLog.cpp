#include <TestEzLog.h>
#include <ezlibs/ezLog.hpp>
#include <ezlibs/ezCTest.hpp>

#include <string>

// Desactivation des warnings de conversion
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4305)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzLog_LogSimpleString() {
    ez::Log::ref().logSimpleString("Test message");
    return true;
}

bool TestEzLog_LogSimpleStringByType() {
    ez::Log::ref().logSimpleStringByType(ez::Log::LOGGING_MESSAGE_TYPE_INFOS, "Info message");
    ez::Log::ref().logSimpleStringByType(ez::Log::LOGGING_MESSAGE_TYPE_WARNING, "Warning message");
    ez::Log::ref().logSimpleStringByType(ez::Log::LOGGING_MESSAGE_TYPE_ERROR, "Error message");
    return true;
}

bool TestEzLog_LogStringWithFunction() {
    ez::Log::ref().logStringWithFunction("TestFunction", 42, "Test with function");
    return true;
}

bool TestEzLog_LogStringByTypeWithFunction() {
    ez::Log::ref().logStringByTypeWithFunction(ez::Log::LOGGING_MESSAGE_TYPE_INFOS, "TestFunction", 100, "Info with function");
    ez::Log::ref().logStringByTypeWithFunction(ez::Log::LOGGING_MESSAGE_TYPE_WARNING, "TestFunction", 101, "Warning with function");
    ez::Log::ref().logStringByTypeWithFunction(ez::Log::LOGGING_MESSAGE_TYPE_ERROR, "TestFunction", 102, "Error with function");
    return true;
}

bool TestEzLog_VerboseMode() {
    ez::Log::ref().setVerboseMode(true);
    CTEST_ASSERT(ez::Log::ref().isVerboseMode() == true);
    ez::Log::ref().setVerboseMode(false);
    CTEST_ASSERT(ez::Log::ref().isVerboseMode() == false);
    return true;
}

bool TestEzLog_LogMacros() {
    LogVarInfo("Test info message: %d", 42);
    LogVarWarning("Test warning message: %s", "test");
    LogVarError("Test error message: %f", 3.14);
    return true;
}

bool TestEzLog_LogLightMacros() {
    LogVarLightInfo("Light info: %d", 123);
    LogVarLightWarning("Light warning: %s", "warning");
    LogVarLightError("Light error: %d", 456);
    return true;
}

bool TestEzLog_LogDebugMacros() {
#ifndef NDEBUG
    LogVarDebugInfo("Debug info: %d", 1);
    LogVarDebugWarning("Debug warning: %d", 2);
    LogVarDebugError("Debug error: %d", 3);
#endif
    return true;
}

bool TestEzLog_CustomLogFunctor() {
    bool functorCalled = false;
    int receivedType = -1;
    std::string receivedMessage;

    ez::Log::LogMessageFunctor functor = [&](const int& type, const std::string& msg) {
        functorCalled = true;
        receivedType = type;
        receivedMessage = msg;
    };

    ez::Log::ref().setStandardLogMessageFunctor(functor);
    ez::Log::ref().logSimpleStringByType(ez::Log::LOGGING_MESSAGE_TYPE_WARNING, "Test functor");

    CTEST_ASSERT(functorCalled);
    CTEST_ASSERT(receivedType == ez::Log::LOGGING_MESSAGE_TYPE_WARNING);
    CTEST_ASSERT(receivedMessage.find("Test functor") != std::string::npos);

    ez::Log::ref().setStandardLogMessageFunctor(nullptr);
    return true;
}

bool TestEzLog_CloseAndReopen() {
    ez::Log::ref().close();
    ez::Log::ref().logSimpleString("After close");
    return true;
}

#ifdef _MSC_VER
bool TestEzLog_GetLastErrorAsString() {
    std::string error = ez::Log::ref().getLastErrorAsString();
    return true;
}
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzLog(const std::string& vTest) {
    IfTestExist(TestEzLog_LogSimpleString);
    else IfTestExist(TestEzLog_LogSimpleStringByType);
    else IfTestExist(TestEzLog_LogStringWithFunction);
    else IfTestExist(TestEzLog_LogStringByTypeWithFunction);
    else IfTestExist(TestEzLog_VerboseMode);
    else IfTestExist(TestEzLog_LogMacros);
    else IfTestExist(TestEzLog_LogLightMacros);
    else IfTestExist(TestEzLog_LogDebugMacros);
    else IfTestExist(TestEzLog_CustomLogFunctor);
    else IfTestExist(TestEzLog_CloseAndReopen);
#ifdef _MSC_VER
    else IfTestExist(TestEzLog_GetLastErrorAsString);
#endif
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
