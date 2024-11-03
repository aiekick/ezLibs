#include <TestEzBmp.h>
#include <TestEzGif.h>
#include <TestEzPng.h>
#include <TestEzVdbWriter.h>
#include <TestEzVoxWriter.h>

#include <string>

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestCollectionExist(v)             \
    if (vTest.find(#v) != std::string::npos) \
    return v(vTest)

bool TestFile(const std::string& vTest) {
    IfTestCollectionExist(TestEzBmp);
    else IfTestCollectionExist(TestEzGif);
    else IfTestCollectionExist(TestEzPng);
    else IfTestCollectionExist(TestEzVdbWriter);
    else IfTestCollectionExist(TestEzVoxWriter);
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
    return TestFile("TestEzGif_Writer") ? 0 : 1;
}
