#include <ezlibs/ezMath.hpp>
#include <cmath>
#include <limits>

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
bool TestEzVec2_Offset() {
    ez::vec2<T> v(1, 2);
    ez::vec2<T> result = v.Offset(1, 2);
    if (!ez::isEqual(result.x, static_cast<T>(2)))
        return false;
    if (!ez::isEqual(result.y, static_cast<T>(4)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2_Negate() {
    ez::vec2<T> v(1, 2);
    ez::vec2<T> result = -v;
    if (!ez::isEqual(result.x, static_cast<T>(-1)))
        return false;
    if (!ez::isEqual(result.y, static_cast<T>(-2)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2_Length() {
    ez::vec2<T> v(3, 4);
    if (ez::isDifferent(v.length(), static_cast<T>(5)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2_Normalize() {
    ez::vec2<T> v(3, 4);
    v.normalize();
    if (ez::isDifferent(v.length(), static_cast<T>(1)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2_Sum() {
    ez::vec2<T> v(1, 2);
    if (!ez::isEqual(v.sum(), static_cast<T>(3)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2_OperatorAdd() {
    ez::vec2<T> v1(1, 2);
    ez::vec2<T> v2(3, 4);
    T scalar = 1;

    ez::vec2<T> result1 = v1 + scalar;
    if (!ez::isEqual(result1.x, static_cast<T>(2)))
        return false;
    if (!ez::isEqual(result1.y, static_cast<T>(3)))
        return false;

    ez::vec2<T> result2 = scalar + v1;
    if (!ez::isEqual(result2.x, static_cast<T>(2)))
        return false;
    if (!ez::isEqual(result2.y, static_cast<T>(3)))
        return false;

    ez::vec2<T> result3 = v1 + v2;
    if (!ez::isEqual(result3.x, static_cast<T>(4)))
        return false;
    if (!ez::isEqual(result3.y, static_cast<T>(6)))
        return false;

    return true;
}

template <typename T>
bool TestEzVec2_OperatorSubtract() {
    ez::vec2<T> v1(3, 4);
    ez::vec2<T> v2(1, 2);
    T scalar = 1;

    ez::vec2<T> result1 = v1 - scalar;
    if (!ez::isEqual(result1.x, static_cast<T>(2)))
        return false;
    if (!ez::isEqual(result1.y, static_cast<T>(3)))
        return false;

    ez::vec2<T> result2 = scalar - v1;
    if (!ez::isEqual(result2.x, static_cast<T>(-2)))
        return false;
    if (!ez::isEqual(result2.y, static_cast<T>(-3)))
        return false;

    ez::vec2<T> result3 = v1 - v2;
    if (!ez::isEqual(result3.x, static_cast<T>(2)))
        return false;
    if (!ez::isEqual(result3.y, static_cast<T>(2)))
        return false;

    return true;
}

template <typename T>
bool TestEzVec2_OperatorMultiply() {
    ez::vec2<T> v1(2, 3);
    ez::vec2<T> v2(4, 5);
    T scalar = 2;

    ez::vec2<T> result1 = v1 * scalar;
    if (!ez::isEqual(result1.x, static_cast<T>(4)))
        return false;
    if (!ez::isEqual(result1.y, static_cast<T>(6)))
        return false;

    ez::vec2<T> result2 = scalar * v1;
    if (!ez::isEqual(result2.x, static_cast<T>(4)))
        return false;
    if (!ez::isEqual(result2.y, static_cast<T>(6)))
        return false;

    ez::vec2<T> result3 = v1 * v2;
    if (!ez::isEqual(result3.x, static_cast<T>(8)))
        return false;
    if (!ez::isEqual(result3.y, static_cast<T>(15)))
        return false;

    return true;
}

template <typename T>
bool TestEzVec2_OperatorDivide() {
    ez::vec2<T> v1(4, 6);
    ez::vec2<T> v2(2, 3);
    T scalar = 2;

    ez::vec2<T> result1 = v1 / scalar;
    if (!ez::isEqual(result1.x, static_cast<T>(2)))
        return false;
    if (!ez::isEqual(result1.y, static_cast<T>(3)))
        return false;

    ez::vec2<T> result3 = v1 / v2;
    if (!ez::isEqual(result3.x, static_cast<T>(2)))
        return false;
    if (!ez::isEqual(result3.y, static_cast<T>(2)))
        return false;

    return true;
}

template <>
bool TestEzVec2_OperatorDivide<float>() {
    ez::vec2<float> v1(4.0f, 6.0f);
    ez::vec2<float> v2(2.0f, 3.0f);
    float scalar = 2.0f;

    ez::vec2<float> result2 = scalar / v1;
    if (!ez::isEqual(result2.x, 0.5f))
        return false;
    if (ez::isDifferent(result2.y, 1.0f / 3.0f))
        return false;

    return true;
}

template <>
bool TestEzVec2_OperatorDivide<double>() {
    ez::vec2<double> v1(4.0, 6.0);
    ez::vec2<double> v2(2.0, 3.0);
    double scalar = 2.0;

    ez::vec2<double> result2 = scalar / v1;
    if (!ez::isEqual(result2.x, 0.5))
        return false;
    if (ez::isDifferent(result2.y, 1.0 / 3.0))
        return false;

    return true;
}

template <typename T>
bool TestEzVec2_EmptyAND() {
    ez::vec2<T> v1(0, 0);
    ez::vec2<T> v2(1, 0);
    ez::vec2<T> v3(0, 1);
    ez::vec2<T> v4(1, 1);

    if (!v1.emptyAND())
        return false;
    if (v2.emptyAND())
        return false;
    if (v3.emptyAND())
        return false;
    if (v4.emptyAND())
        return false;

    return true;
}

template <typename T>
bool TestEzVec2_EmptyOR() {
    ez::vec2<T> v1(0, 0);
    ez::vec2<T> v2(1, 0);
    ez::vec2<T> v3(0, 1);
    ez::vec2<T> v4(1, 1);

    if (!v1.emptyOR())
        return false;
    if (!v2.emptyOR())
        return false;
    if (!v3.emptyOR())
        return false;
    if (v4.emptyOR())
        return false;

    return true;
}

template <typename T>
bool TestEzVec2_Min() {
    ez::vec2<T> v(1, 2);
    if (!ez::isEqual(v.min(), static_cast<T>(1)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2_Max() {
    ez::vec2<T> v(1, 2);
    if (!ez::isEqual(v.max(), static_cast<T>(2)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2_GetNormalized() {
    ez::vec2<T> v(3, 4);
    ez::vec2<T> normalized = v.GetNormalized();
    if (ez::isDifferent(normalized.length(), static_cast<T>(1)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2_ComparisonOperators() {
    ez::vec2<T> v1(1, 2);
    ez::vec2<T> v2(3, 4);
    ez::vec2<T> v3(1, 2);
    T scalar = 2;

    if (!(v1 < v2))
        return false;
    if (v2 < v1)
        return false;
    if (!(v1 < scalar))
        return false;
    if (v2 < scalar)
        return false;

    if (v1 > v2)
        return false;
    if (!(v2 > v1))
        return false;
    if (v1 > scalar)
        return false;
    if (!(v2 > scalar))
        return false;

    if (!(v1 <= v2))
        return false;
    if (!(v1 <= v3))
        return false;
    if (v2 <= v1)
        return false;
    if (!(v1 <= scalar))
        return false;

    if (v1 >= v2)
        return false;
    if (!(v2 >= v1))
        return false;
    if (!(v2 >= scalar))
        return false;

    if (!(v1.operator==(v3)))
        return false;
    if (v1.operator==(v2))
        return false;

    if (!(v1.operator!=(v2)))
        return false;
    if (v1.operator!=(v3))
        return false;

    return true;
}

template <typename T>
bool TestEzVec2_Floor() {
    ez::vec2<T> v(1.5, 2.7);
    ez::vec2<T> floorResult = ez::floor<T>(v);
    if (!ez::isEqual(floorResult.x, static_cast<T>(1)))
        return false;
    if (!ez::isEqual(floorResult.y, static_cast<T>(2)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2_Fract() {
    ez::vec2<T> v(1.5, 2.7);
    ez::vec2<T> fractResult = ez::fract<T>(v);
    if (ez::isDifferent(fractResult.x, static_cast<T>(0.5)))
        return false;
    if (ez::isDifferent(fractResult.y, static_cast<T>(0.7)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2_Ceil() {
    ez::vec2<T> v(1.5, 2.7);
    ez::vec2<T> ceilResult = ez::ceil<T>(v);
    if (!ez::isEqual(ceilResult.x, static_cast<T>(2)))
        return false;
    if (!ez::isEqual(ceilResult.y, static_cast<T>(3)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2_Mini() {
    ez::vec2<T> v1(1, 3);
    ez::vec2<T> v2(2, 4);
    ez::vec2<T> miniResult = ez::mini<T>(v1, v2);
    if (!ez::isEqual(miniResult.x, static_cast<T>(1)))
        return false;
    if (!ez::isEqual(miniResult.y, static_cast<T>(3)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2_Maxi() {
    ez::vec2<T> v1(1, 3);
    ez::vec2<T> v2(2, 4);
    ez::vec2<T> maxiResult = ez::maxi<T>(v1, v2);
    if (!ez::isEqual(maxiResult.x, static_cast<T>(2)))
        return false;
    if (!ez::isEqual(maxiResult.y, static_cast<T>(4)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2_Dot() {
    ez::vec2<T> v1(1, 3);
    ez::vec2<T> v2(2, 4);
    T dotResult = ez::dot<T>(v1, v2);
    if (!ez::isEqual(dotResult, static_cast<T>(14)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2_Det() {
    ez::vec2<T> v1(1, 3);
    ez::vec2<T> v2(2, 4);
    T detResult = ez::det<T>(v1, v2);
    if (!ez::isEqual(detResult, static_cast<T>(-2)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2_Reflect() {
    ez::vec2<T> v1(1, 3);
    ez::vec2<T> v2(2, 4);
    ez::vec2<T> reflectResult = ez::reflect<T>(v1, v2);
    if (ez::isDifferent(reflectResult.x, static_cast<T>(-55.0)))
        return false;
    if (ez::isDifferent(reflectResult.y, static_cast<T>(-109.0)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2_Sign() {
    ez::vec2<T> v(5, 3);
    ez::vec2<T> signResult = ez::sign<T>(v);
    if (!ez::isEqual(signResult.x, static_cast<T>(1)))
        return false;
    if (!ez::isEqual(signResult.y, static_cast<T>(1)))
        return false;

    v = ez::vec2<T>(0, 0);
    signResult = ez::sign(v);
    if (!ez::isEqual(signResult.x, static_cast<T>(0)))
        return false;
    if (!ez::isEqual(signResult.y, static_cast<T>(0)))
        return false;

    return true;
}

template <typename T>
bool TestEzVec2_Sin() {
    ez::vec2<T> v(1.5, 2.7);
    ez::vec2<T> sinResult = ez::sin<T>(v);
    if (ez::isDifferent(sinResult.x, std::sin(static_cast<T>(1.5))))
        return false;
    if (ez::isDifferent(sinResult.y, std::sin(static_cast<T>(2.7))))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2_Cos() {
    ez::vec2<T> v(1.5, 2.7);
    ez::vec2<T> cosResult = ez::cos<T>(v);
    if (ez::isDifferent(cosResult.x, std::cos(static_cast<T>(1.5))))
        return false;
    if (ez::isDifferent(cosResult.y, std::cos(static_cast<T>(2.7))))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2_Tan() {
    ez::vec2<T> v(1.5, 2.7);
    ez::vec2<T> tanResult = ez::tan<T>(v);
    if (ez::isDifferent(tanResult.x, std::tan(static_cast<T>(1.5))))
        return false;
    if (ez::isDifferent(tanResult.y, std::tan(static_cast<T>(2.7))))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2_Atan() {
    ez::vec2<T> v(1.5, 2.7);
    ez::vec2<T> atanResult = ez::atan<T>(v);
    if (ez::isDifferent(atanResult.x, std::atan(static_cast<T>(1.5))))
        return false;
    if (ez::isDifferent(atanResult.y, std::atan(static_cast<T>(2.7))))
        return false;
    return true;
}

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzVec2(const std::string& vTest) {
    IfTestExist(TestEzVec2_Offset<float>);
    else IfTestExist(TestEzVec2_Offset<double>);
    else IfTestExist(TestEzVec2_Offset<int32_t>);
    else IfTestExist(TestEzVec2_Offset<int64_t>);
    else IfTestExist(TestEzVec2_Offset<uint32_t>);
    else IfTestExist(TestEzVec2_Offset<uint64_t>);

    IfTestExist(TestEzVec2_Negate<float>);
    else IfTestExist(TestEzVec2_Negate<double>);
    else IfTestExist(TestEzVec2_Negate<int32_t>);
    else IfTestExist(TestEzVec2_Negate<int64_t>);

    IfTestExist(TestEzVec2_Length<float>);
    else IfTestExist(TestEzVec2_Length<double>);

    IfTestExist(TestEzVec2_Normalize<float>);
    else IfTestExist(TestEzVec2_Normalize<double>);

    IfTestExist(TestEzVec2_Sum<float>);
    else IfTestExist(TestEzVec2_Sum<double>);
    else IfTestExist(TestEzVec2_Sum<int32_t>);
    else IfTestExist(TestEzVec2_Sum<int64_t>);
    else IfTestExist(TestEzVec2_Sum<uint32_t>);
    else IfTestExist(TestEzVec2_Sum<uint64_t>);

    IfTestExist(TestEzVec2_OperatorAdd<float>);
    else IfTestExist(TestEzVec2_OperatorAdd<double>);
    else IfTestExist(TestEzVec2_OperatorAdd<int32_t>);
    else IfTestExist(TestEzVec2_OperatorAdd<int64_t>);
    else IfTestExist(TestEzVec2_OperatorAdd<uint32_t>);
    else IfTestExist(TestEzVec2_OperatorAdd<uint64_t>);

    IfTestExist(TestEzVec2_OperatorSubtract<float>);
    else IfTestExist(TestEzVec2_OperatorSubtract<double>);
    else IfTestExist(TestEzVec2_OperatorSubtract<int32_t>);
    else IfTestExist(TestEzVec2_OperatorSubtract<int64_t>);
    else IfTestExist(TestEzVec2_OperatorSubtract<uint32_t>);
    else IfTestExist(TestEzVec2_OperatorSubtract<uint64_t>);

    IfTestExist(TestEzVec2_OperatorMultiply<float>);
    else IfTestExist(TestEzVec2_OperatorMultiply<double>);
    else IfTestExist(TestEzVec2_OperatorMultiply<int32_t>);
    else IfTestExist(TestEzVec2_OperatorMultiply<int64_t>);
    else IfTestExist(TestEzVec2_OperatorMultiply<uint32_t>);
    else IfTestExist(TestEzVec2_OperatorMultiply<uint64_t>);

    IfTestExist(TestEzVec2_OperatorDivide<float>);
    else IfTestExist(TestEzVec2_OperatorDivide<double>);
    else IfTestExist(TestEzVec2_OperatorDivide<int32_t>);
    else IfTestExist(TestEzVec2_OperatorDivide<int64_t>);
    else IfTestExist(TestEzVec2_OperatorDivide<uint32_t>);
    else IfTestExist(TestEzVec2_OperatorDivide<uint64_t>);

    IfTestExist(TestEzVec2_EmptyAND<float>);
    else IfTestExist(TestEzVec2_EmptyAND<double>);
    else IfTestExist(TestEzVec2_EmptyAND<int32_t>);
    else IfTestExist(TestEzVec2_EmptyAND<int64_t>);
    else IfTestExist(TestEzVec2_EmptyAND<uint32_t>);
    else IfTestExist(TestEzVec2_EmptyAND<uint64_t>);

    IfTestExist(TestEzVec2_EmptyOR<float>);
    else IfTestExist(TestEzVec2_EmptyOR<double>);
    else IfTestExist(TestEzVec2_EmptyOR<int32_t>);
    else IfTestExist(TestEzVec2_EmptyOR<int64_t>);
    else IfTestExist(TestEzVec2_EmptyOR<uint32_t>);
    else IfTestExist(TestEzVec2_EmptyOR<uint64_t>);

    IfTestExist(TestEzVec2_Min<float>);
    else IfTestExist(TestEzVec2_Min<double>);
    else IfTestExist(TestEzVec2_Min<int32_t>);
    else IfTestExist(TestEzVec2_Min<int64_t>);
    else IfTestExist(TestEzVec2_Min<uint32_t>);
    else IfTestExist(TestEzVec2_Min<uint64_t>);

    IfTestExist(TestEzVec2_Max<float>);
    else IfTestExist(TestEzVec2_Max<double>);
    else IfTestExist(TestEzVec2_Max<int32_t>);
    else IfTestExist(TestEzVec2_Max<int64_t>);
    else IfTestExist(TestEzVec2_Max<uint32_t>);
    else IfTestExist(TestEzVec2_Max<uint64_t>);

    IfTestExist(TestEzVec2_GetNormalized<float>);
    else IfTestExist(TestEzVec2_GetNormalized<double>);

    IfTestExist(TestEzVec2_Floor<float>);
    else IfTestExist(TestEzVec2_Floor<double>);

    IfTestExist(TestEzVec2_Fract<float>);
    else IfTestExist(TestEzVec2_Fract<double>);

    IfTestExist(TestEzVec2_Ceil<float>);
    else IfTestExist(TestEzVec2_Ceil<double>);

    IfTestExist(TestEzVec2_Mini<float>);
    else IfTestExist(TestEzVec2_Mini<double>);
    else IfTestExist(TestEzVec2_Mini<int32_t>);
    else IfTestExist(TestEzVec2_Mini<int64_t>);
    else IfTestExist(TestEzVec2_Mini<uint32_t>);
    else IfTestExist(TestEzVec2_Mini<uint64_t>);

    IfTestExist(TestEzVec2_Maxi<float>);
    else IfTestExist(TestEzVec2_Maxi<double>);
    else IfTestExist(TestEzVec2_Maxi<int32_t>);
    else IfTestExist(TestEzVec2_Maxi<int64_t>);
    else IfTestExist(TestEzVec2_Maxi<uint32_t>);
    else IfTestExist(TestEzVec2_Maxi<uint64_t>);

    IfTestExist(TestEzVec2_Dot<float>);
    else IfTestExist(TestEzVec2_Dot<double>);
    else IfTestExist(TestEzVec2_Dot<int32_t>);
    else IfTestExist(TestEzVec2_Dot<int64_t>);
    else IfTestExist(TestEzVec2_Dot<uint32_t>);
    else IfTestExist(TestEzVec2_Dot<uint64_t>);

    IfTestExist(TestEzVec2_Det<float>);
    else IfTestExist(TestEzVec2_Det<double>);
    else IfTestExist(TestEzVec2_Det<int32_t>);
    else IfTestExist(TestEzVec2_Det<int64_t>);
    else IfTestExist(TestEzVec2_Det<uint32_t>);
    else IfTestExist(TestEzVec2_Det<uint64_t>);

    IfTestExist(TestEzVec2_Reflect<float>);
    else IfTestExist(TestEzVec2_Reflect<double>);

    IfTestExist(TestEzVec2_Sign<float>);
    else IfTestExist(TestEzVec2_Sign<double>);
    else IfTestExist(TestEzVec2_Sign<int32_t>);
    else IfTestExist(TestEzVec2_Sign<int64_t>);

    IfTestExist(TestEzVec2_Sin<float>);
    else IfTestExist(TestEzVec2_Sin<double>);

    IfTestExist(TestEzVec2_Cos<float>);
    else IfTestExist(TestEzVec2_Cos<double>);

    IfTestExist(TestEzVec2_Tan<float>);
    else IfTestExist(TestEzVec2_Tan<double>);

    IfTestExist(TestEzVec2_Atan<float>);
    else IfTestExist(TestEzVec2_Atan<double>);

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
