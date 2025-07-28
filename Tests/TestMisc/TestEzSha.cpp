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
    const char* text = "quick brown fox jumps over the lazy dog";

    char hex[ez::sha1::SHA1_HEX_SIZE];
    char base64[ez::sha1::SHA1_BASE64_SIZE];

    // constructor can be empty or take a const char*
    ez::sha1("The ")
        // can be chained
        // can add single chars
        .add(text[0])
        // number of bytes
        .add(&text[1], 4)
        // 0-terminated const char*
        .add(&text[5])
        // finalize must be called, otherwise the hash is not valid
        // after that, no more bytes should be added
        .finalize()
        // print the hash in hexadecimal, 0-terminated
        .printHex(hex)
        // print the hash in base64, 0-terminated
        .printBase64(base64);

    CTEST_ASSERT(std::string(hex) == std::string("2fd4e1c67a2d28fced849ee1bb76e7391b93eb12"));
    CTEST_ASSERT(std::string(base64) == std::string("L9ThxnotKPzthJ7hu3bnORuT6xI="));

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
