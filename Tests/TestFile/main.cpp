#include <ezlibs/ezCTest.hpp>
#include <TestEzBmp.h>
#ifdef TESTING_WIP
#include <TestEzGif.h>
#include <TestEzPng.h>
#include <TestEzSvg.h>
#include <TestEzJson.h>
#endif
#include <TestEzFile.h>
#include <TestEzBinBuf.h>
#include <TestEzVdbWriter.h>
#include <TestEzVoxWriter.h>
#include <TestEzXmlConfig.h>
#include <TestEzFileWatcher.h>

#include <string>

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestFile(const std::string& vTest) {
    IfTestCollectionExist(TestEzBmp);
#ifdef TESTING_WIP
    else IfTestCollectionExist(TestEzGif);
    else IfTestCollectionExist(TestEzPng);
    else IfTestCollectionExist(TestEzSvg);
    else IfTestCollectionExist(TestEzJson);
#endif
    else IfTestCollectionExist(TestEzBinBuf);
    else IfTestCollectionExist(TestEzVdbWriter);
    else IfTestCollectionExist(TestEzVoxWriter);
    else IfTestCollectionExist(TestEzXmlConfig);
    else IfTestCollectionExist(TestEzFileWatcher);
    else IfTestCollectionExist(TestEzFile);
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
    return TestFile("TestEzFileWatcher_Dir") ? 0 : 1;
}
