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

bool TestEzNamedPipe_Basis() {
    const std::string message1 = "HELLO";
    auto serverPtr = ez::NamedPipe::Server::create("TestEzNamedPipe_Basis", 512);
    CTEST_ASSERT_MESSAGE(serverPtr != nullptr, "server init ?");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Give server time to start
    auto clientPtr = ez::NamedPipe::Client::create("TestEzNamedPipe_Basis");
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

bool TestEzNamedPipe_GetActivePipes() {
    auto pipes = ez::NamedPipe::getActivePipes();
    // Just check that the function doesn't crash
    return true;
}

bool TestEzNamedPipe_BufferCommunication() {
    auto serverPtr = ez::NamedPipe::Server::create("TestEzNamedPipe_Buffer", 1024);
    CTEST_ASSERT(serverPtr != nullptr);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto clientPtr = ez::NamedPipe::Client::create("TestEzNamedPipe_Buffer");
    CTEST_ASSERT(clientPtr != nullptr);

    // Send buffer data
    ez::NamedPipe::DatasBuffer buffer = {'H', 'e', 'l', 'l', 'o', '\0'};
    CTEST_ASSERT(clientPtr->writeBuffer(buffer));
    CTEST_ASSERT(serverPtr->isMessageReceived());

    size_t size = 0;
    auto received = serverPtr->readBuffer(size);
    CTEST_ASSERT(size == buffer.size());

    return true;
}

bool TestEzNamedPipe_MultipleMessages() {
    auto serverPtr = ez::NamedPipe::Server::create("TestEzNamedPipe_Multi", 512);
    CTEST_ASSERT(serverPtr != nullptr);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto clientPtr = ez::NamedPipe::Client::create("TestEzNamedPipe_Multi");
    CTEST_ASSERT(clientPtr != nullptr);

    // Send multiple messages
    CTEST_ASSERT(clientPtr->writeString("Message1"));
    CTEST_ASSERT(serverPtr->isMessageReceived());
    CTEST_ASSERT(serverPtr->readString() == "Message1");

    CTEST_ASSERT(clientPtr->writeString("Message2"));
    CTEST_ASSERT(serverPtr->isMessageReceived());
    CTEST_ASSERT(serverPtr->readString() == "Message2");

    return true;
}

bool TestEzNamedPipe_EmptyMessage() {
    auto serverPtr = ez::NamedPipe::Server::create("TestEzNamedPipe_Empty", 512);
    CTEST_ASSERT(serverPtr != nullptr);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto clientPtr = ez::NamedPipe::Client::create("TestEzNamedPipe_Empty");
    CTEST_ASSERT(clientPtr != nullptr);

    // Try to send empty message
    CTEST_ASSERT(!clientPtr->writeString(""));

    return true;
}

////////////////////////////////////////////////////////////////////////////
//// ENTRY POINT ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzNamedPipe(const std::string& vTest) {
    IfTestExist(TestEzNamedPipe_Basis);
    else IfTestExist(TestEzNamedPipe_GetActivePipes);
    else IfTestExist(TestEzNamedPipe_BufferCommunication);
    else IfTestExist(TestEzNamedPipe_MultipleMessages);
    else IfTestExist(TestEzNamedPipe_EmptyMessage);
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
