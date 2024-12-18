#include <TestEzApp.h>
#include <TestEzArgs.h>
#include <TestEzBuildInc.h>

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestCollectionExist(v)             \
    if (vTest.find(#v) != std::string::npos) \
    return v(vTest)

bool TestApp(const std::string& vTest) {
    IfTestCollectionExist(TestEzApp);
    else IfTestCollectionExist(TestEzArgs);
    else IfTestCollectionExist(TestEzBuildInc);
    return true;
}

////////////////////////////////////////////////////////////////////////////
//// ENTRY POINT ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
    if (argc > 1) {
        printf("Exec test : %s\n", argv[1]);
        return TestApp(argv[1]) ? 0 : 1;
    }
    // User testing
    return TestApp("TestEzBuildInc_base") ? 0 : 1;
}