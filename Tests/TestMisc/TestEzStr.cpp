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
    CTEST_ASSERT(ez::str::searchForPatternWithWildcards(buffer, "TOTO*ZOO*BEAU") == buffer);
    CTEST_ASSERT(ez::str::searchForPatternWithWildcards(buffer, "TOTO*ZOO*BEAU", posRange) == buffer && posRange.first == 0 && posRange.second == buffer.size());
    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzStr(const std::string& vTest) {
    IfTestExist(TestEzStrGetDigitsCountOfAIntegralNumber);
    else IfTestExist(TestEzStrSearchForPatternWithWildcards);
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
