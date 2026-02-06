#include <TestEzAABBCC.h>
#include <ezlibs/ezMath.hpp>
#include <ezlibs/ezVec3.hpp>
#include <ezlibs/ezAABBCC.hpp>

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// Desactivation des warnings de conversion
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4244)  // Conversion from 'double' to 'float', possible loss of data
#pragma warning(disable : 4305)  // Truncation from 'double' to 'float'
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzAABBCC_Constructor() {
    ez::fAABBCC aabb;
    if (aabb.lowerBound.x != 0.0f || aabb.lowerBound.y != 0.0f || aabb.lowerBound.z != 0.0f)
        return false;
    if (aabb.upperBound.x != 0.0f || aabb.upperBound.y != 0.0f || aabb.upperBound.z != 0.0f)
        return false;

    ez::fvec3 lower(1.0f, 2.0f, 3.0f);
    ez::fvec3 upper(4.0f, 5.0f, 6.0f);
    ez::fAABBCC aabb2(lower, upper);
    if (aabb2.lowerBound != lower)
        return false;
    if (aabb2.upperBound != upper)
        return false;

    // Test with swapped bounds (should auto-correct)
    ez::fAABBCC aabb3(upper, lower);
    if (aabb3.lowerBound != lower)
        return false;
    if (aabb3.upperBound != upper)
        return false;

    return true;
}

bool TestEzAABBCC_GetCenter() {
    ez::fvec3 lower(0.0f, 0.0f, 0.0f);
    ez::fvec3 upper(10.0f, 20.0f, 30.0f);
    ez::fAABBCC aabb(lower, upper);

    ez::fvec3 center = aabb.GetCenter();
    if (center.x != 5.0f || center.y != 10.0f || center.z != 15.0f)
        return false;

    return true;
}

bool TestEzAABBCC_GetExtents() {
    ez::fvec3 lower(0.0f, 0.0f, 0.0f);
    ez::fvec3 upper(10.0f, 20.0f, 30.0f);
    ez::fAABBCC aabb(lower, upper);

    ez::fvec3 extents = aabb.GetExtents();
    if (extents.x != 5.0f || extents.y != 10.0f || extents.z != 15.0f)
        return false;

    return true;
}

bool TestEzAABBCC_GetPerimeter() {
    ez::fvec3 lower(0.0f, 0.0f, 0.0f);
    ez::fvec3 upper(10.0f, 20.0f, 30.0f);
    ez::fAABBCC aabb(lower, upper);

    float perimeter = aabb.GetPerimeter();
    // Perimeter = 2 * (wx + wy + wz) = 2 * (10 + 20 + 30) = 120
    if (perimeter != 120.0f)
        return false;

    return true;
}

bool TestEzAABBCC_Size() {
    ez::fvec3 lower(0.0f, 0.0f, 0.0f);
    ez::fvec3 upper(10.0f, 20.0f, 30.0f);
    ez::fAABBCC aabb(lower, upper);

    ez::fvec3 size = aabb.Size();
    if (size.x != 10.0f || size.y != 20.0f || size.z != 30.0f)
        return false;

    return true;
}

bool TestEzAABBCC_CombineAABB() {
    ez::fAABBCC aabb1(ez::fvec3(0.0f, 0.0f, 0.0f), ez::fvec3(10.0f, 10.0f, 10.0f));
    ez::fAABBCC aabb2(ez::fvec3(5.0f, 5.0f, 5.0f), ez::fvec3(15.0f, 15.0f, 15.0f));

    aabb1.Combine(aabb2);

    if (aabb1.lowerBound != ez::fvec3(0.0f, 0.0f, 0.0f))
        return false;
    if (aabb1.upperBound != ez::fvec3(15.0f, 15.0f, 15.0f))
        return false;

    return true;
}

bool TestEzAABBCC_CombineTwoAABBs() {
    ez::fAABBCC aabb1(ez::fvec3(0.0f, 0.0f, 0.0f), ez::fvec3(10.0f, 10.0f, 10.0f));
    ez::fAABBCC aabb2(ez::fvec3(5.0f, 5.0f, 5.0f), ez::fvec3(15.0f, 15.0f, 15.0f));
    ez::fAABBCC result;

    result.Combine(aabb1, aabb2);

    if (result.lowerBound != ez::fvec3(0.0f, 0.0f, 0.0f))
        return false;
    if (result.upperBound != ez::fvec3(15.0f, 15.0f, 15.0f))
        return false;

    return true;
}

