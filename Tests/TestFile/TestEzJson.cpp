#ifdef TESTING_WIP

#include <ezlibs/wip/ezJson.hpp>
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

bool TestEzJson_Parse_1() {
    ez::Json js;
    CTEST_ASSERT(js.parse(R"({"nom":"Bob","age":25,"amis":["Alice","Marc"],"actif":true})"));
    auto& root = js.getRootRef();
    CTEST_ASSERT(root["nom"].asString() == "Bob");
    CTEST_ASSERT(root["age"].asNumber() == 25);
    CTEST_ASSERT(root["actif"].asBool() == true);
    root["ville"] = ez::json::Value("Lyon");
    CTEST_ASSERT(root["ville"].asString() == "Lyon");
    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzJson(const std::string& vTest) {
    IfTestExist(TestEzJson_Parse_1);
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

#endif
