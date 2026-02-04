#include <TestEzCron.h>
#include <TestEzDate.h>
#include <TestEzTime.h>

#define IfTestCollectionExist(v)             \
    if (vTest.find(#v) != std::string::npos) \
    return v(vTest)

bool TestTime(const std::string& vTest) {
    IfTestCollectionExist(TestEzCron);
    else IfTestCollectionExist(TestEzDate);
    else IfTestCollectionExist(TestEzTime);
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
    return TestTime("TestEzTime_MeasureOperationUs_WithWork") ? 0 : 1;
}
