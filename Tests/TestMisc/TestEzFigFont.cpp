#include <TestEzFigFont.h>
#include <ezlibs/ezFigFont.hpp>
#include <ezlibs/ezCTest.hpp>

#include <iostream>
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

bool TestEzFigFontLoadAndPrint() {
    ez::FigFont ff;
    CTEST_ASSERT(ff.load(PROJECT_PATH "TestMisc/res/big.flf").isValid());
    const std::string expected_result = u8R"(            _____                                                       ___      __ 
           |  __ \                                                     / _ \    /_ |
  ___  ____| |__) |  ___  _ __    __ _  _ __ ___    ___  _ __  __   __| | | |    | |
 / _ \|_  /|  _  /  / _ \| '_ \  / _` || '_ ` _ \  / _ \| '__| \ \ / /| | | |    | |
|  __/ / / | | \ \ |  __/| | | || (_| || | | | | ||  __/| |     \ V / | |_| | _  | |
 \___|/___||_|  \_\ \___||_| |_| \__,_||_| |_| |_| \___||_|      \_/   \___/ (_) |_|
                                                                                    
                                                                                    
)";
    CTEST_ASSERT(ff.printString("ezRenamer v0.1") == expected_result);
    return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzFigFont(const std::string& vTest) {
    IfTestExist(TestEzFigFontLoadAndPrint);
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
