#include <ezlibs/ezSha.hpp>
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

bool TestEzSha_0() {
    //tested with http://www.sha1-online.com/
    const auto sha1 = ez::sha1("TOTO").finalize().getHex();
    CTEST_ASSERT(sha1 == "eefaf6bedac8f0f58af507ce3fde2a1b77b1cd89");
    const auto sha2 = ez::sha1("TOTO").add("TATA").finalize().getHex();
    CTEST_ASSERT(sha2 == "b222866188afb338a961b52151903118aef9d965");
    const auto sha3 = ez::sha1("TOTO").add("TATA").addValue(15).finalize().getHex();
    CTEST_ASSERT(sha3 == "89371dfb70c53526bc5be4b8f1f5ec112833d523");
    const auto sha4 = ez::sha1("TOTO").add("TATA").add("TITI").addValue(15).finalize().getHex();
    CTEST_ASSERT(sha4 == "1b98949f95c3dcb46daabff91db643152631be9a");

    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzSha(const std::string& vTest) {
    IfTestExist(TestEzSha_0);
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
