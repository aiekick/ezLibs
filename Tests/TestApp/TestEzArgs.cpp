#include <TestEzArgs.h>
#include <ezlibs/ezArgs.hpp>
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

bool TestEzArgs_parsing() {
    try {
        std::vector<char*> arr{"--no-help", "-h"};
        ez::Args args("Test");
        args.addHeader("=========== test tool ===========").addFooter("=========== Thats all folks ===========");
        args.addOptional("--no-help");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isPresent("--no-help"));
        CTEST_ASSERT(args.isPresent("no-help"));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_delimiters() {
    try {
        std::vector<char*> arr{"-s", "sample.txt", "-t=target.txt", "-n:5"};
        ez::Args args("Test");
        args.addOptional("-s/--source").help("Source file", "SOURCE").delimiter(' ');
        args.addOptional("-t/--target").help("Target file", "TARGET").delimiter('=');
        args.addOptional("-n/--count").help("count files", "COUNT").delimiter(':');
        if (!args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U)) {
            return false;
        }
        args.printHelp();
        if (!args.isPresent("-s")) {
            return false;
        }
        if (!args.isPresent("s")) {
            return false;
        }
        if (!args.isPresent("--source")) {
            return false;
        }
        if (!args.isPresent("source")) {
            return false;
        }
        if (args.isPresent("-src")) {
            return false;
        }
        if (args.getValue<std::string>("-s") != "sample.txt") {
            return false;
        }
        if (args.getValue<std::string>("-t") != "target.txt") {
            return false;
        }
        if (args.getValue<int>("-n") != 5) {
            return false;
        }
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_delimiters_empties() {
    try {
        std::vector<char*> arr{"-s", "-t", "-n"};
        ez::Args args("Test");
        args.addOptional("-s/--source").help("Source file", "SOURCE").delimiter(' ');
        args.addOptional("-t/--target").help("Target file", "TARGET").delimiter('=');
        args.addOptional("-n/--count").help("count files", "COUNT").delimiter(':');
        if (!args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U)) {
            return false;
        }
        args.printHelp();
        if (!args.isPresent("-s")) {
            return false;
        }
        if (args.hasValue("-s")) {
            return false;
        }
        if (!args.isPresent("s")) {
            return false;
        }
        if (args.hasValue("s")) {
            return false;
        }
        if (!args.isPresent("--source")) {
            return false;
        }
        if (args.hasValue("--source")) {
            return false;
        }
        if (!args.isPresent("source")) {
            return false;
        }
        if (args.hasValue("source")) {
            return false;
        }
        if (args.isPresent("-src")) {
            return false;
        }
        if (args.hasValue("-src")) {
            return false;
        }
        if (!args.getValue<std::string>("-s").empty()) {
            return false;
        }
        if (!args.getValue<std::string>("-t").empty()) {
            return false;
        }
        if (args.getValue<int>("-n") != 0) {
            return false;
        }
        if (!args.getValue<std::string>("-src", true).empty()) {
            return false;
        }
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_groupeds() {
    try {
        std::vector<char*> arr{"cvzf", "sample.txt", "-ff=mode2"};
        ez::Args args("Test");
        args.addHeader("=========== test tool ===========").addFooter("=========== Thats all folks ===========").addDescription("Just a test");
        args.addOptional("c").help("Create");
        args.addOptional("v").help("Verbose");
        args.addOptional("z").help("gzip");
        args.addOptional("f").help("file").delimiter(' ');
        args.addOptional("-ff").help("file mode").delimiter('=');
        if (!args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U)) {
            return false;
        }
        args.printHelp();
        if (!args.isPresent("c")) {
            return false;
        }
        if (args.getValue<std::string>("c") == "sample.txt") {
            return false;
        }
        if (!args.isPresent("v")) {
            return false;
        }
        if (args.getValue<std::string>("v") == "sample.txt") {
            return false;
        }
        if (!args.isPresent("z")) {
            return false;
        }
        if (args.getValue<std::string>("z") == "sample.txt") {
            return false;
        }
        if (!args.isPresent("f")) {
            return false;
        }
        if (args.getValue<std::string>("f") != "sample.txt") {
            return false;
        }
        if (!args.isPresent("ff")) {
            return false;
        }
        if (args.getValue<std::string>("ff") != "mode2") {
            return false;
        }
        if (!args.isPresent("-ff")) {
            return false;
        }
        if (args.getValue<std::string>("-ff") != "mode2") {
            return false;
        }
    } catch (std::exception&) {
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzArgs(const std::string& vTest) {
    IfTestExist(TestEzArgs_parsing);
    else IfTestExist(TestEzArgs_delimiters);
    else IfTestExist(TestEzArgs_delimiters_empties);
    else IfTestExist(TestEzArgs_groupeds);
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
