#include <TestEzLzw.h>

#include <string>

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestCollectionExist(v)             \
    if (vTest.find(#v) != std::string::npos) \
    return v(vTest)

bool TestFile(const std::string& vTest) {
    IfTestCollectionExist(TestEzLzw);
    return false;
}

////////////////////////////////////////////////////////////////////////////
//// ENTRY POINT ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
    if (argc > 1) {
        printf("Exec test : %s\n", argv[1]);
        return TestFile(argv[1]) ? 0 : 1;
    }
    // User testing
    return TestFile("TestEzLzw_Comp_0") ? 0 : 1;
}