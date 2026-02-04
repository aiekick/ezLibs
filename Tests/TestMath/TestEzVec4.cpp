#include <ezlibs/ezMath.hpp>

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

template <typename T>
bool TestEzVec4_Offset() {
    ez::vec4<T> v(1, 2, 3, 4);
    ez::vec4<T> offsetResult = v.Offset(1, 1, 1, 1);
    if (!ez::isEqual(offsetResult.x, static_cast<T>(2)))
        return false;
    if (!ez::isEqual(offsetResult.y, static_cast<T>(3)))
        return false;
    if (!ez::isEqual(offsetResult.z, static_cast<T>(4)))
        return false;
    if (!ez::isEqual(offsetResult.w, static_cast<T>(5)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4_Set() {
    ez::vec4<T> v;
    v.Set(4, 5, 6, 7);
    if (!ez::isEqual(v.x, static_cast<T>(4)))
        return false;
    if (!ez::isEqual(v.y, static_cast<T>(5)))
        return false;
    if (!ez::isEqual(v.z, static_cast<T>(6)))
        return false;
    if (!ez::isEqual(v.w, static_cast<T>(7)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4_Negate() {
    ez::vec4<T> v(1, -2, 3, -4);
    ez::vec4<T> negateResult = -v;
    if (!ez::isEqual(negateResult.x, static_cast<T>(-1)))
        return false;
    if (!ez::isEqual(negateResult.y, static_cast<T>(2)))
        return false;
    if (!ez::isEqual(negateResult.z, static_cast<T>(-3)))
        return false;
    if (!ez::isEqual(negateResult.w, static_cast<T>(4)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4_LogicalNot() {
    ez::vec4<T> v(1, 0, 3, 0);
    ez::vec4<T> notResult = !v;
    if (!ez::isEqual(notResult.x, static_cast<T>(0)))
        return false;
    if (!ez::isEqual(notResult.y, static_cast<T>(1)))
        return false;
    if (!ez::isEqual(notResult.z, static_cast<T>(0)))
        return false;
    if (!ez::isEqual(notResult.w, static_cast<T>(1)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4_XY() {
    ez::vec4<T> v(1, 2, 3, 4);
    ez::vec2<T> xyResult = v.xy();
    if (!ez::isEqual(xyResult.x, static_cast<T>(1)))
        return false;
    if (!ez::isEqual(xyResult.y, static_cast<T>(2)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4_XYZ() {
    ez::vec4<T> v(1, 2, 3, 4);
    ez::vec3<T> xyzResult = v.xyz();
    if (!ez::isEqual(xyzResult.x, static_cast<T>(1)))
        return false;
    if (!ez::isEqual(xyzResult.y, static_cast<T>(2)))
        return false;
    if (!ez::isEqual(xyzResult.z, static_cast<T>(3)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4_ZW() {
    ez::vec4<T> v(1, 2, 3, 4);
    ez::vec2<T> zwResult = v.zw();
    if (!ez::isEqual(zwResult.x, static_cast<T>(3)))
        return false;
    if (!ez::isEqual(zwResult.y, static_cast<T>(4)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4_Increment() {
    ez::vec4<T> v(1, 2, 3, 4);
    ++v;
    if (!ez::isEqual(v.x, static_cast<T>(2)))
        return false;
    if (!ez::isEqual(v.y, static_cast<T>(3)))
        return false;
    if (!ez::isEqual(v.z, static_cast<T>(4)))
        return false;
    if (!ez::isEqual(v.w, static_cast<T>(5)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4_Decrement() {
    ez::vec4<T> v(1, 2, 3, 4);
    --v;
    if (!ez::isEqual(v.x, static_cast<T>(0)))
        return false;
    if (!ez::isEqual(v.y, static_cast<T>(1)))
        return false;
    if (!ez::isEqual(v.z, static_cast<T>(2)))
        return false;
    if (!ez::isEqual(v.w, static_cast<T>(3)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4_Length() {
    ez::vec4<T> v(1, 2, 3, 4);
    if (!ez::isEqual(v.length(), static_cast<T>(5.4772255750516612)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4_Normalize() {
    ez::vec4<T> v(1, 2, 2, 1);
    v.normalize();
    if (ez::isDifferent(v.length(), static_cast<T>(1.0)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4_Sum() {
    ez::vec4<T> v(1, 2, 3, 4);
    T sum = v.sum();
    if (!ez::isEqual(sum, static_cast<T>(10)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4_SumAbs() {
    ez::vec4<T> v(1, -2, 3, -4);
    T sumAbs = v.sumAbs();
    if (!ez::isEqual(sumAbs, static_cast<T>(10)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4_EmptyAND() {
    ez::vec4<T> v;
    if (!v.emptyAND())
        return false;
    v.x = 1;
    if (v.emptyAND())
        return false;
    return true;
}

template <typename T>
bool TestEzVec4_EmptyOR() {
    ez::vec4<T> v(0, 1, 1, 1);
    if (!v.emptyOR())
        return false;
    v.x = 1;
    if (v.emptyOR())
        return false;
    return true;
}

template <typename T>
bool TestEzVec4_String() {
    ez::vec4<T> v(1, 2, 3, 4);
    const std::string str = v.string();
    if (str != "1;2;3;4")
        return false;
    return true;
}

template <typename T>
bool TestEzVec4_Mini() {
   const ez::vec4<T> v(1, 2, 3, 4);
    T mini = v.mini();
    if (!ez::isEqual(mini, static_cast<T>(1)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4_Maxi() {
    ez::vec4<T> v(1, 2, 3, 4);
    T maxi = v.maxi();
    if (!ez::isEqual(maxi, static_cast<T>(4)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4_Equality() {
    ez::vec4<T> v1(1, 2, 3, 4);
    ez::vec4<T> v2(1, 2, 3, 4);
    if (!ez::isEqual(v1, v2))
        return false;
    ez::vec4<T> v3(5, 6, 7, 8);
    if (ez::isEqual(v1, v3))
        return false;
    return true;
}

template <>
bool TestEzVec4_Equality<float>() {
    const ez::vec4<float> v1(1.0f, 2.0f, 3.0f, 4.0f);
    const ez::vec4<float> v2(1.0f, 2.0f, 3.0f, 4.0f);
    if (!ez::isEqual(v1, v2))
        return false;
    const ez::vec4<float> v3(5.0f, 2.0f, 3.0f, 4.0f);
    if (ez::isEqual(v1, v3))
        return false;
    const ez::vec4<float> v4(1.0f, 5.0f, 3.0f, 4.0f);
    if (ez::isEqual(v1, v4))
        return false;
    const ez::vec4<float> v5(1.0f, 2.0f, 5.0f, 4.0f);
    if (ez::isEqual(v1, v4))
        return false;
    const ez::vec4<float> v6(1.0f, 2.0f, 3.0f, 5.0f);
    if (ez::isEqual(v1, v6))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4_Addition() {
    ez::vec4<T> v1(1, 2, 3, 4);
    ez::vec4<T> v2(5, 6, 7, 8);
    ez::vec4<T> result = v1 + v2;
    if (!ez::isEqual(result.x, static_cast<T>(6)))
        return false;
    if (!ez::isEqual(result.y, static_cast<T>(8)))
        return false;
    if (!ez::isEqual(result.z, static_cast<T>(10)))
        return false;
    if (!ez::isEqual(result.w, static_cast<T>(12)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4_Subtraction() {
    ez::vec4<T> v1(5, 6, 7, 8);
    ez::vec4<T> v2(1, 2, 3, 4);
    ez::vec4<T> result = v1 - v2;
    if (!ez::isEqual(result.x, static_cast<T>(4)))
        return false;
    if (!ez::isEqual(result.y, static_cast<T>(4)))
        return false;
    if (!ez::isEqual(result.z, static_cast<T>(4)))
        return false;
    if (!ez::isEqual(result.w, static_cast<T>(4)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4_Multiplication() {
    ez::vec4<T> v(1, 2, 3, 4);
    ez::vec4<T> result = v * static_cast<T>(2);
    if (!ez::isEqual(result.x, static_cast<T>(2)))
        return false;
    if (!ez::isEqual(result.y, static_cast<T>(4)))
        return false;
    if (!ez::isEqual(result.z, static_cast<T>(6)))
        return false;
    if (!ez::isEqual(result.w, static_cast<T>(8)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4_Division() {
    ez::vec4<T> v(4, 8, 12, 16);
    ez::vec4<T> result = v / static_cast<T>(2);
    if (!ez::isEqual(result.x, static_cast<T>(2)))
        return false;
    if (!ez::isEqual(result.y, static_cast<T>(4)))
        return false;
    if (!ez::isEqual(result.z, static_cast<T>(6)))
        return false;
    if (!ez::isEqual(result.w, static_cast<T>(8)))
        return false;
    return true;
}

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzVec4(const std::string& vTest) {
    IfTestExist(TestEzVec4_Offset<float>);
    IfTestExist(TestEzVec4_Offset<double>);
    IfTestExist(TestEzVec4_Offset<int32_t>);
    IfTestExist(TestEzVec4_Offset<int64_t>);
    IfTestExist(TestEzVec4_Offset<uint32_t>);
    IfTestExist(TestEzVec4_Offset<uint64_t>);

    IfTestExist(TestEzVec4_Set<float>);
    IfTestExist(TestEzVec4_Set<double>);
    IfTestExist(TestEzVec4_Set<int32_t>);
    IfTestExist(TestEzVec4_Set<uint32_t>);
    IfTestExist(TestEzVec4_Set<int64_t>);
    IfTestExist(TestEzVec4_Set<uint64_t>);

    IfTestExist(TestEzVec4_Negate<float>);
    IfTestExist(TestEzVec4_Negate<double>);

    IfTestExist(TestEzVec4_LogicalNot<int32_t>);
    IfTestExist(TestEzVec4_LogicalNot<int64_t>);

    IfTestExist(TestEzVec4_XY<float>);
    IfTestExist(TestEzVec4_XY<double>);
    IfTestExist(TestEzVec4_XY<int32_t>);
    IfTestExist(TestEzVec4_XY<uint32_t>);
    IfTestExist(TestEzVec4_XY<int64_t>);
    IfTestExist(TestEzVec4_XY<uint64_t>);

    IfTestExist(TestEzVec4_XYZ<float>);
    IfTestExist(TestEzVec4_XYZ<double>);
    IfTestExist(TestEzVec4_XYZ<int32_t>);
    IfTestExist(TestEzVec4_XYZ<uint32_t>);
    IfTestExist(TestEzVec4_XYZ<int64_t>);
    IfTestExist(TestEzVec4_XYZ<uint64_t>);

    IfTestExist(TestEzVec4_ZW<float>);
    IfTestExist(TestEzVec4_ZW<double>);
    IfTestExist(TestEzVec4_ZW<int32_t>);
    IfTestExist(TestEzVec4_ZW<uint32_t>);
    IfTestExist(TestEzVec4_ZW<int64_t>);
    IfTestExist(TestEzVec4_ZW<uint64_t>);

    IfTestExist(TestEzVec4_Increment<float>);
    IfTestExist(TestEzVec4_Increment<double>);
    IfTestExist(TestEzVec4_Increment<int32_t>);
    IfTestExist(TestEzVec4_Increment<uint32_t>);
    IfTestExist(TestEzVec4_Increment<int64_t>);
    IfTestExist(TestEzVec4_Increment<uint64_t>);

    IfTestExist(TestEzVec4_Decrement<float>);
    IfTestExist(TestEzVec4_Decrement<double>);
    IfTestExist(TestEzVec4_Decrement<int32_t>);
    IfTestExist(TestEzVec4_Decrement<uint32_t>);
    IfTestExist(TestEzVec4_Decrement<int64_t>);
    IfTestExist(TestEzVec4_Decrement<uint64_t>);

    IfTestExist(TestEzVec4_Length<float>);
    IfTestExist(TestEzVec4_Length<double>);

    IfTestExist(TestEzVec4_Normalize<float>);
    IfTestExist(TestEzVec4_Normalize<double>);

    IfTestExist(TestEzVec4_Sum<float>);
    IfTestExist(TestEzVec4_Sum<double>);
    IfTestExist(TestEzVec4_Sum<int32_t>);
    IfTestExist(TestEzVec4_Sum<uint32_t>);
    IfTestExist(TestEzVec4_Sum<int64_t>);
    IfTestExist(TestEzVec4_Sum<uint64_t>);

    IfTestExist(TestEzVec4_SumAbs<float>);
    IfTestExist(TestEzVec4_SumAbs<double>);
    IfTestExist(TestEzVec4_SumAbs<int32_t>);
    IfTestExist(TestEzVec4_SumAbs<int64_t>);

    IfTestExist(TestEzVec4_EmptyAND<float>);
    IfTestExist(TestEzVec4_EmptyAND<double>);
    IfTestExist(TestEzVec4_EmptyAND<int32_t>);
    IfTestExist(TestEzVec4_EmptyAND<uint32_t>);
    IfTestExist(TestEzVec4_EmptyAND<int64_t>);
    IfTestExist(TestEzVec4_EmptyAND<uint64_t>);

    IfTestExist(TestEzVec4_EmptyOR<float>);
    IfTestExist(TestEzVec4_EmptyOR<double>);
    IfTestExist(TestEzVec4_EmptyOR<int32_t>);
    IfTestExist(TestEzVec4_EmptyOR<uint32_t>);
    IfTestExist(TestEzVec4_EmptyOR<int64_t>);
    IfTestExist(TestEzVec4_EmptyOR<uint64_t>);

    IfTestExist(TestEzVec4_String<float>);
    IfTestExist(TestEzVec4_String<double>);
    IfTestExist(TestEzVec4_String<int32_t>);
    IfTestExist(TestEzVec4_String<uint32_t>);
    IfTestExist(TestEzVec4_String<int64_t>);
    IfTestExist(TestEzVec4_String<uint64_t>);

    IfTestExist(TestEzVec4_Mini<float>);
    IfTestExist(TestEzVec4_Mini<double>);
    IfTestExist(TestEzVec4_Mini<int32_t>);
    IfTestExist(TestEzVec4_Mini<uint32_t>);
    IfTestExist(TestEzVec4_Mini<int64_t>);
    IfTestExist(TestEzVec4_Mini<uint64_t>);

    IfTestExist(TestEzVec4_Maxi<float>);
    IfTestExist(TestEzVec4_Maxi<double>);
    IfTestExist(TestEzVec4_Maxi<int32_t>);
    IfTestExist(TestEzVec4_Maxi<uint32_t>);
    IfTestExist(TestEzVec4_Maxi<int64_t>);
    IfTestExist(TestEzVec4_Maxi<uint64_t>);

    IfTestExist(TestEzVec4_Equality<float>);
    IfTestExist(TestEzVec4_Equality<double>);
    IfTestExist(TestEzVec4_Equality<int32_t>);
    IfTestExist(TestEzVec4_Equality<uint32_t>);
    IfTestExist(TestEzVec4_Equality<int64_t>);
    IfTestExist(TestEzVec4_Equality<uint64_t>);

    IfTestExist(TestEzVec4_Addition<float>);
    IfTestExist(TestEzVec4_Addition<double>);
    IfTestExist(TestEzVec4_Addition<int32_t>);
    IfTestExist(TestEzVec4_Addition<uint32_t>);
    IfTestExist(TestEzVec4_Addition<int64_t>);
    IfTestExist(TestEzVec4_Addition<uint64_t>);

    IfTestExist(TestEzVec4_Subtraction<float>);
    IfTestExist(TestEzVec4_Subtraction<double>);
    IfTestExist(TestEzVec4_Subtraction<int32_t>);
    IfTestExist(TestEzVec4_Subtraction<uint32_t>);
    IfTestExist(TestEzVec4_Subtraction<int64_t>);
    IfTestExist(TestEzVec4_Subtraction<uint64_t>);

    IfTestExist(TestEzVec4_Multiplication<float>);
    IfTestExist(TestEzVec4_Multiplication<double>);
    IfTestExist(TestEzVec4_Multiplication<int32_t>);
    IfTestExist(TestEzVec4_Multiplication<uint32_t>);
    IfTestExist(TestEzVec4_Multiplication<int64_t>);
    IfTestExist(TestEzVec4_Multiplication<uint64_t>);

    IfTestExist(TestEzVec4_Division<float>);
    IfTestExist(TestEzVec4_Division<double>);
    IfTestExist(TestEzVec4_Division<int32_t>);
    IfTestExist(TestEzVec4_Division<uint32_t>);
    IfTestExist(TestEzVec4_Division<int64_t>);
    IfTestExist(TestEzVec4_Division<uint64_t>);

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
