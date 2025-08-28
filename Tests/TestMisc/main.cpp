#include <TestEzCmdProcessor.h>
#include <TestEzGraph.h>
#include <TestEzXml.h>
#include <TestEzStr.h>
#include <TestEzStackString.h>
#include <TestEzCnt.h>
#include <TestEzFigFont.h>
#include <TestEzSha.h>
#include <TestEzSqlite.h>
#include <TestEzScreen.h>
#ifdef TESTING_WIP
#include <TestEzQrCode.h>
#endif

#define IfTestCollectionExist(v)             \
    if (vTest.find(#v) != std::string::npos) \
    return v(vTest)

bool TestMisc(const std::string& vTest) {
    IfTestCollectionExist(TestEzCmdProcessor);
    else IfTestCollectionExist(TestEzGraph);
    else IfTestCollectionExist(TestEzXml);
    else IfTestCollectionExist(TestEzStr);
    else IfTestCollectionExist(TestEzStackString);
    else IfTestCollectionExist(TestEzCnt);
    else IfTestCollectionExist(TestEzFigFont);
    else IfTestCollectionExist(TestEzSha);
    else IfTestCollectionExist(TestEzSqlite);
    else IfTestCollectionExist(TestEzScreen);
#ifdef TESTING_WIP
    else IfTestCollectionExist(TestEzQrCode);
#endif
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
    return TestMisc("TestEzScreen_FloorNb_Scalar_Float") ? 0 : 1;
}
