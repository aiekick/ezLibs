#include <TestEzCmdProcessor.h>
#include <TestEzGraph.h>
#include <TestEzXml.h>
#include <TestEzStr.h>
#include <TestEzCnt.h>
#include <TestEzFigFont.h>
#include <TestEzQrCode.h>

#define IfTestCollectionExist(v)             \
    if (vTest.find(#v) != std::string::npos) \
    return v(vTest)

bool TestMisc(const std::string& vTest) {
    IfTestCollectionExist(TestEzCmdProcessor);
    else IfTestCollectionExist(TestEzGraph);
    else IfTestCollectionExist(TestEzXml);
    else IfTestCollectionExist(TestEzStr);
    else IfTestCollectionExist(TestEzCnt);
    else IfTestCollectionExist(TestEzFigFont);
    else IfTestCollectionExist(TestEzQrCode);
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
    return TestMisc("TestEzQrCodeBase") ? 0 : 1;
}
