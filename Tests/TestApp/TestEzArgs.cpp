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
        std::vector<char*> arr{"--no-help", "-t"};
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

bool TestEzArgs_parsing_help() {
    try {
        std::vector<char*> arr{"--no-help", "-h"};
        ez::Args args("Test");
        args.addHeader("=========== test tool ===========").addFooter("=========== Thats all folks ===========");
        args.addOptional("--no-help");
        CTEST_ASSERT(!args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U)); // print help
        CTEST_ASSERT(args.isPresent("--no-help"));
        CTEST_ASSERT(args.isPresent("no-help"));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_optional_delimiters() {
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

bool TestEzArgs_optional_delimiters_empties() {
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

bool TestEzArgs_optional_groupeds() {
    try {
        std::vector<char*> arr{"cvzf", "sample.txt", "-ff=mode2"};
        ez::Args args("Test");
        args.addHeader("=========== test tool ===========").addFooter("=========== Thats all folks ===========").addDescription("Just a test");
        args.addOptional("c").help("Create", "<c>");
        args.addOptional("v").help("Verbose", "<v>");
        args.addOptional("z").help("gzip", "<z>");
        args.addOptional("f").help("file", "<f>").delimiter(' ');
        args.addOptional("-ff").help("file mode", "<ff>").delimiter('=');
        if (!args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U)) {
            return false;
        }
        args.printHelp();
        if (!args.isPresent("c")) {
            return false;
        }
        if (args.getValue<std::string>("c") != "") {
            return false;
        }
        if (!args.isPresent("v")) {
            return false;
        }
        if (args.getValue<std::string>("v") != "") {
            return false;
        }
        if (!args.isPresent("z")) {
            return false;
        }
        if (args.getValue<std::string>("z") != "") {
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
bool TestEzArgs_optional_required() {
    try {
        std::vector<char*> arr{"cvzf", "sample.txt", "-ff=mode2"};
        ez::Args args("Test");
        args.addOptional("G").required(true);        
        if (args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U)) {
            return false;
        }
        if (args.getErrors().empty()) {
            return false;
        }
        if (args.getErrors().at(0) != "Error : Optional <G> not present") {
            return false;
        }
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_positional() {
    try {
        std::vector<char*> arr{"file1.txt", "file2.xml", "file3.csv"};
        ez::Args args("Test");
        args.addPositional("file1").help("file 1", "<file 1>");
        args.addPositional("file2").help("file 2", "<file 2>");
        args.addPositional("file3").help("file 3", "<file 3>");
        if (!args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U)) {
            return false;
        }
        if (!args.hasValue("file1")) {
            return false;
        }
        if (args.getValue<std::string>("file1") != "file1.txt") {
            return false;
        }
        if (!args.hasValue("file2")) {
            return false;
        }
        if (args.getValue<std::string>("file2") != "file2.xml") {
            return false;
        }
        if (!args.hasValue("file3")) {
            return false;
        }
        if (args.getValue<std::string>("file3") != "file3.csv") {
            return false;
        }
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_positional_optional() {
    try {
        std::vector<char*> arr{"TestApp", "-cvzf", "project.tar.gz"};
        ez::Args args("Test");
        args.addPositional("project").help("project", "<project>");
        args.addOptional("c").help("compress", "<compress>");
        args.addOptional("v").help("verbose", "<verbose>");
        args.addOptional("z").help("gzip", "<gzip>");
        args.addOptional("f").help("file", "<file>").delimiter(' ');
        args.addOptional("G");
        if (!args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U)) {
            return false;
        }
        if (!args.hasValue("project")) {
            return false;
        }
        if (args.getValue<std::string>("project") != "TestApp") {
            return false;
        }
        if (!args.isPresent("c")) {
            return false;
        }
        if (args.getValue<std::string>("c") != "") {
            return false;
        }
        if (!args.isPresent("v")) {
            return false;
        }
        if (args.getValue<std::string>("v") != "") {
            return false;
        }
        if (!args.isPresent("z")) {
            return false;
        }
        if (args.getValue<std::string>("z") != "") {
            return false;
        }
        if (!args.isPresent("f")) {
            return false;
        }
        if (!args.hasValue("f")) {
            return false;
        }
        if (args.getValue<std::string>("f") != "project.tar.gz") {
            return false;
        }
        if (args.isPresent("G")) {
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
    else IfTestExist(TestEzArgs_parsing_help);
    else IfTestExist(TestEzArgs_optional_delimiters);
    else IfTestExist(TestEzArgs_optional_delimiters_empties);
    else IfTestExist(TestEzArgs_optional_groupeds);
    else IfTestExist(TestEzArgs_optional_required);
    else IfTestExist(TestEzArgs_positional);
    else IfTestExist(TestEzArgs_positional_optional);
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
