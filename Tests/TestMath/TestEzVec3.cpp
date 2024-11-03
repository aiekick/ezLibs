#include <ezlibs/ezMath.hpp>
#include <cmath>
#include <limits>

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

// Test for Offset function
template <typename T>
bool TestEzVec3Offset() {
    ez::vec3<T> v(1, 2, 3);
    ez::vec3<T> offsetResult = v.Offset(1, 1, 1);
    if (offsetResult.x != 2)
        return false;
    if (offsetResult.y != 3)
        return false;
    if (offsetResult.z != 4)
        return false;
    return true;
}

// Test for Set function
template <typename T>
bool TestEzVec3Set() {
    ez::vec3<T> v;
    v.Set(4, 5, 6);
    if (v.x != 4)
        return false;
    if (v.y != 5)
        return false;
    if (v.z != 6)
        return false;
    return true;
}

// Test for Negate operator
template <typename T>
bool TestEzVec3Negate() {
    ez::vec3<T> v(1, -2, 3);
    ez::vec3<T> negateResult = -v;
    if (negateResult.x != -1)
        return false;
    if (negateResult.y != 2)
        return false;
    if (negateResult.z != -3)
        return false;
    return true;
}

// Test for logical NOT operator
template <typename T>
bool TestEzVec3LogicalNot() {
    ez::vec3<T> v(1, 0, 3);
    ez::vec3<T> notResult = !v;
    if (notResult.x != 0)
        return false;
    if (notResult.y != 1)
        return false;
    if (notResult.z != 0)
        return false;
    return true;
}

// Test for xy function
template <typename T>
bool TestEzVec3XY() {
    ez::vec3<T> v(1, 2, 3);
    ez::vec2<T> xyResult = v.xy();
    if (xyResult.x != 1)
        return false;
    if (xyResult.y != 2)
        return false;
    return true;
}

// Test for xz function
template <typename T>
bool TestEzVec3XZ() {
    ez::vec3<T> v(1, 2, 3);
    ez::vec2<T> xzResult = v.xz();
    if (xzResult.x != 1)
        return false;
    if (xzResult.y != 3)
        return false;
    return true;
}

// Test for yz function
template <typename T>
bool TestEzVec3YZ() {
    ez::vec3<T> v(1, 2, 3);
    ez::vec2<T> yzResult = v.yz();
    if (yzResult.x != 2)
        return false;
    if (yzResult.y != 3)
        return false;
    return true;
}

// Test for yzx function
template <typename T>
bool TestEzVec3YZX() {
    ez::vec3<T> v(1, 2, 3);
    ez::vec3<T> yzxResult = v.yzx();
    if (yzxResult.x != 2)
        return false;
    if (yzxResult.y != 3)
        return false;
    if (yzxResult.z != 1)
        return false;
    return true;
}

// Test for Increment operator
template <typename T>
bool TestEzVec3Increment() {
    ez::vec3<T> v(1, 2, 3);
    ++v;
    if (v.x != 2)
        return false;
    if (v.y != 3)
        return false;
    if (v.z != 4)
        return false;
    return true;
}

// Test for Decrement operator
template <typename T>
bool TestEzVec3Decrement() {
    ez::vec3<T> v(1, 2, 3);
    --v;
    if (v.x != 0)
        return false;
    if (v.y != 1)
        return false;
    if (v.z != 2)
        return false;
    return true;
}

// Test for Length function
template <typename T>
bool TestEzVec3Length() {
    ez::vec3<T> v(3, 4, 0);
    T len = v.length();
    if (len != 5)
        return false;
    return true;
}

template <>
bool TestEzVec3Length<float>() {
    ez::vec3<float> v(3.0f, 4.0f, 0.0f);
    float len = v.length();
    if (ez::abs(len - 5.0f) > 0.0001f)
        return false;
    return true;
}

// Test for Normalize function
template <typename T>
bool TestEzVec3Normalize() {
    ez::vec3<T> v(3, 4, 0);
    v.normalize();
    if (ez::isDifferent(v.length(), static_cast<T>(1.0)))
        return false;
    return true;
}