bool TestEzAABBCC_CombinePoint() {
    ez::fAABBCC aabb(ez::fvec3(0.0f, 0.0f, 0.0f), ez::fvec3(10.0f, 10.0f, 10.0f));

    aabb.Combine(ez::fvec3(15.0f, 5.0f, 5.0f));

    if (aabb.lowerBound != ez::fvec3(0.0f, 0.0f, 0.0f))
        return false;
    if (aabb.upperBound != ez::fvec3(15.0f, 10.0f, 10.0f))
        return false;

    aabb.Combine(ez::fvec3(-5.0f, 5.0f, 5.0f));

    if (aabb.lowerBound != ez::fvec3(-5.0f, 0.0f, 0.0f))
        return false;

    return true;
}

bool TestEzAABBCC_Contains() {
    ez::fAABBCC aabb1(ez::fvec3(0.0f, 0.0f, 0.0f), ez::fvec3(10.0f, 10.0f, 10.0f));
    ez::fAABBCC aabb2(ez::fvec3(2.0f, 2.0f, 2.0f), ez::fvec3(8.0f, 8.0f, 8.0f));
    ez::fAABBCC aabb3(ez::fvec3(5.0f, 5.0f, 5.0f), ez::fvec3(15.0f, 15.0f, 15.0f));

    if (!aabb1.Contains(aabb2))
        return false;
    if (aabb1.Contains(aabb3))
        return false;

    return true;
}

bool TestEzAABBCC_ContainsPoint() {
    ez::fAABBCC aabb(ez::fvec3(0.0f, 0.0f, 0.0f), ez::fvec3(10.0f, 10.0f, 10.0f));

    if (!aabb.ContainsPoint(ez::fvec3(5.0f, 5.0f, 5.0f)))
        return false;
    if (!aabb.ContainsPoint(ez::fvec3(0.0f, 0.0f, 0.0f)))
        return false;
    if (!aabb.ContainsPoint(ez::fvec3(10.0f, 10.0f, 10.0f)))
        return false;
    if (aabb.ContainsPoint(ez::fvec3(-1.0f, 5.0f, 5.0f)))
        return false;
    if (aabb.ContainsPoint(ez::fvec3(11.0f, 5.0f, 5.0f)))
        return false;

    return true;
}

bool TestEzAABBCC_Operators() {
    ez::fAABBCC aabb(ez::fvec3(0.0f, 0.0f, 0.0f), ez::fvec3(10.0f, 10.0f, 10.0f));

    // Test += operator
    aabb += ez::fvec3(5.0f, 5.0f, 5.0f);
    if (aabb.lowerBound != ez::fvec3(5.0f, 5.0f, 5.0f))
        return false;
    if (aabb.upperBound != ez::fvec3(15.0f, 15.0f, 15.0f))
        return false;

    // Test -= operator
    aabb -= ez::fvec3(5.0f, 5.0f, 5.0f);
    if (aabb.lowerBound != ez::fvec3(0.0f, 0.0f, 0.0f))
        return false;
    if (aabb.upperBound != ez::fvec3(10.0f, 10.0f, 10.0f))
        return false;

    // Test *= operator
    aabb *= 2.0f;
    if (aabb.lowerBound != ez::fvec3(0.0f, 0.0f, 0.0f))
        return false;
    if (aabb.upperBound != ez::fvec3(20.0f, 20.0f, 20.0f))
        return false;

    // Test /= operator
    aabb /= 2.0f;
    if (aabb.lowerBound != ez::fvec3(0.0f, 0.0f, 0.0f))
        return false;
    if (aabb.upperBound != ez::fvec3(10.0f, 10.0f, 10.0f))
        return false;

    return true;
}

////////////////////////////////////////////////////////////////////////////
//// ENTRY POINT ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzAABBCC(const std::string& vTest) {
    IfTestExist(TestEzAABBCC_Constructor);
    else IfTestExist(TestEzAABBCC_GetCenter);
    else IfTestExist(TestEzAABBCC_GetExtents);
    else IfTestExist(TestEzAABBCC_GetPerimeter);
    else IfTestExist(TestEzAABBCC_Size);
    else IfTestExist(TestEzAABBCC_CombineAABB);
    else IfTestExist(TestEzAABBCC_CombineTwoAABBs);
    else IfTestExist(TestEzAABBCC_CombinePoint);
    else IfTestExist(TestEzAABBCC_Contains);
    else IfTestExist(TestEzAABBCC_ContainsPoint);
    else IfTestExist(TestEzAABBCC_Operators);
    return false;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
