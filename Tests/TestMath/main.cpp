#include <TestEzMath.h>
#include <TestEzVec2.h>
#include <TestEzVec3.h>
#include <TestEzVec4.h>
#include <TestEzAABBCC.h>
#include <TestEzMat2.h>
#include <TestEzQuat.h>
#include <TestEzAABB.h>
#include <TestEzExpr.h>

#include <limits>
#include <cmath>

#define IfTestCollectionExist(v)             \
    if (vTest.find(#v) != std::string::npos) \
    return v(vTest)

bool TestMisc(const std::string& vTest) {
    IfTestCollectionExist(TestEzMath);
    IfTestCollectionExist(TestEzAABBCC);
    IfTestCollectionExist(TestEzVec2);
    IfTestCollectionExist(TestEzVec3);
    IfTestCollectionExist(TestEzVec4);
    IfTestCollectionExist(TestEzMat2);
    IfTestCollectionExist(TestEzQuat);
    IfTestCollectionExist(TestEzAABB);
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
    return TestMisc("TestEzVec2_GetNormalized<double>") ? 0 : 1;
}