template <>
bool TestEzVec3Normalize<int32_t>() {
    return false;  // Normalization is invalid for integer types
}

template <>
bool TestEzVec3Normalize<int64_t>() {
    return false;  // Normalization is invalid for integer types
}

// Test for Sum function
template <typename T>
bool TestEzVec3Sum() {
    ez::vec3<T> v(1, 2, 3);
    T sum = v.sum();
    if (sum != 6)
        return false;
    return true;
}

// Test for SumAbs function
template <typename T>
bool TestEzVec3SumAbs() {
    ez::vec3<T> v(1, -2, 3);
    T sumAbs = v.sumAbs();
    if (sumAbs != 6)
        return false;
    return true;
}

// Test for EmptyAND function
template <typename T>
bool TestEzVec3EmptyAND() {
    ez::vec3<T> v(0, 0, 0);
    if (!v.emptyAND())
        return false;
    v.x = 1;
    if (v.emptyAND())
        return false;
    return true;
}

// Test for EmptyOR function
template <typename T>
bool TestEzVec3EmptyOR() {
    ez::vec3<T> v(0, 1, 1);
    if (!v.emptyOR())
        return false;
    v.x = 1;
    if (v.emptyOR())
        return false;
    return true;
}

// Test for String conversion
template <typename T>
bool TestEzVec3String() {
    ez::vec3<T> v(1, 2, 3);
    std::string str = v.string();
    if (str != "1;2;3")
        return false;
    return true;
}

// Test for Mini function
template <typename T>
bool TestEzVec3Mini() {
    ez::vec3<T> v(1, 2, 3);
    if (ez::isDifferent(v.mini(), static_cast<T>(1)))
        return false;
    return true;
}

// Test for Maxi function
template <typename T>
bool TestEzVec3Maxi() {
    ez::vec3<T> v(1, 2, 3);
    if (ez::isDifferent(v.maxi(), static_cast<T>(3)))
        return false;
    return true;
}

// Test for Equality operator
template <typename T>
bool TestEzVec3Equality() {
    ez::vec3<T> v1(1, 2, 3);
    ez::vec3<T> v2(1, 2, 3);
    if (!(v1 == v2))
        return false;
    ez::vec3<T> v3(4, 5, 6);
    if (v1 == v3)
        return false;
    return true;
}

// Test for Arithmetic operators
template <typename T>
bool TestEzVec3Addition() {
    ez::vec3<T> v1(1, 2, 3);
    ez::vec3<T> v2(4, 5, 6);
    ez::vec3<T> result = v1 + v2;
    if (result.x != 5)
        return false;
    if (result.y != 7)
        return false;
    if (result.z != 9)
        return false;
    return true;
}

template <typename T>
bool TestEzVec3Subtraction() {
    ez::vec3<T> v1(4, 5, 6);
    ez::vec3<T> v2(1, 2, 3);
    ez::vec3<T> result = v1 - v2;
    if (result.x != 3)
        return false;
    if (result.y != 3)
        return false;
    if (result.z != 3)
        return false;
    return true;
}

template <typename T>
bool TestEzVec3Multiplication() {
    ez::vec3<T> v(1, 2, 3);
    ez::vec3<T> result = v * static_cast<T>(2);
    if (result.x != 2)
        return false;
    if (result.y != 4)
        return false;
    if (result.z != 6)
        return false;
    return true;
}

