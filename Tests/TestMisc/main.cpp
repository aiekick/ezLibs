#include <TestEzGraph.h>
#include <TestEzXml.h>
#include <TestEzStr.h>
#include <TestEzFigFont.h>

#define IfTestCollectionExist(v)             \
    if (vTest.find(#v) != std::string::npos) \
    return v(vTest)

bool TestMisc(const std::string& vTest) {
    IfTestCollectionExist(TestEzGraph);
    else IfTestCollectionExist(TestEzXml);
    else IfTestCollectionExist(TestEzStr);
    else IfTestCollectionExist(TestEzFigFont);
    return false;
}

////////////////////////////////////////////////////////////////////////////
//// ENTRY POINT ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
    if (argc > 1) {
        printf("Exec test : %s\n", argv[1]);
        return TestMisc(argv[1]) ? 0 : 1;
    }
    // User testing
    return TestMisc("TestEzFigFontLoadAndPrint") ? 0 : 1;
}
