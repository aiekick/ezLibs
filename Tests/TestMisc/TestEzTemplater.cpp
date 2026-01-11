#include <ezlibs/ezStr.hpp>
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
    const auto result = tpl.saveToString();
    CTEST_ASSERT(result == "Hello World");
    return true;
}

bool TestEzTemplater_MissingTagIsIgnored() {
    ez::Templater tpl;
    CTEST_ASSERT(tpl.loadFromString("Hello [[TOTO]]"));
    const auto result = tpl.saveToString();
    CTEST_ASSERT(result == "Hello ");
    return true;
}

bool TestEzTemplater_DisabledTag() {
    ez::Templater tpl;
    CTEST_ASSERT(tpl.loadFromString("Hello [[TOTO]]"));
    tpl.useTag("TOTO", "World");
    auto result = tpl.saveToString();
    CTEST_ASSERT(result == "Hello World");
    tpl.unuseTag("TOTO");
    result = tpl.saveToString();
    CTEST_ASSERT(result == "Hello ");
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
    const auto result = tpl.saveToString();
    CTEST_ASSERT(result == "\nif (X) {\n    doSomething();\n}\n");
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
    const auto result = tpl.saveToString();
    CTEST_ASSERT(result == "\n");
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
    const auto result = tpl.saveToString();
    CTEST_ASSERT(result == "\nA1\nB1\nA2\n");
    return true;
}

bool TestEzTemplater_IndentPreservedForMultilineValue() {
    ez::Templater tpl;
    CTEST_ASSERT(tpl.loadFromString(R"(
    [[TOTO]]
)"));
    tpl.useTag("TOTO", "line1\nline2");
    const auto result = tpl.saveToString();
    CTEST_ASSERT(result == "\n    line1\n    line2\n");
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

bool TestEzTemplater_GetUsedTagInfos_Basic() {
    ez::Templater tpl;
    tpl.useTag("TOTO", "value1");
    tpl.useTag("TATA", "value2");
    auto infos = tpl.getUsedTagInfos(">>", false);
    CTEST_ASSERT(infos == ">> [[TATA]] => 'value2'\n>> [[TOTO]] => 'value1'");
    infos = tpl.getUsedTagInfos(">>", true);
    CTEST_ASSERT(infos == ">> [[TATA]] => 'value2'\n>> [[TOTO]] => 'value1'");
    return true;
}

bool TestEzTemplater_GetUsedTagInfos_MultilineValue() {
    ez::Templater tpl;
    tpl.useTag("MULTI", "line1\nline2");
    auto infos = tpl.getUsedTagInfos("--", false);
    CTEST_ASSERT(infos == "-- [[MULTI]] => '\nline1\nline2'");
    infos = tpl.getUsedTagInfos("--", true);
    CTEST_ASSERT(infos == "-- [[MULTI]] => 'line1\\nline2'");
    return true;
}

bool TestEzTemplater_LoadFromFile() {
    const auto filePath = "TestEzTemplater_LoadFromFile.txt";
    std::ofstream out(filePath);
    out << "Value = [[X]]";
    out.close();
    ez::Templater tpl;
    CTEST_ASSERT(tpl.loadFromFile(filePath));
    tpl.useTag("X", "42");
    CTEST_ASSERT(tpl.saveToString() == "Value = 42");
    return true;
}

bool TestEzTemplater_SaveToFile() {
    ez::Templater tpl;
    tpl.loadFromString("Hello [[NAME]]");
    tpl.useTag("NAME", "World");
    const auto filePath = "TestEzTemplater_SaveToFile.txt";
    CTEST_ASSERT(tpl.saveToFile(filePath));
    std::ifstream in(filePath);
    CTEST_ASSERT(in.good());
    std::string content;
    std::getline(in, content);
    CTEST_ASSERT(content == "Hello World");
    return true;
}

bool TestEzTemplater_Exception_CloseBeforeOpen() {
    ez::Templater tpl;
    CTEST_ASSERT(tpl.loadFromString("abc ]] def"));
    const auto result = tpl.saveToString();
    CTEST_ASSERT(result.empty());
    return true;
}

bool TestEzTemplater_Exception_MissingCloseSimpleTag() {
    ez::Templater tpl;
    CTEST_ASSERT(tpl.loadFromString("abc [[TOTO"));
    const auto result = tpl.saveToString();
    CTEST_ASSERT(result.empty());
    return true;
}

bool TestEzTemplater_Exception_UnclosedMultilineBlock() {
    ez::Templater tpl;
    CTEST_ASSERT(tpl.loadFromString(R"(
[[TOTO
line1
line2
)"));
    const auto result = tpl.saveToString();
    CTEST_ASSERT(result.empty());
    return true;
}

bool TestEzTemplater_Exception_UnclosedNestedBlock() {
    ez::Templater tpl;
    CTEST_ASSERT(tpl.loadFromString(R"(
[[A
text
[[B
nested
]]
)"));
    const auto result = tpl.saveToString();
    CTEST_ASSERT(result.empty());
    return true;
}

bool TestEzTemplater_Exception_StrayClosingTag() {
    ez::Templater tpl;
    CTEST_ASSERT(tpl.loadFromString("ok\n]]\nok"));
    const auto result = tpl.saveToString();
    CTEST_ASSERT(result.empty());
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
    else IfTestExist(TestEzTemplater_GetUsedTagInfos_Basic);
    else IfTestExist(TestEzTemplater_GetUsedTagInfos_MultilineValue);
    else IfTestExist(TestEzTemplater_LoadFromFile);
    else IfTestExist(TestEzTemplater_SaveToFile);
    else IfTestExist(TestEzTemplater_Exception_CloseBeforeOpen);
    else IfTestExist(TestEzTemplater_Exception_MissingCloseSimpleTag);
    else IfTestExist(TestEzTemplater_Exception_UnclosedMultilineBlock);
    else IfTestExist(TestEzTemplater_Exception_UnclosedNestedBlock);
    else IfTestExist(TestEzTemplater_Exception_StrayClosingTag);
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
