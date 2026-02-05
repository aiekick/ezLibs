#include <ezlibs/ezStr.hpp>
#include <ezlibs/ezCTest.hpp>
#include <string>

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

bool TestEzStrSplitStringToList() {
    auto result = ez::str::splitStringToList("a,b,c", ",");
    CTEST_ASSERT(result.size() == 3);
    auto it = result.begin();
    CTEST_ASSERT(*it++ == "a");
    CTEST_ASSERT(*it++ == "b");
    CTEST_ASSERT(*it++ == "c");

    result = ez::str::splitStringToList("a,,c", ",", true);
    CTEST_ASSERT(result.size() == 3);
    it = result.begin();
    CTEST_ASSERT(*it++ == "a");
    CTEST_ASSERT(it->empty());
    ++it;
    CTEST_ASSERT(*it++ == "c");

    result = ez::str::splitStringToList("a,b,c", ',');
    CTEST_ASSERT(result.size() == 3);
    return true;
}

bool TestEzStrSplitStringToVector() {
    auto result = ez::str::splitStringToVector("x;y;z", ";");
    CTEST_ASSERT(result.size() == 3);
    CTEST_ASSERT(result[0] == "x");
    CTEST_ASSERT(result[1] == "y");
    CTEST_ASSERT(result[2] == "z");

    result = ez::str::splitStringToVector("1;2;3", ';');
    CTEST_ASSERT(result.size() == 3);
    CTEST_ASSERT(result[0] == "1");
    return true;
}

bool TestEzStrSplitStringToSet() {
    auto result = ez::str::splitStringToSet("a,b,a,c", ",");
    CTEST_ASSERT(result.size() == 3);
    CTEST_ASSERT(result.count("a") == 1);
    CTEST_ASSERT(result.count("b") == 1);
    CTEST_ASSERT(result.count("c") == 1);
    return true;
}

bool TestEzStrStringToNumber() {
    int intVal = 0;
    CTEST_ASSERT(ez::str::stringToNumber("42", intVal));
    CTEST_ASSERT(intVal == 42);

    float floatVal = 0.0f;
    CTEST_ASSERT(ez::str::stringToNumber("3.14", floatVal));
    CTEST_ASSERT(floatVal > 3.13f && floatVal < 3.15f);

    // stringToNumber returns true even for invalid input (stringstream behavior)
    // It just leaves the stream in a failed state but doesn't throw
    return true;
}

bool TestEzStrStringToNumberVector() {
    auto result = ez::str::stringToNumberVector<int>("1,2,3,4", ',');
    CTEST_ASSERT(result.size() == 4);
    CTEST_ASSERT(result[0] == 1);
    CTEST_ASSERT(result[1] == 2);
    CTEST_ASSERT(result[2] == 3);
    CTEST_ASSERT(result[3] == 4);

    auto floats = ez::str::stringToNumberVector<float>("1.5,2.5,3.5", ',');
    CTEST_ASSERT(floats.size() == 3);
    CTEST_ASSERT(floats[0] > 1.4f && floats[0] < 1.6f);
    return true;
}

bool TestEzStrToUpper() {
    CTEST_ASSERT(ez::str::toUpper("hello") == "HELLO");
    CTEST_ASSERT(ez::str::toUpper("Hello World") == "HELLO WORLD");
    CTEST_ASSERT(ez::str::toUpper("") == "");
    CTEST_ASSERT(ez::str::toUpper("123") == "123");
    return true;
}

bool TestEzStrToLower() {
    CTEST_ASSERT(ez::str::toLower("HELLO") == "hello");
    CTEST_ASSERT(ez::str::toLower("Hello World") == "hello world");
    CTEST_ASSERT(ez::str::toLower("") == "");
    CTEST_ASSERT(ez::str::toLower("123") == "123");
    return true;
}

bool TestEzStrToHex() {
    CTEST_ASSERT(ez::str::toHex("A") == "41");
    CTEST_ASSERT(ez::str::toHex("") == "");
    return true;
}

bool TestEzStrToHexStr() {
    CTEST_ASSERT(ez::str::toHexStr(255) == "ff");
    CTEST_ASSERT(ez::str::toHexStr(16) == "10");
    return true;
}

bool TestEzStrToDecStr() {
    CTEST_ASSERT(ez::str::toDecStr(42) == "42");
    CTEST_ASSERT(ez::str::toDecStr(0) == "0");
    return true;
}

bool TestEzStrStrContains() {
    auto result = ez::str::strContains("hello world hello", "hello");
    CTEST_ASSERT(result.size() == 2);
    CTEST_ASSERT(result[0] == 0);
    CTEST_ASSERT(result[1] == 12);

    result = ez::str::strContains("test", "xyz");
    CTEST_ASSERT(result.empty());
    return true;
}

