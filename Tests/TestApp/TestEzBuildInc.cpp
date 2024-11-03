#include <TestEzBuildInc.h>
#include <ezlibs/ezBuildInc.hpp>
#include <ezlibs/ezFile.hpp>

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

bool TestEzBuildInc_base() {
    std::string file = "build.h";
    std::string baseContent = u8R"(
#pragma once

#define Project_Label "project"
#define Project_BuildNumber 3629
#define Project_MinorNumber 3
#define Project_MajorNumber 0
#define Project_BuildId "0.3.3629"
)";
    if (!ez::file::saveStringToFile(baseContent, file)) {
        return false;
    }
    auto builder = ez::BuildInc(file);
    if (builder.getBuildNumber() != 3629) {
        return false;
    }
    if (builder.getMinor() != 3) {
        return false;
    }
    if (builder.getMajor() != 0) {
        return false;
    }
    if (builder.getLabel() != "project") {
        return false;
    }
    if (builder.getProject() != "Project") {
        return false;
    }
    std::string expectedContent = u8R"(#pragma once

#define Project_Label "project"
#define Project_BuildNumber 3630
#define Project_MinorNumber 3
#define Project_MajorNumber 0
#define Project_BuildId "0.3.3630"
)";

    builder.incBuildNumber().write().printInfos();
    auto newContent = ez::file::loadFileToString(file);
    if (newContent != expectedContent) {
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

bool TestEzBuildInc(const std::string& vTest) {
    IfTestExist(TestEzBuildInc_base);
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
