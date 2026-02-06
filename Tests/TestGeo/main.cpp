#include <TestEzGeo.h>
#include <TestEzTile.h>
#include <TestEzFormats.h>

#define IfTestCollectionExist(v)             \
    if (vTest.find(#v) != std::string::npos) \
    return v(vTest)

bool TestTime(const std::string& vTest) {
    IfTestCollectionExist(TestEzGeo);
    else IfTestCollectionExist(TestEzTile);
    else IfTestCollectionExist(TestEzFormats);
    // Check for format-specific tests that don't include "TestEzFormats" in their name
    else if (vTest.find("TestEzDemAsc") != std::string::npos ||
             vTest.find("TestEzDemBin") != std::string::npos ||
             vTest.find("TestEzHgt") != std::string::npos ||
             vTest.find("TestEzShp") != std::string::npos) {
        return TestEzFormats(vTest);
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////
//// ENTRY POINT ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
    if (argc > 1) {
        printf("Exec test : %s\n", argv[1]);
        return TestTime(argv[1]) ? 0 : 1;
    }
    // User testing
    return TestTime("TestEzGeo_CheckDemFileName_Valid_Upper") ? 0 : 1;
}