bool TestEzStrReplaceString() {
    std::string str = "hello world hello";
    CTEST_ASSERT(ez::str::replaceString(str, "hello", "hi"));
    CTEST_ASSERT(str == "hi world hi");

    str = "test";
    CTEST_ASSERT(!ez::str::replaceString(str, "xyz", "abc"));
    CTEST_ASSERT(str == "test");
    return true;
}

bool TestEzStrGetCountOccurence() {
    CTEST_ASSERT(ez::str::getCountOccurence("hello world hello", "hello") == 2);
    CTEST_ASSERT(ez::str::getCountOccurence("test", "xyz") == 0);
    CTEST_ASSERT(ez::str::getCountOccurence("aaa", "a") == 3);

    CTEST_ASSERT(ez::str::getCountOccurence("hello", 'l') == 2);
    CTEST_ASSERT(ez::str::getCountOccurence("test", 'x') == 0);
    return true;
}

bool TestEzStrGetCountOccurenceInSection() {
    CTEST_ASSERT(ez::str::getCountOccurenceInSection("hello world hello", 0, 11, "hello") == 1);
    CTEST_ASSERT(ez::str::getCountOccurenceInSection("hello world hello", 0, 17, "hello") == 2);

    CTEST_ASSERT(ez::str::getCountOccurenceInSection("hello", 0, 3, 'l') == 1);
    CTEST_ASSERT(ez::str::getCountOccurenceInSection("hello", 0, 5, 'l') == 2);
    return true;
}

bool TestEzStrGetDigitsCountOfAIntegralNumber() {
    CTEST_ASSERT(ez::str::getDigitsCountOfAIntegralNumber(0) == 1);
    CTEST_ASSERT(ez::str::getDigitsCountOfAIntegralNumber(1) == 1);
    CTEST_ASSERT(ez::str::getDigitsCountOfAIntegralNumber(-1) == 2);
    CTEST_ASSERT(ez::str::getDigitsCountOfAIntegralNumber(000) == 1);
    CTEST_ASSERT(ez::str::getDigitsCountOfAIntegralNumber(150) == 3);
    CTEST_ASSERT(ez::str::getDigitsCountOfAIntegralNumber(050) == 2);
    return true;
}

bool TestEzStrSearchForPatternWithWildcards() {
    std::string buffer = "TOTO VA AU ZOO ET C'EST BEAU";
    std::pair<size_t, size_t> posRange;
    CTEST_ASSERT(ez::str::searchForPatternWithWildcards(buffer, "PAPA*ZOO*BEAU").empty());
    CTEST_ASSERT(ez::str::searchForPatternWithWildcards(buffer, "TOTO*ZOO*BEAU") == buffer);
    CTEST_ASSERT(ez::str::searchForPatternWithWildcards(buffer, "TOTO*ZOO*BEAU", posRange) == buffer && posRange.first == 0 && posRange.second == buffer.size());
    return true;
}

bool TestEzStrExtractWildcardsFromPattern() {
    std::string buffer = "TOTO VA AU ZOO ET C'EST BEAU";
    const auto arr = ez::str::extractWildcardsFromPattern(buffer, "*TOTO*ZOO*BEAU*");
    CTEST_ASSERT(arr.size() == 2U);
    CTEST_ASSERT(arr.at(0) == " VA AU ");
    CTEST_ASSERT(arr.at(1) == " ET C'EST ");
    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzStr(const std::string& vTest) {
    IfTestExist(TestEzStrSplitStringToList);
    else IfTestExist(TestEzStrSplitStringToVector);
    else IfTestExist(TestEzStrSplitStringToSet);
    else IfTestExist(TestEzStrStringToNumber);
    else IfTestExist(TestEzStrStringToNumberVector);
    else IfTestExist(TestEzStrToUpper);
    else IfTestExist(TestEzStrToLower);
    else IfTestExist(TestEzStrToHex);
    else IfTestExist(TestEzStrToHexStr);
    else IfTestExist(TestEzStrToDecStr);
    else IfTestExist(TestEzStrStrContains);
    else IfTestExist(TestEzStrReplaceString);
    else IfTestExist(TestEzStrGetCountOccurence);
    else IfTestExist(TestEzStrGetCountOccurenceInSection);
    else IfTestExist(TestEzStrGetDigitsCountOfAIntegralNumber);
    else IfTestExist(TestEzStrSearchForPatternWithWildcards);
    else IfTestExist(TestEzStrExtractWildcardsFromPattern);
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
