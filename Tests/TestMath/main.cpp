#include <TestEzMath.h>
#include <TestEzVec2.h>
#include <TestEzVec3.h>
#include <TestEzVec4.h>
#include <TestEzExpr.h>

#include <limits>
#include <cmath>

#define IfTestCollectionExist(v)             \
    if (vTest.find(#v) != std::string::npos) \
    return v(vTest)

bool TestMisc(const std::string& vTest) {
    IfTestCollectionExist(TestEzMath);
    IfTestCollectionExist(TestEzVec2);
    IfTestCollectionExist(TestEzVec3);
    IfTestCollectionExist(TestEzVec4);
    IfTestCollectionExist(TestEzExpr);
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
    return TestMisc("TestEzExpr_Exceptions_EvaluationNaN_Log") ? 0 : 1;
}
