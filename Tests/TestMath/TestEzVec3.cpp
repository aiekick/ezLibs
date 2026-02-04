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
bool TestEzVec3_Offset() {
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
bool TestEzVec3_Set() {
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
bool TestEzVec3_Negate() {
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
bool TestEzVec3_LogicalNot() {
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
bool TestEzVec3_XY() {
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
bool TestEzVec3_XZ() {
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
bool TestEzVec3_YZ() {
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
bool TestEzVec3_YZX() {
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
bool TestEzVec3_Increment() {
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
bool TestEzVec3_Decrement() {
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
bool TestEzVec3_Length() {
    ez::vec3<T> v(3, 4, 0);
    T len = v.length();
    if (len != 5)
        return false;
    return true;
}

template <>
bool TestEzVec3_Length<float>() {
    ez::vec3<float> v(3.0f, 4.0f, 0.0f);
    float len = v.length();
    if (ez::abs(len - 5.0f) > 0.0001f)
        return false;
    return true;
}

// Test for Normalize function
template <typename T>
bool TestEzVec3_Normalize() {
    ez::vec3<T> v(3, 4, 0);
    v.normalize();
    if (ez::isDifferent(v.length(), static_cast<T>(1.0)))
        return false;
    return true;
}

template <>
bool TestEzVec3_Normalize<int32_t>() {
    return false;  // Normalization is invalid for integer types
}

template <>
bool TestEzVec3_Normalize<int64_t>() {
    return false;  // Normalization is invalid for integer types
}

// Test for Sum function
template <typename T>
bool TestEzVec3_Sum() {
    ez::vec3<T> v(1, 2, 3);
    T sum = v.sum();
    if (sum != 6)
        return false;
    return true;
}

// Test for SumAbs function
template <typename T>
bool TestEzVec3_SumAbs() {
    ez::vec3<T> v(1, -2, 3);
    T sumAbs = v.sumAbs();
    if (sumAbs != 6)
        return false;
    return true;
}

// Test for EmptyAND function
template <typename T>
bool TestEzVec3_EmptyAND() {
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
bool TestEzVec3_EmptyOR() {
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
bool TestEzVec3_String() {
    ez::vec3<T> v(1, 2, 3);
    std::string str = v.string();
    if (str != "1;2;3")
        return false;
    return true;
}

// Test for Mini function
template <typename T>
bool TestEzVec3_Mini() {
    ez::vec3<T> v(1, 2, 3);
    if (ez::isDifferent(v.mini(), static_cast<T>(1)))
        return false;
    return true;
}

// Test for Maxi function
template <typename T>
bool TestEzVec3_Maxi() {
    ez::vec3<T> v(1, 2, 3);
    if (ez::isDifferent(v.maxi(), static_cast<T>(3)))
        return false;
    return true;
}

// Test for Equality operator
template <typename T>
bool TestEzVec3_Equality() {
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
bool TestEzVec3_Addition() {
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
bool TestEzVec3_Subtraction() {
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
bool TestEzVec3_Multiplication() {
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
bool TestEzVec3_Division() {
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
    IfTestExist(TestEzVec3_Offset<float>);
    else IfTestExist(TestEzVec3_Offset<double>);
    else IfTestExist(TestEzVec3_Offset<int32_t>);
    else IfTestExist(TestEzVec3_Offset<int64_t>);
    else IfTestExist(TestEzVec3_Offset<uint32_t>);
    else IfTestExist(TestEzVec3_Offset<uint64_t>);

    IfTestExist(TestEzVec3_Set<float>);
    else IfTestExist(TestEzVec3_Set<double>);
    else IfTestExist(TestEzVec3_Set<int32_t>);
    else IfTestExist(TestEzVec3_Set<uint32_t>);
    else IfTestExist(TestEzVec3_Set<int64_t>);
    else IfTestExist(TestEzVec3_Set<uint64_t>);

    IfTestExist(TestEzVec3_Negate<float>);
    else IfTestExist(TestEzVec3_Negate<double>);
    else IfTestExist(TestEzVec3_Negate<int32_t>);
    else IfTestExist(TestEzVec3_Negate<int64_t>);

    IfTestExist(TestEzVec3_LogicalNot<int32_t>);
    else IfTestExist(TestEzVec3_LogicalNot<int64_t>);

    IfTestExist(TestEzVec3_XY<float>);
    else IfTestExist(TestEzVec3_XY<double>);
    else IfTestExist(TestEzVec3_XY<int32_t>);
    else IfTestExist(TestEzVec3_XY<uint32_t>);
    else IfTestExist(TestEzVec3_XY<int64_t>);
    else IfTestExist(TestEzVec3_XY<uint64_t>);

    IfTestExist(TestEzVec3_XZ<float>);
    else IfTestExist(TestEzVec3_XZ<double>);
    else IfTestExist(TestEzVec3_XZ<int32_t>);
    else IfTestExist(TestEzVec3_XZ<uint32_t>);
    else IfTestExist(TestEzVec3_XZ<int64_t>);
    else IfTestExist(TestEzVec3_XZ<uint64_t>);

    IfTestExist(TestEzVec3_YZ<float>);
    else IfTestExist(TestEzVec3_YZ<double>);
    else IfTestExist(TestEzVec3_YZ<int32_t>);
    else IfTestExist(TestEzVec3_YZ<uint32_t>);
    else IfTestExist(TestEzVec3_YZ<int64_t>);
    else IfTestExist(TestEzVec3_YZ<uint64_t>);

    IfTestExist(TestEzVec3_YZX<float>);
    else IfTestExist(TestEzVec3_YZX<double>);
    else IfTestExist(TestEzVec3_YZX<int32_t>);
    else IfTestExist(TestEzVec3_YZX<uint32_t>);
    else IfTestExist(TestEzVec3_YZX<int64_t>);
    else IfTestExist(TestEzVec3_YZX<uint64_t>);

    IfTestExist(TestEzVec3_Increment<float>);
    else IfTestExist(TestEzVec3_Increment<double>);
    else IfTestExist(TestEzVec3_Increment<int32_t>);
    else IfTestExist(TestEzVec3_Increment<uint32_t>);
    else IfTestExist(TestEzVec3_Increment<int64_t>);
    else IfTestExist(TestEzVec3_Increment<uint64_t>);

    IfTestExist(TestEzVec3_Decrement<float>);
    else IfTestExist(TestEzVec3_Decrement<double>);
    else IfTestExist(TestEzVec3_Decrement<int32_t>);
    else IfTestExist(TestEzVec3_Decrement<uint32_t>);
    else IfTestExist(TestEzVec3_Decrement<int64_t>);
    else IfTestExist(TestEzVec3_Decrement<uint64_t>);

    IfTestExist(TestEzVec3_Length<float>);
    else IfTestExist(TestEzVec3_Length<double>);

    IfTestExist(TestEzVec3_Normalize<float>);
    else IfTestExist(TestEzVec3_Normalize<double>);

    IfTestExist(TestEzVec3_Sum<float>);
    else IfTestExist(TestEzVec3_Sum<double>);
    else IfTestExist(TestEzVec3_Sum<int32_t>);
    else IfTestExist(TestEzVec3_Sum<uint32_t>);
    else IfTestExist(TestEzVec3_Sum<int64_t>);
    else IfTestExist(TestEzVec3_Sum<uint64_t>);

    IfTestExist(TestEzVec3_SumAbs<float>);
    else IfTestExist(TestEzVec3_SumAbs<double>);
    else IfTestExist(TestEzVec3_SumAbs<int32_t>);
    else IfTestExist(TestEzVec3_SumAbs<int64_t>);

    IfTestExist(TestEzVec3_EmptyAND<float>);
    else IfTestExist(TestEzVec3_EmptyAND<double>);
    else IfTestExist(TestEzVec3_EmptyAND<int32_t>);
    else IfTestExist(TestEzVec3_EmptyAND<uint32_t>);
    else IfTestExist(TestEzVec3_EmptyAND<int64_t>);
    else IfTestExist(TestEzVec3_EmptyAND<uint64_t>);

    IfTestExist(TestEzVec3_EmptyOR<float>);
    else IfTestExist(TestEzVec3_EmptyOR<double>);
    else IfTestExist(TestEzVec3_EmptyOR<int32_t>);
    else IfTestExist(TestEzVec3_EmptyOR<uint32_t>);
    else IfTestExist(TestEzVec3_EmptyOR<int64_t>);
    else IfTestExist(TestEzVec3_EmptyOR<uint64_t>);

    IfTestExist(TestEzVec3_String<float>);
    else IfTestExist(TestEzVec3_String<double>);
    else IfTestExist(TestEzVec3_String<int32_t>);
    else IfTestExist(TestEzVec3_String<uint32_t>);
    else IfTestExist(TestEzVec3_String<int64_t>);
    else IfTestExist(TestEzVec3_String<uint64_t>);

    IfTestExist(TestEzVec3_Mini<float>);
    else IfTestExist(TestEzVec3_Mini<double>);
    else IfTestExist(TestEzVec3_Mini<int32_t>);
    else IfTestExist(TestEzVec3_Mini<uint32_t>);
    else IfTestExist(TestEzVec3_Mini<int64_t>);
    else IfTestExist(TestEzVec3_Mini<uint64_t>);

    IfTestExist(TestEzVec3_Maxi<float>);
    else IfTestExist(TestEzVec3_Maxi<double>);
    else IfTestExist(TestEzVec3_Maxi<int32_t>);
    else IfTestExist(TestEzVec3_Maxi<uint32_t>);
    else IfTestExist(TestEzVec3_Maxi<int64_t>);
    else IfTestExist(TestEzVec3_Maxi<uint64_t>);

    IfTestExist(TestEzVec3_Equality<float>);
    else IfTestExist(TestEzVec3_Equality<double>);
    else IfTestExist(TestEzVec3_Equality<int32_t>);
    else IfTestExist(TestEzVec3_Equality<uint32_t>);
    else IfTestExist(TestEzVec3_Equality<int64_t>);
    else IfTestExist(TestEzVec3_Equality<uint64_t>);

    IfTestExist(TestEzVec3_Addition<float>);
    else IfTestExist(TestEzVec3_Addition<double>);
    else IfTestExist(TestEzVec3_Addition<int32_t>);
    else IfTestExist(TestEzVec3_Addition<uint32_t>);
    else IfTestExist(TestEzVec3_Addition<int64_t>);
    else IfTestExist(TestEzVec3_Addition<uint64_t>);

    IfTestExist(TestEzVec3_Subtraction<float>);
    else IfTestExist(TestEzVec3_Subtraction<double>);
    else IfTestExist(TestEzVec3_Subtraction<int32_t>);
    else IfTestExist(TestEzVec3_Subtraction<uint32_t>);
    else IfTestExist(TestEzVec3_Subtraction<int64_t>);
    else IfTestExist(TestEzVec3_Subtraction<uint64_t>);

    IfTestExist(TestEzVec3_Multiplication<float>);
    else IfTestExist(TestEzVec3_Multiplication<double>);
    else IfTestExist(TestEzVec3_Multiplication<int32_t>);
    else IfTestExist(TestEzVec3_Multiplication<uint32_t>);
    else IfTestExist(TestEzVec3_Multiplication<int64_t>);
    else IfTestExist(TestEzVec3_Multiplication<uint64_t>);

    IfTestExist(TestEzVec3_Division<float>);
    else IfTestExist(TestEzVec3_Division<double>);
    else IfTestExist(TestEzVec3_Division<int32_t>);
    else IfTestExist(TestEzVec3_Division<uint32_t>);
    else IfTestExist(TestEzVec3_Division<int64_t>);
    else IfTestExist(TestEzVec3_Division<uint64_t>);

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
