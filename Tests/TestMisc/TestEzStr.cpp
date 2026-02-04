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

bool TestEzStr_SplitStringToList() {
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

bool TestEzStr_SplitStringToVector() {
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

bool TestEzStr_SplitStringToSet() {
    auto result = ez::str::splitStringToSet("a,b,a,c", ",");
    CTEST_ASSERT(result.size() == 3);
    CTEST_ASSERT(result.count("a") == 1);
    CTEST_ASSERT(result.count("b") == 1);
    CTEST_ASSERT(result.count("c") == 1);
    return true;
}

bool TestEzStr_StringToNumber() {
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

bool TestEzStr_StringToNumberVector() {
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

bool TestEzStr_ToUpper() {
    CTEST_ASSERT(ez::str::toUpper("hello") == "HELLO");
    CTEST_ASSERT(ez::str::toUpper("Hello World") == "HELLO WORLD");
    CTEST_ASSERT(ez::str::toUpper("") == "");
    CTEST_ASSERT(ez::str::toUpper("123") == "123");
    return true;
}

bool TestEzStr_ToLower() {
    CTEST_ASSERT(ez::str::toLower("HELLO") == "hello");
    CTEST_ASSERT(ez::str::toLower("Hello World") == "hello world");
    CTEST_ASSERT(ez::str::toLower("") == "");
    CTEST_ASSERT(ez::str::toLower("123") == "123");
    return true;
}

bool TestEzStr_ToHex() {
    CTEST_ASSERT(ez::str::toHex("A") == "41");
    CTEST_ASSERT(ez::str::toHex("") == "");
    return true;
}

bool TestEzStr_ToHexStr() {
    CTEST_ASSERT(ez::str::toHexStr(255) == "ff");
    CTEST_ASSERT(ez::str::toHexStr(16) == "10");
    return true;
}

bool TestEzStr_ToDecStr() {
    CTEST_ASSERT(ez::str::toDecStr(42) == "42");
    CTEST_ASSERT(ez::str::toDecStr(0) == "0");
    return true;
}

bool TestEzStr_StrContains() {
    auto result = ez::str::strContains("hello world hello", "hello");
    CTEST_ASSERT(result.size() == 2);
    CTEST_ASSERT(result[0] == 0);
    CTEST_ASSERT(result[1] == 12);

    result = ez::str::strContains("test", "xyz");
    CTEST_ASSERT(result.empty());
    return true;
}

bool TestEzStr_ReplaceString() {
    std::string str = "hello world hello";
    CTEST_ASSERT(ez::str::replaceString(str, "hello", "hi"));
    CTEST_ASSERT(str == "hi world hi");

    str = "test";
    CTEST_ASSERT(!ez::str::replaceString(str, "xyz", "abc"));
    CTEST_ASSERT(str == "test");
    return true;
}

bool TestEzStr_GetCountOccurence() {
    CTEST_ASSERT(ez::str::getCountOccurence("hello world hello", "hello") == 2);
    CTEST_ASSERT(ez::str::getCountOccurence("test", "xyz") == 0);
    CTEST_ASSERT(ez::str::getCountOccurence("aaa", "a") == 3);

    CTEST_ASSERT(ez::str::getCountOccurence("hello", 'l') == 2);
    CTEST_ASSERT(ez::str::getCountOccurence("test", 'x') == 0);
    return true;
}

bool TestEzStr_GetCountOccurenceInSection() {
    CTEST_ASSERT(ez::str::getCountOccurenceInSection("hello world hello", 0, 11, "hello") == 1);
    CTEST_ASSERT(ez::str::getCountOccurenceInSection("hello world hello", 0, 17, "hello") == 2);

    CTEST_ASSERT(ez::str::getCountOccurenceInSection("hello", 0, 3, 'l') == 1);
    CTEST_ASSERT(ez::str::getCountOccurenceInSection("hello", 0, 5, 'l') == 2);
    return true;
}

bool TestEzStr_GetDigitsCountOfAIntegralNumber() {
    CTEST_ASSERT(ez::str::getDigitsCountOfAIntegralNumber(0) == 1);
    CTEST_ASSERT(ez::str::getDigitsCountOfAIntegralNumber(1) == 1);
    CTEST_ASSERT(ez::str::getDigitsCountOfAIntegralNumber(-1) == 2);
    CTEST_ASSERT(ez::str::getDigitsCountOfAIntegralNumber(000) == 1);
    CTEST_ASSERT(ez::str::getDigitsCountOfAIntegralNumber(150) == 3);
    CTEST_ASSERT(ez::str::getDigitsCountOfAIntegralNumber(050) == 2);
    return true;
}

bool TestEzStr_SearchForPatternWithWildcards() {
    std::string buffer = "TOTO VA AU ZOO ET C'EST BEAU";
    std::pair<size_t, size_t> posRange;
    CTEST_ASSERT(ez::str::searchForPatternWithWildcards(buffer, "PAPA*ZOO*BEAU").empty());
    CTEST_ASSERT(ez::str::searchForPatternWithWildcards(buffer, "TOTO*ZOO*BEAU") == buffer);
    CTEST_ASSERT(ez::str::searchForPatternWithWildcards(buffer, "TOTO*ZOO*BEAU", posRange) == buffer && posRange.first == 0 && posRange.second == buffer.size());
    return true;
}

bool TestEzStr_ExtractWildcardsFromPattern() {
    std::string buffer = "TOTO VA AU ZOO ET C'EST BEAU";
    const auto arr = ez::str::extractWildcardsFromPattern(buffer, "*TOTO*ZOO*BEAU*");
    CTEST_ASSERT(arr.size() == 2U);
    CTEST_ASSERT(arr.at(0) == " VA AU ");
    CTEST_ASSERT(arr.at(1) == " ET C'EST ");
    return true;
}

bool TestEzStr_ToStr() {
    std::string result = ez::str::toStr("Hello %s", "World");
    CTEST_ASSERT(result == "Hello World");

    result = ez::str::toStr("Number: %d", 42);
    CTEST_ASSERT(result == "Number: 42");

    result = ez::str::toStr("Float: %.2f", 3.14159);
    CTEST_ASSERT(result == "Float: 3.14");

    return true;
}

bool TestEzStr_ToStrFromArray() {
    int arr[] = {1, 2, 3, 4};
    std::string result = ez::str::toStrFromArray(arr, 4);
    CTEST_ASSERT(result == "1;2;3;4");

    result = ez::str::toStrFromArray(arr, 4, ',');
    CTEST_ASSERT(result == "1,2,3,4");

    float farr[] = {1.5f, 2.5f, 3.5f};
    result = ez::str::toStrFromArray(farr, 3);
    CTEST_ASSERT(!result.empty());

    return true;
}

bool TestEzStr_ToStrTemplate() {
    std::string result = ez::str::toStr(42);
    CTEST_ASSERT(result == "42");

    result = ez::str::toStr(3.14);
    CTEST_ASSERT(!result.empty());

    result = ez::str::toStr(true);
    CTEST_ASSERT(result == "1");

    return true;
}

bool TestEzStr_SplitStringToListInversion() {
    auto result = ez::str::splitStringToList("a,b,c", ",", false, true);
    CTEST_ASSERT(result.size() == 3);
    auto it = result.begin();
    CTEST_ASSERT(*it++ == "c");
    CTEST_ASSERT(*it++ == "b");
    CTEST_ASSERT(*it++ == "a");

    return true;
}

bool TestEzStr_SplitStringToListEmpty() {
    auto result = ez::str::splitStringToList("", ",");
    CTEST_ASSERT(result.empty());

    result = ez::str::splitStringToList("single", ",");
    CTEST_ASSERT(result.size() == 1);
    CTEST_ASSERT(result.front() == "single");

    return true;
}

bool TestEzStr_SplitStringToVectorEmpty() {
    auto result = ez::str::splitStringToVector("", ";");
    CTEST_ASSERT(result.empty());

    result = ez::str::splitStringToVector("a;;b", ";", true);
    CTEST_ASSERT(result.size() == 3);
    CTEST_ASSERT(result[0] == "a");
    CTEST_ASSERT(result[1].empty());
    CTEST_ASSERT(result[2] == "b");

    return true;
}

bool TestEzStr_SplitStringToSetEmpty() {
    auto result = ez::str::splitStringToSet("", ",");
    CTEST_ASSERT(result.empty());

    result = ez::str::splitStringToSet("a,b,a,b,c", ",");
    CTEST_ASSERT(result.size() == 3);

    return true;
}

bool TestEzStr_StringToNumberEdgeCases() {
    int intVal = 0;
    CTEST_ASSERT(ez::str::stringToNumber("0", intVal));
    CTEST_ASSERT(intVal == 0);

    CTEST_ASSERT(ez::str::stringToNumber("-42", intVal));
    CTEST_ASSERT(intVal == -42);

    double doubleVal = 0.0;
    CTEST_ASSERT(ez::str::stringToNumber("1.23456", doubleVal));
    CTEST_ASSERT(doubleVal > 1.23 && doubleVal < 1.24);

    return true;
}

bool TestEzStr_StringToNumberVectorEmpty() {
    auto result = ez::str::stringToNumberVector<int>("", ',');
    CTEST_ASSERT(result.empty());

    result = ez::str::stringToNumberVector<int>("42", ',');
    CTEST_ASSERT(result.size() == 1);
    CTEST_ASSERT(result[0] == 42);

    return true;
}

bool TestEzStr_ToHexEdgeCases() {
    CTEST_ASSERT(ez::str::toHex("ABC") == "414243");
    CTEST_ASSERT(ez::str::toHex(" ") == "20");

    return true;
}

bool TestEzStr_ToHexStrEdgeCases() {
    CTEST_ASSERT(ez::str::toHexStr(0) == "0");
    CTEST_ASSERT(ez::str::toHexStr(15) == "f");
    CTEST_ASSERT(ez::str::toHexStr(256) == "100");

    return true;
}

bool TestEzStr_ToDecStrEdgeCases() {
    CTEST_ASSERT(ez::str::toDecStr(-1) == "-1");
    CTEST_ASSERT(ez::str::toDecStr(999) == "999");

    return true;
}

bool TestEzStr_StrContainsEdgeCases() {
    auto result = ez::str::strContains("", "test");
    CTEST_ASSERT(result.empty());

    result = ez::str::strContains("test", "");
    CTEST_ASSERT(result.empty());

    result = ez::str::strContains("aaa", "aa");
    CTEST_ASSERT(result.size() == 2);
    CTEST_ASSERT(result[0] == 0);
    CTEST_ASSERT(result[1] == 1);

    return true;
}

bool TestEzStr_ReplaceStringEdgeCases() {
    std::string str = "";
    CTEST_ASSERT(!ez::str::replaceString(str, "hello", "hi"));
    CTEST_ASSERT(str.empty());

    str = "aaa";
    CTEST_ASSERT(ez::str::replaceString(str, "a", "b"));
    CTEST_ASSERT(str == "bbb");

    str = "test";
    CTEST_ASSERT(ez::str::replaceString(str, "", "x") == false);

    return true;
}

bool TestEzStr_GetCountOccurenceEdgeCases() {
    CTEST_ASSERT(ez::str::getCountOccurence("", "test") == 0);
    CTEST_ASSERT(ez::str::getCountOccurence("test", "") == 0);
    CTEST_ASSERT(ez::str::getCountOccurence("", 'x') == 0);

    return true;
}

bool TestEzStr_GetCountOccurenceInSectionEdgeCases() {
    CTEST_ASSERT(ez::str::getCountOccurenceInSection("hello", 10, 20, "l") == 0);
    CTEST_ASSERT(ez::str::getCountOccurenceInSection("hello", 0, 0, "l") == 0);
    CTEST_ASSERT(ez::str::getCountOccurenceInSection("hello", 2, 4, 'l') == 2);

    return true;
}

bool TestEzStr_GetDigitsCountOfAIntegralNumberEdgeCases() {
    CTEST_ASSERT(ez::str::getDigitsCountOfAIntegralNumber(-999) == 4);
    CTEST_ASSERT(ez::str::getDigitsCountOfAIntegralNumber(999) == 3);
    CTEST_ASSERT(ez::str::getDigitsCountOfAIntegralNumber(-10) == 3);

    return true;
}

bool TestEzStr_SearchForPatternWithWildcardsEdgeCases() {
    std::string buffer = "TEST";
    CTEST_ASSERT(ez::str::searchForPatternWithWildcards(buffer, "TEST") == "TEST");
    CTEST_ASSERT(ez::str::searchForPatternWithWildcards(buffer, "T*T") == "TEST");
    CTEST_ASSERT(ez::str::searchForPatternWithWildcards(buffer, "*E*") == "TEST");
    CTEST_ASSERT(ez::str::searchForPatternWithWildcards("", "TEST").empty());

    return true;
}

bool TestEzStr_ExtractWildcardsFromPatternEdgeCases() {
    const auto arr = ez::str::extractWildcardsFromPattern("ABC", "*A*B*C*");
    CTEST_ASSERT(arr.size() == 2U);
    CTEST_ASSERT(arr.at(0).empty());
    CTEST_ASSERT(arr.at(1).empty());

    const auto arr2 = ez::str::extractWildcardsFromPattern("", "*test*");
    CTEST_ASSERT(arr2.empty());

    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzStr(const std::string& vTest) {
    IfTestExist(TestEzStr_SplitStringToList);
    else IfTestExist(TestEzStr_SplitStringToVector);
    else IfTestExist(TestEzStr_SplitStringToSet);
    else IfTestExist(TestEzStr_StringToNumber);
    else IfTestExist(TestEzStr_StringToNumberVector);
    else IfTestExist(TestEzStr_ToUpper);
    else IfTestExist(TestEzStr_ToLower);
    else IfTestExist(TestEzStr_ToHex);
    else IfTestExist(TestEzStr_ToHexStr);
    else IfTestExist(TestEzStr_ToDecStr);
    else IfTestExist(TestEzStr_StrContains);
    else IfTestExist(TestEzStr_ReplaceString);
    else IfTestExist(TestEzStr_GetCountOccurence);
    else IfTestExist(TestEzStr_GetCountOccurenceInSection);
    else IfTestExist(TestEzStr_GetDigitsCountOfAIntegralNumber);
    else IfTestExist(TestEzStr_SearchForPatternWithWildcards);
    else IfTestExist(TestEzStr_ExtractWildcardsFromPattern);
    else IfTestExist(TestEzStr_ToStr);
    else IfTestExist(TestEzStr_ToStrFromArray);
    else IfTestExist(TestEzStr_ToStrTemplate);
    else IfTestExist(TestEzStr_SplitStringToListInversion);
    else IfTestExist(TestEzStr_SplitStringToListEmpty);
    else IfTestExist(TestEzStr_SplitStringToVectorEmpty);
    else IfTestExist(TestEzStr_SplitStringToSetEmpty);
    else IfTestExist(TestEzStr_StringToNumberEdgeCases);
    else IfTestExist(TestEzStr_StringToNumberVectorEmpty);
    else IfTestExist(TestEzStr_ToHexEdgeCases);
    else IfTestExist(TestEzStr_ToHexStrEdgeCases);
    else IfTestExist(TestEzStr_ToDecStrEdgeCases);
    else IfTestExist(TestEzStr_StrContainsEdgeCases);
    else IfTestExist(TestEzStr_ReplaceStringEdgeCases);
    else IfTestExist(TestEzStr_GetCountOccurenceEdgeCases);
    else IfTestExist(TestEzStr_GetCountOccurenceInSectionEdgeCases);
    else IfTestExist(TestEzStr_GetDigitsCountOfAIntegralNumberEdgeCases);
    else IfTestExist(TestEzStr_SearchForPatternWithWildcardsEdgeCases);
    else IfTestExist(TestEzStr_ExtractWildcardsFromPatternEdgeCases);
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
