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

// ========== Basic parsing ==========

bool TestEzArgs_parsing() {
    try {
        std::vector<char*> arr{"--no-help"};
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

bool TestEzArgs_unknown_option() {
    try {
        std::vector<char*> arr{"--no-help", "-t"};
        ez::Args args("Test");
        args.addHeader("=========== test tool ===========").addFooter("=========== Thats all folks ===========");
        args.addOptional("--no-help");
        CTEST_ASSERT(!args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.getErrors().size() == 1U);
        CTEST_ASSERT(args.getErrors().front() == "Error : Unknown argument: -t");
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Constructor & init ==========

bool TestEzArgs_default_constructor() {
    try {
        ez::Args args;
        CTEST_ASSERT(args.init("MyApp", "-h/--help"));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_empty_name() {
    try {
        ez::Args args;
        CTEST_ASSERT(!args.init("", "-h/--help"));
        CTEST_ASSERT(args.hasErrors());
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_custom_help_key() {
    try {
        // Using a custom help key, requesting help should trigger help and return false
        std::vector<char*> arr{"--aide"};
        ez::Args args("Test", "--aide");
        CTEST_ASSERT(!args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Help trigger ==========

bool TestEzArgs_help_short() {
    try {
        std::vector<char*> arr{"-h"};
        ez::Args args("Test");
        CTEST_ASSERT(!args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_help_long() {
    try {
        std::vector<char*> arr{"--help"};
        ez::Args args("Test");
        CTEST_ASSERT(!args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Header, footer, description ==========

bool TestEzArgs_header_footer_description() {
    try {
        ez::Args args("Test");
        args.addHeader("HEADER").addFooter("FOOTER").addDescription("DESC");
        std::stringstream ss;
        args.getHelp(ss, "  ");
        auto help = ss.str();
        CTEST_ASSERT(help.find("HEADER") != std::string::npos);
        CTEST_ASSERT(help.find("FOOTER") != std::string::npos);
        CTEST_ASSERT(help.find("DESC") != std::string::npos);
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Optional arguments with aliases ==========

bool TestEzArgs_optional_alias() {
    try {
        std::vector<char*> arr{"-v"};
        ez::Args args("Test");
        args.addOptional("-v/--verbose").help("Enable verbose", "");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isPresent("-v"));
        CTEST_ASSERT(args.isPresent("--verbose"));
        CTEST_ASSERT(args.isPresent("v"));
        CTEST_ASSERT(args.isPresent("verbose"));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_optional_alias_long_form() {
    try {
        std::vector<char*> arr{"--verbose"};
        ez::Args args("Test");
        args.addOptional("-v/--verbose");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isPresent("-v"));
        CTEST_ASSERT(args.isPresent("--verbose"));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Optional with value (delimiter '=') ==========

bool TestEzArgs_optional_value_eq_delimiter() {
    try {
        std::vector<char*> arr{"--output=file.txt"};
        ez::Args args("Test");
        args.addOptional("-o/--output").help("Output file", "FILE").delimiter('=');
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isPresent("--output"));
        CTEST_ASSERT(args.hasValue("--output"));
        CTEST_ASSERT(args.getValue<std::string>("--output") == "file.txt");
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Optional with value (delimiter ' ') ==========

bool TestEzArgs_optional_value_space_delimiter() {
    try {
        std::vector<char*> arr{"--output", "file.txt"};
        ez::Args args("Test");
        args.addOptional("-o/--output").help("Output file", "FILE").delimiter(' ');
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isPresent("--output"));
        CTEST_ASSERT(args.hasValue("--output"));
        CTEST_ASSERT(args.getValue<std::string>("--output") == "file.txt");
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Optional with default value ==========

bool TestEzArgs_optional_default_value() {
    try {
        std::vector<char*> arr{};
        ez::Args args("Test");
        args.addOptional("--level").def("5").delimiter('=');
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(!args.isPresent("--level"));
        CTEST_ASSERT(args.getValue<int>("--level") == 5);
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Required optional missing ==========

bool TestEzArgs_required_optional_missing() {
    try {
        std::vector<char*> arr{};
        ez::Args args("Test");
        args.addOptional("--config").required(true).delimiter('=');
        CTEST_ASSERT(!args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.hasErrors());
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_required_optional_present() {
    try {
        std::vector<char*> arr{"--config=app.cfg"};
        ez::Args args("Test");
        args.addOptional("--config").required(true).delimiter('=');
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.getValue<std::string>("--config") == "app.cfg");
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Positional arguments ==========

bool TestEzArgs_positional_single() {
    try {
        std::vector<char*> arr{"input.txt"};
        ez::Args args("Test");
        args.addPositional("input").help("Input file", "FILE");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isPresent("input"));
        CTEST_ASSERT(args.hasValue("input"));
        CTEST_ASSERT(args.getValue<std::string>("input") == "input.txt");
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_positional_multiple() {
    try {
        std::vector<char*> arr{"in.txt", "out.txt"};
        ez::Args args("Test");
        args.addPositional("src").help("Source", "SRC");
        args.addPositional("dst").help("Destination", "DST");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.getValue<std::string>("src") == "in.txt");
        CTEST_ASSERT(args.getValue<std::string>("dst") == "out.txt");
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_positional_missing() {
    try {
        std::vector<char*> arr{};
        ez::Args args("Test");
        args.addPositional("input");
        CTEST_ASSERT(!args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.hasErrors());
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Positional array ==========

bool TestEzArgs_positional_array_fixed() {
    try {
        std::vector<char*> arr{"a.txt", "b.txt", "c.txt"};
        ez::Args args("Test");
        args.addPositional("files").help("Input files", "FILES").array(3);
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isArray("files"));
        auto vals = args.getArrayValues("files");
        CTEST_ASSERT(vals.size() == 3U);
        CTEST_ASSERT(vals[0] == "a.txt");
        CTEST_ASSERT(vals[1] == "b.txt");
        CTEST_ASSERT(vals[2] == "c.txt");
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_positional_array_range() {
    try {
        std::vector<char*> arr{"a.txt", "b.txt"};
        ez::Args args("Test");
        args.addPositional("files").array(1, 5);
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        auto vals = args.getArrayValues("files");
        CTEST_ASSERT(vals.size() == 2U);
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_positional_array_too_few() {
    try {
        std::vector<char*> arr{"a.txt"};
        ez::Args args("Test");
        args.addPositional("files").array(3);
        CTEST_ASSERT(!args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.hasErrors());
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_positional_array_unlimited() {
    try {
        std::vector<char*> arr{"a", "b", "c", "d", "e"};
        ez::Args args("Test");
        args.addPositional("items").arrayUnlimited();
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        auto vals = args.getArrayValues("items");
        CTEST_ASSERT(vals.size() == 5U);
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Optional array (space delimiter) ==========

bool TestEzArgs_optional_array_space() {
    try {
        std::vector<char*> arr{"--files", "a.txt", "b.txt", "c.txt"};
        ez::Args args("Test");
        args.addOptional("--files").help("Files", "FILE").delimiter(' ').array(1, 5);
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isPresent("--files"));
        CTEST_ASSERT(args.isArray("--files"));
        auto vals = args.getArrayValues("--files");
        CTEST_ASSERT(vals.size() == 3U);
        CTEST_ASSERT(vals[0] == "a.txt");
        CTEST_ASSERT(vals[1] == "b.txt");
        CTEST_ASSERT(vals[2] == "c.txt");
        // getValue should return first element
        CTEST_ASSERT(args.getValue<std::string>("--files") == "a.txt");
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_optional_array_unlimited() {
    try {
        std::vector<char*> arr{"--tags", "foo", "bar", "baz"};
        ez::Args args("Test");
        args.addOptional("--tags").delimiter(' ').arrayUnlimited();
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        auto vals = args.getArrayValues("--tags");
        CTEST_ASSERT(vals.size() == 3U);
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== getValue with type conversions ==========

bool TestEzArgs_getValue_int() {
    try {
        std::vector<char*> arr{"--port=8080"};
        ez::Args args("Test");
        args.addOptional("--port").delimiter('=');
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.getValue<int>("--port") == 8080);
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_getValue_float() {
    try {
        std::vector<char*> arr{"--ratio=3.14"};
        ez::Args args("Test");
        args.addOptional("--ratio").delimiter('=');
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        auto val = args.getValue<float>("--ratio");
        CTEST_ASSERT(val > 3.13f && val < 3.15f);
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_getValue_double() {
    try {
        std::vector<char*> arr{"--precision=1.23456789"};
        ez::Args args("Test");
        args.addOptional("--precision").delimiter('=');
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        auto val = args.getValue<double>("--precision");
        CTEST_ASSERT(val > 1.234 && val < 1.235);
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_getValue_bool_true() {
    try {
        std::vector<char*> arr{"--flag=true"};
        ez::Args args("Test");
        args.addOptional("--flag").delimiter('=');
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.getValue<bool>("--flag") == true);
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_getValue_bool_one() {
    try {
        std::vector<char*> arr{"--flag=1"};
        ez::Args args("Test");
        args.addOptional("--flag").delimiter('=');
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.getValue<bool>("--flag") == true);
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_getValue_bool_false() {
    try {
        std::vector<char*> arr{"--flag=false"};
        ez::Args args("Test");
        args.addOptional("--flag").delimiter('=');
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.getValue<bool>("--flag") == false);
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_getValue_unknown_key() {
    try {
        std::vector<char*> arr{};
        ez::Args args("Test");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        // getValue on unknown key should return default-constructed value
        CTEST_ASSERT(args.getValue<int>("--unknown") == 0);
        CTEST_ASSERT(args.getValue<std::string>("--unknown").empty());
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== getArrayValues with typed conversion ==========

bool TestEzArgs_getArrayValues_typed() {
    try {
        std::vector<char*> arr{"--nums", "1", "2", "3"};
        ez::Args args("Test");
        args.addOptional("--nums").delimiter(' ').arrayUnlimited();
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        auto vals = args.getArrayValues<int>("--nums");
        CTEST_ASSERT(vals.size() == 3U);
        CTEST_ASSERT(vals[0] == 1);
        CTEST_ASSERT(vals[1] == 2);
        CTEST_ASSERT(vals[2] == 3);
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Combined short args ==========

bool TestEzArgs_combined_short_args() {
    try {
        std::vector<char*> arr{"-abc"};
        ez::Args args("Test");
        args.addOptional("-a/--alpha");
        args.addOptional("-b/--beta");
        args.addOptional("-c/--charlie");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isPresent("-a"));
        CTEST_ASSERT(args.isPresent("-b"));
        CTEST_ASSERT(args.isPresent("-c"));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_combined_short_args_unknown() {
    try {
        std::vector<char*> arr{"-abz"};
        ez::Args args("Test");
        args.addOptional("-a/--alpha");
        args.addOptional("-b/--beta");
        CTEST_ASSERT(!args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.hasErrors());
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Commands ==========

bool TestEzArgs_command_basic() {
    try {
        std::vector<char*> arr{"build"};
        ez::Args args("Test");
        args.addCommand("build").help("Build the project", "");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isCommand("build"));
        CTEST_ASSERT(args.getActiveCommand() != nullptr);
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_command_not_active() {
    try {
        std::vector<char*> arr{};
        ez::Args args("Test");
        args.addCommand("build");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(!args.isCommand("build"));
        CTEST_ASSERT(args.getActiveCommand() == nullptr);
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_command_with_sub_positional() {
    try {
        std::vector<char*> arr{"deploy", "production"};
        ez::Args args("Test");
        auto& cmd = args.addCommand("deploy").help("Deploy the app", "");
        cmd.addPositional("target").help("Deploy target", "TARGET");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isCommand("deploy"));
        CTEST_ASSERT(args.isPresent("target"));
        CTEST_ASSERT(args.getValue<std::string>("target") == "production");
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_command_with_sub_optional() {
    try {
        std::vector<char*> arr{"run", "--force"};
        ez::Args args("Test");
        auto& cmd = args.addCommand("run").help("Run", "");
        cmd.addOptional("-f/--force").help("Force run", "");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isCommand("run"));
        CTEST_ASSERT(args.isPresent("--force"));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_command_missing_required_sub_positional() {
    try {
        std::vector<char*> arr{"deploy"};
        ez::Args args("Test");
        auto& cmd = args.addCommand("deploy");
        cmd.addPositional("target");
        CTEST_ASSERT(!args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.hasErrors());
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_command_missing_required_sub_optional() {
    try {
        std::vector<char*> arr{"deploy"};
        ez::Args args("Test");
        auto& cmd = args.addCommand("deploy");
        cmd.addOptional("--env").required(true).delimiter('=');
        CTEST_ASSERT(!args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.hasErrors());
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_command_with_sub_optional_value() {
    try {
        std::vector<char*> arr{"run", "--env=staging"};
        ez::Args args("Test");
        auto& cmd = args.addCommand("run");
        cmd.addOptional("--env").delimiter('=');
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isCommand("run"));
        CTEST_ASSERT(args.getValue<std::string>("--env") == "staging");
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Mixed positional + optional ==========

bool TestEzArgs_mixed_positional_and_optional() {
    try {
        std::vector<char*> arr{"--verbose", "input.txt"};
        ez::Args args("Test");
        args.addOptional("-v/--verbose");
        args.addPositional("input");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isPresent("--verbose"));
        CTEST_ASSERT(args.getValue<std::string>("input") == "input.txt");
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_positional_then_optional() {
    try {
        std::vector<char*> arr{"input.txt", "--verbose"};
        ez::Args args("Test");
        args.addOptional("-v/--verbose");
        args.addPositional("input");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isPresent("--verbose"));
        CTEST_ASSERT(args.getValue<std::string>("input") == "input.txt");
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Global optional + command ==========

bool TestEzArgs_global_optional_with_command() {
    try {
        std::vector<char*> arr{"--verbose", "build"};
        ez::Args args("Test");
        args.addOptional("-v/--verbose");
        args.addCommand("build");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isPresent("--verbose"));
        CTEST_ASSERT(args.isCommand("build"));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== isPresent / hasValue / isArray for not present ==========

bool TestEzArgs_isPresent_false() {
    try {
        std::vector<char*> arr{};
        ez::Args args("Test");
        args.addOptional("--debug");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(!args.isPresent("--debug"));
        CTEST_ASSERT(!args.hasValue("--debug"));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_isArray_false() {
    try {
        std::vector<char*> arr{"--debug"};
        ez::Args args("Test");
        args.addOptional("--debug");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(!args.isArray("--debug"));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== getArrayValues on non-array returns empty ==========

bool TestEzArgs_getArrayValues_non_array() {
    try {
        std::vector<char*> arr{"--debug"};
        ez::Args args("Test");
        args.addOptional("--debug");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        auto vals = args.getArrayValues("--debug");
        CTEST_ASSERT(vals.empty());
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Multiple errors ==========

bool TestEzArgs_multiple_errors() {
    try {
        std::vector<char*> arr{"--unknown1", "--unknown2"};
        ez::Args args("Test");
        CTEST_ASSERT(!args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.getErrors().size() >= 2U);
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Empty args parse succeeds if no required ==========

bool TestEzArgs_empty_args_no_required() {
    try {
        std::vector<char*> arr{};
        ez::Args args("Test");
        args.addOptional("--verbose");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(!args.hasErrors());
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== startIdx parameter ==========

bool TestEzArgs_parse_startIdx() {
    try {
        // Simulating typical argc/argv where argv[0] is the program name
        std::vector<char*> arr{"myapp", "--verbose"};
        ez::Args args("Test");
        args.addOptional("--verbose");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 1U));
        CTEST_ASSERT(args.isPresent("--verbose"));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Optional with type() ==========

bool TestEzArgs_optional_type_annotation() {
    try {
        std::vector<char*> arr{"--port=9090"};
        ez::Args args("Test");
        args.addOptional("--port").type("int").delimiter('=').help("Port number", "PORT");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.getValue<int>("--port") == 9090);
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Positional with type() ==========

bool TestEzArgs_positional_type_annotation() {
    try {
        std::vector<char*> arr{"42"};
        ez::Args args("Test");
        args.addPositional("count").type("int");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.getValue<int>("count") == 42);
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Positional starting with dash fails ==========

bool TestEzArgs_positional_dash_error() {
    try {
        std::vector<char*> arr{"-bad"};
        ez::Args args("Test");
        args.addPositional("input");
        CTEST_ASSERT(!args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.hasErrors());
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Empty key errors ==========

bool TestEzArgs_empty_optional_key() {
    try {
        ez::Args args("Test");
        args.addOptional("");
        CTEST_ASSERT(args.hasErrors());
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_empty_positional_key() {
    try {
        ez::Args args("Test");
        args.addPositional("");
        CTEST_ASSERT(args.hasErrors());
    } catch (std::exception&) {
        return false;
    }
    return true;
}

bool TestEzArgs_empty_command_key() {
    try {
        ez::Args args("Test");
        args.addCommand("");
        CTEST_ASSERT(args.hasErrors());
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== printHelp output ==========

bool TestEzArgs_printHelp_output() {
    try {
        ez::Args args("MyApp");
        args.addHeader("=== MyApp ===").addFooter("=== End ===").addDescription("A great tool");
        args.addOptional("-v/--verbose").help("Verbose mode", "");
        args.addOptional("-o/--output").help("Output file", "FILE").delimiter('=');
        args.addPositional("input").help("Input file", "INPUT");
        std::stringstream ss;
        args.getHelp(ss, "  ");
        auto help = ss.str();
        CTEST_ASSERT(help.find("=== MyApp ===") != std::string::npos);
        CTEST_ASSERT(help.find("=== End ===") != std::string::npos);
        CTEST_ASSERT(help.find("A great tool") != std::string::npos);
        CTEST_ASSERT(help.find("Verbose mode") != std::string::npos);
        CTEST_ASSERT(help.find("Output file") != std::string::npos);
        CTEST_ASSERT(help.find("Input file") != std::string::npos);
        CTEST_ASSERT(help.find("usage : MyApp") != std::string::npos);
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Chaining API ==========

bool TestEzArgs_chaining() {
    try {
        std::vector<char*> arr{"--output=test.bin"};
        ez::Args args("Test");
        args.addHeader("H").addFooter("F").addDescription("D");
        args.addOptional("-o/--output").help("Output", "FILE").type("string").delimiter('=').def("default.bin");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.getValue<std::string>("--output") == "test.bin");
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Optional array stops at known argument ==========

bool TestEzArgs_optional_array_stops_at_known_arg() {
    try {
        std::vector<char*> arr{"--files", "a.txt", "b.txt", "--verbose"};
        ez::Args args("Test");
        args.addOptional("--files").delimiter(' ').arrayUnlimited();
        args.addOptional("--verbose");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        auto vals = args.getArrayValues("--files");
        CTEST_ASSERT(vals.size() == 2U);
        CTEST_ASSERT(args.isPresent("--verbose"));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Positional array stops at dash ==========

bool TestEzArgs_positional_array_stops_at_dash() {
    try {
        std::vector<char*> arr{"a", "b", "--verbose"};
        ez::Args args("Test");
        args.addPositional("items").arrayUnlimited();
        args.addOptional("--verbose");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        auto vals = args.getArrayValues("items");
        CTEST_ASSERT(vals.size() == 2U);
        CTEST_ASSERT(args.isPresent("--verbose"));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Command alias ==========

bool TestEzArgs_command_alias() {
    try {
        std::vector<char*> arr{"b"};
        ez::Args args("Test");
        args.addCommand("b/build");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isCommand("b"));
        CTEST_ASSERT(args.isCommand("build"));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== hasErrors / getErrors ==========

bool TestEzArgs_hasErrors_no_errors() {
    try {
        std::vector<char*> arr{};
        ez::Args args("Test");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(!args.hasErrors());
        CTEST_ASSERT(args.getErrors().empty());
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Optional array with eq delimiter ==========

bool TestEzArgs_optional_array_eq_delimiter() {
    try {
        std::vector<char*> arr{"--tag=foo"};
        ez::Args args("Test");
        args.addOptional("--tag").delimiter('=').arrayUnlimited();
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        auto vals = args.getArrayValues("--tag");
        CTEST_ASSERT(vals.size() == 1U);
        CTEST_ASSERT(vals[0] == "foo");
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== Space delimiter: no value if next arg is known ==========

bool TestEzArgs_space_delimiter_no_value_if_next_known() {
    try {
        std::vector<char*> arr{"--output", "--verbose"};
        ez::Args args("Test");
        args.addOptional("--output").delimiter(' ');
        args.addOptional("--verbose");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isPresent("--output"));
        CTEST_ASSERT(!args.hasValue("--output"));
        CTEST_ASSERT(args.isPresent("--verbose"));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// ========== -ff (multi-char short) should NOT be treated as combined short ==========

bool TestEzArgs_multi_char_short_option() {
    try {
        std::vector<char*> arr{"-ff"};
        ez::Args args("Test");
        args.addOptional("-ff/--fast-forward");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isPresent("-ff"));
        CTEST_ASSERT(args.isPresent("--fast-forward"));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// -f is combinable but -ff is not, both can coexist
bool TestEzArgs_short_and_multi_char_coexist() {
    try {
        std::vector<char*> arr{"-f", "-ff"};
        ez::Args args("Test");
        args.addOptional("-f/--force");
        args.addOptional("-ff/--fast-forward");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isPresent("-f"));
        CTEST_ASSERT(args.isPresent("-ff"));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// -abc still works when -a, -b, -c are all single-char options
bool TestEzArgs_combined_short_still_works() {
    try {
        std::vector<char*> arr{"-abc"};
        ez::Args args("Test");
        args.addOptional("-a/--alpha");
        args.addOptional("-b/--beta");
        args.addOptional("-c/--charlie");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isPresent("-a"));
        CTEST_ASSERT(args.isPresent("-b"));
        CTEST_ASSERT(args.isPresent("-c"));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// -ff should NOT be interpreted as -f -f combined when -f is defined but -ff is also defined
bool TestEzArgs_ff_not_combined_when_ff_defined() {
    try {
        std::vector<char*> arr{"-ff"};
        ez::Args args("Test");
        args.addOptional("-f/--force");          // -f is a single-char combinable
        args.addOptional("-ff/--fast-forward");  // -ff is a multi-char, not combinable
        // -ff should match the -ff option, not be treated as -f -f
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isPresent("-ff"));
        CTEST_ASSERT(args.isPresent("--fast-forward"));
        // -f should NOT be marked present by -ff
        CTEST_ASSERT(!args.isPresent("--force"));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// Custom prefix: #f, #b, #c can be combined as #fbc
bool TestEzArgs_custom_prefix_combined() {
    try {
        std::vector<char*> arr{"#fbc"};
        ez::Args args("Test", "#h/##help");
        args.addOptional("#f/##force");
        args.addOptional("#b/##build");
        args.addOptional("#c/##clean");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isPresent("##force"));
        CTEST_ASSERT(args.isPresent("##build"));
        CTEST_ASSERT(args.isPresent("##clean"));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// Custom prefix: #gg is not combinable (2 chars after #)
bool TestEzArgs_custom_prefix_multi_char_not_combined() {
    try {
        std::vector<char*> arr{"#gg"};
        ez::Args args("Test", "#h/##help");
        args.addOptional("#gg/##go-go");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isPresent("#gg"));
        CTEST_ASSERT(args.isPresent("##go-go"));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// Different prefixes should NOT be combined: -f and #b cannot form -f#b or anything
bool TestEzArgs_different_prefix_no_combine() {
    try {
        std::vector<char*> arr{"-f", "#b"};
        ez::Args args("Test", "#h/##help");
        args.addOptional("-f/--force");
        args.addOptional("#b/##build");
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isPresent("-f"));
        CTEST_ASSERT(args.isPresent("#b"));
    } catch (std::exception&) {
        return false;
    }
    return true;
}

// -ff when only -f is defined should fall through to unknown arg (not crash)
bool TestEzArgs_ff_when_only_f_defined() {
    try {
        std::vector<char*> arr{"-ff"};
        ez::Args args("Test");
        args.addOptional("-f/--force");
        // -ff: suffix is "ff", not all are combinable short args for prefix "-"
        // because m_isShortArg("-", 'f') is true for both chars => all_short is true
        // so -ff IS treated as combined -f -f (which is valid, just sets -f present twice)
        CTEST_ASSERT(args.parse(static_cast<int32_t>(arr.size()), arr.data(), 0U));
        CTEST_ASSERT(args.isPresent("-f"));
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
    else IfTestExist(TestEzArgs_unknown_option);
    else IfTestExist(TestEzArgs_default_constructor);
    else IfTestExist(TestEzArgs_empty_name);
    else IfTestExist(TestEzArgs_custom_help_key);
    else IfTestExist(TestEzArgs_help_short);
    else IfTestExist(TestEzArgs_help_long);
    else IfTestExist(TestEzArgs_header_footer_description);
    else IfTestExist(TestEzArgs_optional_alias);
    else IfTestExist(TestEzArgs_optional_alias_long_form);
    else IfTestExist(TestEzArgs_optional_value_eq_delimiter);
    else IfTestExist(TestEzArgs_optional_value_space_delimiter);
    else IfTestExist(TestEzArgs_optional_default_value);
    else IfTestExist(TestEzArgs_required_optional_missing);
    else IfTestExist(TestEzArgs_required_optional_present);
    else IfTestExist(TestEzArgs_positional_single);
    else IfTestExist(TestEzArgs_positional_multiple);
    else IfTestExist(TestEzArgs_positional_missing);
    else IfTestExist(TestEzArgs_positional_array_fixed);
    else IfTestExist(TestEzArgs_positional_array_range);
    else IfTestExist(TestEzArgs_positional_array_too_few);
    else IfTestExist(TestEzArgs_positional_array_unlimited);
    else IfTestExist(TestEzArgs_optional_array_space);
    else IfTestExist(TestEzArgs_optional_array_unlimited);
    else IfTestExist(TestEzArgs_getValue_int);
    else IfTestExist(TestEzArgs_getValue_float);
    else IfTestExist(TestEzArgs_getValue_double);
    else IfTestExist(TestEzArgs_getValue_bool_true);
    else IfTestExist(TestEzArgs_getValue_bool_one);
    else IfTestExist(TestEzArgs_getValue_bool_false);
    else IfTestExist(TestEzArgs_getValue_unknown_key);
    else IfTestExist(TestEzArgs_getArrayValues_typed);
    else IfTestExist(TestEzArgs_combined_short_args);
    else IfTestExist(TestEzArgs_combined_short_args_unknown);
    else IfTestExist(TestEzArgs_command_basic);
    else IfTestExist(TestEzArgs_command_not_active);
    else IfTestExist(TestEzArgs_command_with_sub_positional);
    else IfTestExist(TestEzArgs_command_with_sub_optional);
    else IfTestExist(TestEzArgs_command_missing_required_sub_positional);
    else IfTestExist(TestEzArgs_command_missing_required_sub_optional);
    else IfTestExist(TestEzArgs_command_with_sub_optional_value);
    else IfTestExist(TestEzArgs_mixed_positional_and_optional);
    else IfTestExist(TestEzArgs_positional_then_optional);
    else IfTestExist(TestEzArgs_global_optional_with_command);
    else IfTestExist(TestEzArgs_isPresent_false);
    else IfTestExist(TestEzArgs_isArray_false);
    else IfTestExist(TestEzArgs_getArrayValues_non_array);
    else IfTestExist(TestEzArgs_multiple_errors);
    else IfTestExist(TestEzArgs_empty_args_no_required);
    else IfTestExist(TestEzArgs_parse_startIdx);
    else IfTestExist(TestEzArgs_optional_type_annotation);
    else IfTestExist(TestEzArgs_positional_type_annotation);
    else IfTestExist(TestEzArgs_positional_dash_error);
    else IfTestExist(TestEzArgs_empty_optional_key);
    else IfTestExist(TestEzArgs_empty_positional_key);
    else IfTestExist(TestEzArgs_empty_command_key);
    else IfTestExist(TestEzArgs_printHelp_output);
    else IfTestExist(TestEzArgs_chaining);
    else IfTestExist(TestEzArgs_optional_array_stops_at_known_arg);
    else IfTestExist(TestEzArgs_positional_array_stops_at_dash);
    else IfTestExist(TestEzArgs_command_alias);
    else IfTestExist(TestEzArgs_hasErrors_no_errors);
    else IfTestExist(TestEzArgs_optional_array_eq_delimiter);
    else IfTestExist(TestEzArgs_space_delimiter_no_value_if_next_known);
    else IfTestExist(TestEzArgs_multi_char_short_option);
    else IfTestExist(TestEzArgs_short_and_multi_char_coexist);
    else IfTestExist(TestEzArgs_combined_short_still_works);
    else IfTestExist(TestEzArgs_ff_not_combined_when_ff_defined);
    else IfTestExist(TestEzArgs_custom_prefix_combined);
    else IfTestExist(TestEzArgs_custom_prefix_multi_char_not_combined);
    else IfTestExist(TestEzArgs_different_prefix_no_combine);
    else IfTestExist(TestEzArgs_ff_when_only_f_defined);
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
