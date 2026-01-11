#include <ezlibs/ezTemplater.hpp>
#include <ezlibs/ezCTest.hpp>
#include <string>

// Désactivation des warnings de conversion
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

bool TestEzTemplater_SimpleTag() {
    ez::Templater tpl;
    CTEST_ASSERT(tpl.loadFromString("Hello [[TOTO]]"));
    tpl.useTag("TOTO", "World");
    CTEST_ASSERT(tpl.saveToString() == "Hello World");
    return true;
}

bool TestEzTemplater_MissingTagIsIgnored() {
    ez::Templater tpl;
    CTEST_ASSERT(tpl.loadFromString("Hello [[TOTO]]"));
    CTEST_ASSERT(tpl.saveToString() == "Hello ");
    return true;
}

bool TestEzTemplater_DisabledTag() {
    ez::Templater tpl;
    CTEST_ASSERT(tpl.loadFromString("Hello [[TOTO]]"));
    tpl.useTag("TOTO", "World");
    tpl.unuseTag("TOTO");
    CTEST_ASSERT(tpl.saveToString() == "Hello ");
    return true;
}

bool TestEzTemplater_MultiLineBlockEnabled() {
    ez::Templater tpl;
    CTEST_ASSERT(tpl.loadFromString(R"(
[[COND
if (X) {
    doSomething();
}
]]
)"));
    tpl.useTag("COND");
    const std::string result = tpl.saveToString();
    CTEST_ASSERT(result.find("if (X)") != std::string::npos);
    CTEST_ASSERT(result.find("doSomething();") != std::string::npos);
    return true;
}

bool TestEzTemplater_MultiLineBlockDisabled() {
    ez::Templater tpl;
    CTEST_ASSERT(tpl.loadFromString(R"(
[[COND
if (X) {
    doSomething();
}
]]
)"));
    tpl.unuseTag("COND");
    CTEST_ASSERT(tpl.saveToString().find("doSomething") == std::string::npos);
    return true;
}

bool TestEzTemplater_NestedBlocks() {
    ez::Templater tpl;
    CTEST_ASSERT(tpl.loadFromString(R"(
[[A
A1
[[B
B1
]]
A2
]]
)"));
    tpl.useTag("A").useTag("B");
    const std::string result = tpl.saveToString();
    CTEST_ASSERT(result.find("A1") != std::string::npos);
    CTEST_ASSERT(result.find("B1") != std::string::npos);
    CTEST_ASSERT(result.find("A2") != std::string::npos);
    return true;
}

bool TestEzTemplater_IndentPreservedForMultilineValue() {
    ez::Templater tpl;
    CTEST_ASSERT(tpl.loadFromString(R"(
    [[TOTO]]
)"));
    tpl.useTag("TOTO", "line1\nline2");
    const std::string result = tpl.saveToString();
    CTEST_ASSERT(result.find("    line1") != std::string::npos);
    CTEST_ASSERT(result.find("    line2") != std::string::npos);
    return true;
}

bool TestEzTemplater_SyntaxError_UnmatchedClose() {
    ez::Templater tpl;
    CTEST_ASSERT(tpl.loadFromString("]]"));
    CTEST_ASSERT(tpl.saveToString().empty());
    return true;
}

bool TestEzTemplater_SyntaxError_UnclosedBlock() {
    ez::Templater tpl;
    CTEST_ASSERT(tpl.loadFromString("[[TOTO"));
    CTEST_ASSERT(tpl.saveToString().empty());
    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzTemplater(const std::string& vTest) {
    IfTestExist(TestEzTemplater_SimpleTag);
    else IfTestExist(TestEzTemplater_MissingTagIsIgnored);
    else IfTestExist(TestEzTemplater_DisabledTag);
    else IfTestExist(TestEzTemplater_MultiLineBlockEnabled);
    else IfTestExist(TestEzTemplater_MultiLineBlockDisabled);
    else IfTestExist(TestEzTemplater_NestedBlocks);
    else IfTestExist(TestEzTemplater_IndentPreservedForMultilineValue);
    else IfTestExist(TestEzTemplater_SyntaxError_UnmatchedClose);
    else IfTestExist(TestEzTemplater_SyntaxError_UnclosedBlock);
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
