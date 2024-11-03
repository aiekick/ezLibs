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
bool TestEzVec4Offset() {
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
bool TestEzVec4Set() {
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
bool TestEzVec4Negate() {
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
bool TestEzVec4LogicalNot() {
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
bool TestEzVec4XY() {
    ez::vec4<T> v(1, 2, 3, 4);
    ez::vec2<T> xyResult = v.xy();
    if (!ez::isEqual(xyResult.x, static_cast<T>(1)))
        return false;
    if (!ez::isEqual(xyResult.y, static_cast<T>(2)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4XYZ() {
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
bool TestEzVec4ZW() {
    ez::vec4<T> v(1, 2, 3, 4);
    ez::vec2<T> zwResult = v.zw();
    if (!ez::isEqual(zwResult.x, static_cast<T>(3)))
        return false;
    if (!ez::isEqual(zwResult.y, static_cast<T>(4)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4Increment() {
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
bool TestEzVec4Decrement() {
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
bool TestEzVec4Length() {
    ez::vec4<T> v(1, 2, 3, 4);
    if (!ez::isEqual(v.length(), static_cast<T>(5.4772255750516612)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4Normalize() {
    ez::vec4<T> v(1, 2, 2, 1);
    v.normalize();
    if (ez::isDifferent(v.length(), static_cast<T>(1.0)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4Sum() {
    ez::vec4<T> v(1, 2, 3, 4);
    T sum = v.sum();
    if (!ez::isEqual(sum, static_cast<T>(10)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4SumAbs() {
    ez::vec4<T> v(1, -2, 3, -4);
    T sumAbs = v.sumAbs();
    if (!ez::isEqual(sumAbs, static_cast<T>(10)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4EmptyAND() {
    ez::vec4<T> v;
    if (!v.emptyAND())
        return false;
    v.x = 1;
    if (v.emptyAND())
        return false;
    return true;
}

template <typename T>
bool TestEzVec4EmptyOR() {
    ez::vec4<T> v(0, 1, 1, 1);
    if (!v.emptyOR())
        return false;
    v.x = 1;
    if (v.emptyOR())
        return false;
    return true;
}

template <typename T>
bool TestEzVec4String() {
    ez::vec4<T> v(1, 2, 3, 4);
    const std::string str = v.string();
    if (str != "1;2;3;4")
        return false;
    return true;
}

template <typename T>
bool TestEzVec4Mini() {
   const ez::vec4<T> v(1, 2, 3, 4);
    T mini = v.mini();
    if (!ez::isEqual(mini, static_cast<T>(1)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4Maxi() {
    ez::vec4<T> v(1, 2, 3, 4);
    T maxi = v.maxi();
    if (!ez::isEqual(maxi, static_cast<T>(4)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec4Equality() {
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
bool TestEzVec4Equality<float>() {
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
bool TestEzVec4Addition() {
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
bool TestEzVec4Subtraction() {
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
bool TestEzVec4Multiplication() {
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
bool TestEzVec4Division() {
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
    IfTestExist(TestEzVec4Offset<float>);
    IfTestExist(TestEzVec4Offset<double>);
    IfTestExist(TestEzVec4Offset<int32_t>);
    IfTestExist(TestEzVec4Offset<int64_t>);
    IfTestExist(TestEzVec4Offset<uint32_t>);
    IfTestExist(TestEzVec4Offset<uint64_t>);

    IfTestExist(TestEzVec4Set<float>);
    IfTestExist(TestEzVec4Set<double>);
    IfTestExist(TestEzVec4Set<int32_t>);
    IfTestExist(TestEzVec4Set<uint32_t>);
    IfTestExist(TestEzVec4Set<int64_t>);
    IfTestExist(TestEzVec4Set<uint64_t>);

    IfTestExist(TestEzVec4Negate<float>);
    IfTestExist(TestEzVec4Negate<double>);

    IfTestExist(TestEzVec4LogicalNot<int32_t>);
    IfTestExist(TestEzVec4LogicalNot<int64_t>);

    IfTestExist(TestEzVec4XY<float>);
    IfTestExist(TestEzVec4XY<double>);
    IfTestExist(TestEzVec4XY<int32_t>);
    IfTestExist(TestEzVec4XY<uint32_t>);
    IfTestExist(TestEzVec4XY<int64_t>);
    IfTestExist(TestEzVec4XY<uint64_t>);

    IfTestExist(TestEzVec4XYZ<float>);
    IfTestExist(TestEzVec4XYZ<double>);
    IfTestExist(TestEzVec4XYZ<int32_t>);
    IfTestExist(TestEzVec4XYZ<uint32_t>);
    IfTestExist(TestEzVec4XYZ<int64_t>);
    IfTestExist(TestEzVec4XYZ<uint64_t>);

    IfTestExist(TestEzVec4ZW<float>);
    IfTestExist(TestEzVec4ZW<double>);
    IfTestExist(TestEzVec4ZW<int32_t>);
    IfTestExist(TestEzVec4ZW<uint32_t>);
    IfTestExist(TestEzVec4ZW<int64_t>);
    IfTestExist(TestEzVec4ZW<uint64_t>);

    IfTestExist(TestEzVec4Increment<float>);
    IfTestExist(TestEzVec4Increment<double>);
    IfTestExist(TestEzVec4Increment<int32_t>);
    IfTestExist(TestEzVec4Increment<uint32_t>);
    IfTestExist(TestEzVec4Increment<int64_t>);
    IfTestExist(TestEzVec4Increment<uint64_t>);

    IfTestExist(TestEzVec4Decrement<float>);
    IfTestExist(TestEzVec4Decrement<double>);
    IfTestExist(TestEzVec4Decrement<int32_t>);
    IfTestExist(TestEzVec4Decrement<uint32_t>);
    IfTestExist(TestEzVec4Decrement<int64_t>);
    IfTestExist(TestEzVec4Decrement<uint64_t>);

    IfTestExist(TestEzVec4Length<float>);
    IfTestExist(TestEzVec4Length<double>);

    IfTestExist(TestEzVec4Normalize<float>);
    IfTestExist(TestEzVec4Normalize<double>);

    IfTestExist(TestEzVec4Sum<float>);
    IfTestExist(TestEzVec4Sum<double>);
    IfTestExist(TestEzVec4Sum<int32_t>);
    IfTestExist(TestEzVec4Sum<uint32_t>);
    IfTestExist(TestEzVec4Sum<int64_t>);
    IfTestExist(TestEzVec4Sum<uint64_t>);

    IfTestExist(TestEzVec4SumAbs<float>);
    IfTestExist(TestEzVec4SumAbs<double>);
    IfTestExist(TestEzVec4SumAbs<int32_t>);
    IfTestExist(TestEzVec4SumAbs<int64_t>);

    IfTestExist(TestEzVec4EmptyAND<float>);
    IfTestExist(TestEzVec4EmptyAND<double>);
    IfTestExist(TestEzVec4EmptyAND<int32_t>);
    IfTestExist(TestEzVec4EmptyAND<uint32_t>);
    IfTestExist(TestEzVec4EmptyAND<int64_t>);
    IfTestExist(TestEzVec4EmptyAND<uint64_t>);

    IfTestExist(TestEzVec4EmptyOR<float>);
    IfTestExist(TestEzVec4EmptyOR<double>);
    IfTestExist(TestEzVec4EmptyOR<int32_t>);
    IfTestExist(TestEzVec4EmptyOR<uint32_t>);
    IfTestExist(TestEzVec4EmptyOR<int64_t>);
    IfTestExist(TestEzVec4EmptyOR<uint64_t>);

    IfTestExist(TestEzVec4String<float>);
    IfTestExist(TestEzVec4String<double>);
    IfTestExist(TestEzVec4String<int32_t>);
    IfTestExist(TestEzVec4String<uint32_t>);
    IfTestExist(TestEzVec4String<int64_t>);
    IfTestExist(TestEzVec4String<uint64_t>);

    IfTestExist(TestEzVec4Mini<float>);
    IfTestExist(TestEzVec4Mini<double>);
    IfTestExist(TestEzVec4Mini<int32_t>);
    IfTestExist(TestEzVec4Mini<uint32_t>);
    IfTestExist(TestEzVec4Mini<int64_t>);
    IfTestExist(TestEzVec4Mini<uint64_t>);

    IfTestExist(TestEzVec4Maxi<float>);
    IfTestExist(TestEzVec4Maxi<double>);
    IfTestExist(TestEzVec4Maxi<int32_t>);
    IfTestExist(TestEzVec4Maxi<uint32_t>);
    IfTestExist(TestEzVec4Maxi<int64_t>);
    IfTestExist(TestEzVec4Maxi<uint64_t>);

    IfTestExist(TestEzVec4Equality<float>);
    IfTestExist(TestEzVec4Equality<double>);
    IfTestExist(TestEzVec4Equality<int32_t>);
    IfTestExist(TestEzVec4Equality<uint32_t>);
    IfTestExist(TestEzVec4Equality<int64_t>);
    IfTestExist(TestEzVec4Equality<uint64_t>);

    IfTestExist(TestEzVec4Addition<float>);
    IfTestExist(TestEzVec4Addition<double>);
    IfTestExist(TestEzVec4Addition<int32_t>);
    IfTestExist(TestEzVec4Addition<uint32_t>);
    IfTestExist(TestEzVec4Addition<int64_t>);
    IfTestExist(TestEzVec4Addition<uint64_t>);

    IfTestExist(TestEzVec4Subtraction<float>);
    IfTestExist(TestEzVec4Subtraction<double>);
    IfTestExist(TestEzVec4Subtraction<int32_t>);
    IfTestExist(TestEzVec4Subtraction<uint32_t>);
    IfTestExist(TestEzVec4Subtraction<int64_t>);
    IfTestExist(TestEzVec4Subtraction<uint64_t>);

    IfTestExist(TestEzVec4Multiplication<float>);
    IfTestExist(TestEzVec4Multiplication<double>);
    IfTestExist(TestEzVec4Multiplication<int32_t>);
    IfTestExist(TestEzVec4Multiplication<uint32_t>);
    IfTestExist(TestEzVec4Multiplication<int64_t>);
    IfTestExist(TestEzVec4Multiplication<uint64_t>);

    IfTestExist(TestEzVec4Division<float>);
    IfTestExist(TestEzVec4Division<double>);
    IfTestExist(TestEzVec4Division<int32_t>);
    IfTestExist(TestEzVec4Division<uint32_t>);
    IfTestExist(TestEzVec4Division<int64_t>);
    IfTestExist(TestEzVec4Division<uint64_t>);

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