template <typename T>
bool TestEzVec3Division() {
    ez::vec3<T> v(4, 6, 8);
    ez::vec3<T> result = v / static_cast<T>(2);
    if (result.x != 2)
        return false;
    if (result.y != 3)
        return false;
    if (result.z != 4)
        return false;
    return true;
}

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzVec3(const std::string& vTest) {
    IfTestExist(TestEzVec3Offset<float>);
    else IfTestExist(TestEzVec3Offset<double>);
    else IfTestExist(TestEzVec3Offset<int32_t>);
    else IfTestExist(TestEzVec3Offset<int64_t>);
    else IfTestExist(TestEzVec3Offset<uint32_t>);
    else IfTestExist(TestEzVec3Offset<uint64_t>);

    IfTestExist(TestEzVec3Set<float>);
    else IfTestExist(TestEzVec3Set<double>);
    else IfTestExist(TestEzVec3Set<int32_t>);
    else IfTestExist(TestEzVec3Set<uint32_t>);
    else IfTestExist(TestEzVec3Set<int64_t>);
    else IfTestExist(TestEzVec3Set<uint64_t>);

    IfTestExist(TestEzVec3Negate<float>);
    else IfTestExist(TestEzVec3Negate<double>);
    else IfTestExist(TestEzVec3Negate<int32_t>);
    else IfTestExist(TestEzVec3Negate<int64_t>);

    IfTestExist(TestEzVec3LogicalNot<int32_t>);
    else IfTestExist(TestEzVec3LogicalNot<int64_t>);

    IfTestExist(TestEzVec3XY<float>);
    else IfTestExist(TestEzVec3XY<double>);
    else IfTestExist(TestEzVec3XY<int32_t>);
    else IfTestExist(TestEzVec3XY<uint32_t>);
    else IfTestExist(TestEzVec3XY<int64_t>);
    else IfTestExist(TestEzVec3XY<uint64_t>);

    IfTestExist(TestEzVec3XZ<float>);
    else IfTestExist(TestEzVec3XZ<double>);
    else IfTestExist(TestEzVec3XZ<int32_t>);
    else IfTestExist(TestEzVec3XZ<uint32_t>);
    else IfTestExist(TestEzVec3XZ<int64_t>);
    else IfTestExist(TestEzVec3XZ<uint64_t>);

    IfTestExist(TestEzVec3YZ<float>);
    else IfTestExist(TestEzVec3YZ<double>);
    else IfTestExist(TestEzVec3YZ<int32_t>);
    else IfTestExist(TestEzVec3YZ<uint32_t>);
    else IfTestExist(TestEzVec3YZ<int64_t>);
    else IfTestExist(TestEzVec3YZ<uint64_t>);

    IfTestExist(TestEzVec3YZX<float>);
    else IfTestExist(TestEzVec3YZX<double>);
    else IfTestExist(TestEzVec3YZX<int32_t>);
    else IfTestExist(TestEzVec3YZX<uint32_t>);
    else IfTestExist(TestEzVec3YZX<int64_t>);
    else IfTestExist(TestEzVec3YZX<uint64_t>);

    IfTestExist(TestEzVec3Increment<float>);
    else IfTestExist(TestEzVec3Increment<double>);
    else IfTestExist(TestEzVec3Increment<int32_t>);
    else IfTestExist(TestEzVec3Increment<uint32_t>);
    else IfTestExist(TestEzVec3Increment<int64_t>);
    else IfTestExist(TestEzVec3Increment<uint64_t>);

    IfTestExist(TestEzVec3Decrement<float>);
    else IfTestExist(TestEzVec3Decrement<double>);
    else IfTestExist(TestEzVec3Decrement<int32_t>);
    else IfTestExist(TestEzVec3Decrement<uint32_t>);
    else IfTestExist(TestEzVec3Decrement<int64_t>);
    else IfTestExist(TestEzVec3Decrement<uint64_t>);

    IfTestExist(TestEzVec3Length<float>);
    else IfTestExist(TestEzVec3Length<double>);

    IfTestExist(TestEzVec3Normalize<float>);
    else IfTestExist(TestEzVec3Normalize<double>);

    IfTestExist(TestEzVec3Sum<float>);
    else IfTestExist(TestEzVec3Sum<double>);
    else IfTestExist(TestEzVec3Sum<int32_t>);
    else IfTestExist(TestEzVec3Sum<uint32_t>);
    else IfTestExist(TestEzVec3Sum<int64_t>);
    else IfTestExist(TestEzVec3Sum<uint64_t>);

    IfTestExist(TestEzVec3SumAbs<float>);
    else IfTestExist(TestEzVec3SumAbs<double>);
    else IfTestExist(TestEzVec3SumAbs<int32_t>);
    else IfTestExist(TestEzVec3SumAbs<int64_t>);

    IfTestExist(TestEzVec3EmptyAND<float>);
    else IfTestExist(TestEzVec3EmptyAND<double>);
    else IfTestExist(TestEzVec3EmptyAND<int32_t>);
    else IfTestExist(TestEzVec3EmptyAND<uint32_t>);
    else IfTestExist(TestEzVec3EmptyAND<int64_t>);
    else IfTestExist(TestEzVec3EmptyAND<uint64_t>);

    IfTestExist(TestEzVec3EmptyOR<float>);
    else IfTestExist(TestEzVec3EmptyOR<double>);
    else IfTestExist(TestEzVec3EmptyOR<int32_t>);
    else IfTestExist(TestEzVec3EmptyOR<uint32_t>);
    else IfTestExist(TestEzVec3EmptyOR<int64_t>);
    else IfTestExist(TestEzVec3EmptyOR<uint64_t>);

    IfTestExist(TestEzVec3String<float>);
    else IfTestExist(TestEzVec3String<double>);
    else IfTestExist(TestEzVec3String<int32_t>);
    else IfTestExist(TestEzVec3String<uint32_t>);
    else IfTestExist(TestEzVec3String<int64_t>);
    else IfTestExist(TestEzVec3String<uint64_t>);

    IfTestExist(TestEzVec3Mini<float>);
    else IfTestExist(TestEzVec3Mini<double>);
    else IfTestExist(TestEzVec3Mini<int32_t>);
    else IfTestExist(TestEzVec3Mini<uint32_t>);
    else IfTestExist(TestEzVec3Mini<int64_t>);
    else IfTestExist(TestEzVec3Mini<uint64_t>);

    IfTestExist(TestEzVec3Maxi<float>);
    else IfTestExist(TestEzVec3Maxi<double>);
    else IfTestExist(TestEzVec3Maxi<int32_t>);
    else IfTestExist(TestEzVec3Maxi<uint32_t>);
    else IfTestExist(TestEzVec3Maxi<int64_t>);
    else IfTestExist(TestEzVec3Maxi<uint64_t>);

    IfTestExist(TestEzVec3Equality<float>);
    else IfTestExist(TestEzVec3Equality<double>);
    else IfTestExist(TestEzVec3Equality<int32_t>);
    else IfTestExist(TestEzVec3Equality<uint32_t>);
    else IfTestExist(TestEzVec3Equality<int64_t>);
    else IfTestExist(TestEzVec3Equality<uint64_t>);

    IfTestExist(TestEzVec3Addition<float>);
    else IfTestExist(TestEzVec3Addition<double>);
    else IfTestExist(TestEzVec3Addition<int32_t>);
    else IfTestExist(TestEzVec3Addition<uint32_t>);
    else IfTestExist(TestEzVec3Addition<int64_t>);
    else IfTestExist(TestEzVec3Addition<uint64_t>);

    IfTestExist(TestEzVec3Subtraction<float>);
    else IfTestExist(TestEzVec3Subtraction<double>);
    else IfTestExist(TestEzVec3Subtraction<int32_t>);
    else IfTestExist(TestEzVec3Subtraction<uint32_t>);
    else IfTestExist(TestEzVec3Subtraction<int64_t>);
    else IfTestExist(TestEzVec3Subtraction<uint64_t>);

    IfTestExist(TestEzVec3Multiplication<float>);
    else IfTestExist(TestEzVec3Multiplication<double>);
    else IfTestExist(TestEzVec3Multiplication<int32_t>);
    else IfTestExist(TestEzVec3Multiplication<uint32_t>);
    else IfTestExist(TestEzVec3Multiplication<int64_t>);
    else IfTestExist(TestEzVec3Multiplication<uint64_t>);

    IfTestExist(TestEzVec3Division<float>);
    else IfTestExist(TestEzVec3Division<double>);
    else IfTestExist(TestEzVec3Division<int32_t>);
    else IfTestExist(TestEzVec3Division<uint32_t>);
    else IfTestExist(TestEzVec3Division<int64_t>);
    else IfTestExist(TestEzVec3Division<uint64_t>);

    return false;  // Return false if the test case is not found
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
