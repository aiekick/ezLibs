#include <TestEzNamedPipe.h>
#include <ezlibs/ezOS.hpp>
#include <ezlibs/ezNamedPipe.hpp>
#include <ezlibs/ezCTest.hpp>
#include <thread>

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

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

bool TestEzNamedPipeBasis() {
    const std::string message1 = "HELLO";
    auto serverPtr = ez::NamedPipe::Server::create("TestEzNamedPipeBasis", 512);
    CTEST_ASSERT_MESSAGE(serverPtr != nullptr, "server init ?");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Give server time to start
    auto clientPtr = ez::NamedPipe::Client::create("TestEzNamedPipeBasis");
    CTEST_ASSERT_MESSAGE(clientPtr != nullptr, "client init ?");
    CTEST_ASSERT_MESSAGE(clientPtr->writeString(message1), "client sending ?");
    CTEST_ASSERT_MESSAGE(serverPtr->isMessageReceived(), "server received a message ?");
    CTEST_ASSERT_MESSAGE(serverPtr->readString() == message1, "the message is the good one");
    serverPtr->unit(); // destroying the pipe
    const std::string message2 = "GOODBYE OR YOU ARE ALREADY SLEEPING";
#ifdef WINDOWS_OS
    // on windows the client can send nothing if the server is not joinable
    CTEST_ASSERT_MESSAGE(!clientPtr->writeString(message2), "client sending failed ?");
#else
    // but on linux we can always send even if the server is not joinable
    CTEST_ASSERT_MESSAGE(clientPtr->writeString(message2), "client sending failed ?");
#endif
    CTEST_ASSERT_MESSAGE(!serverPtr->isMessageReceived(), "server received nothing ?");
    CTEST_ASSERT_MESSAGE(serverPtr->readString() == message1, "server message is the oldest ?");
    clientPtr->unit(); // destroying the client
    CTEST_ASSERT_MESSAGE(!clientPtr->writeString(message2), "client sending failed ?");  // the message will be send on a destroyed pipe
    serverPtr.reset();
    clientPtr.reset();
    return true;
}

////////////////////////////////////////////////////////////////////////////
//// ENTRY POINT ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzNamedPipe(const std::string& vTest) {
    IfTestExist(TestEzNamedPipeBasis);
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
