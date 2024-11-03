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
bool TestEzVec2Offset() {
    ez::vec2<T> v(1, 2);
    ez::vec2<T> result = v.Offset(1, 2);
    if (!ez::isEqual(result.x, static_cast<T>(2)))
        return false;
    if (!ez::isEqual(result.y, static_cast<T>(4)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2Negate() {
    ez::vec2<T> v(1, 2);
    ez::vec2<T> result = -v;
    if (!ez::isEqual(result.x, static_cast<T>(-1)))
        return false;
    if (!ez::isEqual(result.y, static_cast<T>(-2)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2Length() {
    ez::vec2<T> v(3, 4);
    if (ez::isDifferent(v.length(), static_cast<T>(5)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2Normalize() {
    ez::vec2<T> v(3, 4);
    v.normalize();
    if (ez::isDifferent(v.length(), static_cast<T>(1)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2Sum() {
    ez::vec2<T> v(1, 2);
    if (!ez::isEqual(v.sum(), static_cast<T>(3)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2OperatorAdd() {
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
bool TestEzVec2OperatorSubtract() {
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
bool TestEzVec2OperatorMultiply() {
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
bool TestEzVec2OperatorDivide() {
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
bool TestEzVec2OperatorDivide<float>() {
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
bool TestEzVec2OperatorDivide<double>() {
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
bool TestEzVec2EmptyAND() {
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
bool TestEzVec2EmptyOR() {
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
bool TestEzVec2Min() {
    ez::vec2<T> v(1, 2);
    if (!ez::isEqual(v.min(), static_cast<T>(1)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2Max() {
    ez::vec2<T> v(1, 2);
    if (!ez::isEqual(v.max(), static_cast<T>(2)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2GetNormalized() {
    ez::vec2<T> v(3, 4);
    ez::vec2<T> normalized = v.GetNormalized();
    if (ez::isDifferent(normalized.length(), static_cast<T>(1)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2ComparisonOperators() {
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
bool TestEzVec2Floor() {
    ez::vec2<T> v(1.5, 2.7);
    ez::vec2<T> floorResult = ez::floor<T>(v);
    if (!ez::isEqual(floorResult.x, static_cast<T>(1)))
        return false;
    if (!ez::isEqual(floorResult.y, static_cast<T>(2)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2Fract() {
    ez::vec2<T> v(1.5, 2.7);
    ez::vec2<T> fractResult = ez::fract<T>(v);
    if (ez::isDifferent(fractResult.x, static_cast<T>(0.5)))
        return false;
    if (ez::isDifferent(fractResult.y, static_cast<T>(0.7)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2Ceil() {
    ez::vec2<T> v(1.5, 2.7);
    ez::vec2<T> ceilResult = ez::ceil<T>(v);
    if (!ez::isEqual(ceilResult.x, static_cast<T>(2)))
        return false;
    if (!ez::isEqual(ceilResult.y, static_cast<T>(3)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2Mini() {
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
bool TestEzVec2Maxi() {
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
bool TestEzVec2Dot() {
    ez::vec2<T> v1(1, 3);
    ez::vec2<T> v2(2, 4);
    T dotResult = ez::dot<T>(v1, v2);
    if (!ez::isEqual(dotResult, static_cast<T>(14)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2Det() {
    ez::vec2<T> v1(1, 3);
    ez::vec2<T> v2(2, 4);
    T detResult = ez::det<T>(v1, v2);
    if (!ez::isEqual(detResult, static_cast<T>(-2)))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2Reflect() {
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
bool TestEzVec2Sign() {
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
bool TestEzVec2Sin() {
    ez::vec2<T> v(1.5, 2.7);
    ez::vec2<T> sinResult = ez::sin<T>(v);
    if (ez::isDifferent(sinResult.x, std::sin(static_cast<T>(1.5))))
        return false;
    if (ez::isDifferent(sinResult.y, std::sin(static_cast<T>(2.7))))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2Cos() {
    ez::vec2<T> v(1.5, 2.7);
    ez::vec2<T> cosResult = ez::cos<T>(v);
    if (ez::isDifferent(cosResult.x, std::cos(static_cast<T>(1.5))))
        return false;
    if (ez::isDifferent(cosResult.y, std::cos(static_cast<T>(2.7))))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2Tan() {
    ez::vec2<T> v(1.5, 2.7);
    ez::vec2<T> tanResult = ez::tan<T>(v);
    if (ez::isDifferent(tanResult.x, std::tan(static_cast<T>(1.5))))
        return false;
    if (ez::isDifferent(tanResult.y, std::tan(static_cast<T>(2.7))))
        return false;
    return true;
}

template <typename T>
bool TestEzVec2Atan() {
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
    IfTestExist(TestEzVec2Offset<float>);
    else IfTestExist(TestEzVec2Offset<double>);
    else IfTestExist(TestEzVec2Offset<int32_t>);
    else IfTestExist(TestEzVec2Offset<int64_t>);
    else IfTestExist(TestEzVec2Offset<uint32_t>);
    else IfTestExist(TestEzVec2Offset<uint64_t>);

    IfTestExist(TestEzVec2Negate<float>);
    else IfTestExist(TestEzVec2Negate<double>);
    else IfTestExist(TestEzVec2Negate<int32_t>);
    else IfTestExist(TestEzVec2Negate<int64_t>);

    IfTestExist(TestEzVec2Length<float>);
    else IfTestExist(TestEzVec2Length<double>);

    IfTestExist(TestEzVec2Normalize<float>);
    else IfTestExist(TestEzVec2Normalize<double>);

    IfTestExist(TestEzVec2Sum<float>);
    else IfTestExist(TestEzVec2Sum<double>);
    else IfTestExist(TestEzVec2Sum<int32_t>);
    else IfTestExist(TestEzVec2Sum<int64_t>);
    else IfTestExist(TestEzVec2Sum<uint32_t>);
    else IfTestExist(TestEzVec2Sum<uint64_t>);

    IfTestExist(TestEzVec2OperatorAdd<float>);
    else IfTestExist(TestEzVec2OperatorAdd<double>);
    else IfTestExist(TestEzVec2OperatorAdd<int32_t>);
    else IfTestExist(TestEzVec2OperatorAdd<int64_t>);
    else IfTestExist(TestEzVec2OperatorAdd<uint32_t>);
    else IfTestExist(TestEzVec2OperatorAdd<uint64_t>);

    IfTestExist(TestEzVec2OperatorSubtract<float>);
    else IfTestExist(TestEzVec2OperatorSubtract<double>);
    else IfTestExist(TestEzVec2OperatorSubtract<int32_t>);
    else IfTestExist(TestEzVec2OperatorSubtract<int64_t>);
    else IfTestExist(TestEzVec2OperatorSubtract<uint32_t>);
    else IfTestExist(TestEzVec2OperatorSubtract<uint64_t>);

    IfTestExist(TestEzVec2OperatorMultiply<float>);
    else IfTestExist(TestEzVec2OperatorMultiply<double>);
    else IfTestExist(TestEzVec2OperatorMultiply<int32_t>);
    else IfTestExist(TestEzVec2OperatorMultiply<int64_t>);
    else IfTestExist(TestEzVec2OperatorMultiply<uint32_t>);
    else IfTestExist(TestEzVec2OperatorMultiply<uint64_t>);

    IfTestExist(TestEzVec2OperatorDivide<float>);
    else IfTestExist(TestEzVec2OperatorDivide<double>);
    else IfTestExist(TestEzVec2OperatorDivide<int32_t>);
    else IfTestExist(TestEzVec2OperatorDivide<int64_t>);
    else IfTestExist(TestEzVec2OperatorDivide<uint32_t>);
    else IfTestExist(TestEzVec2OperatorDivide<uint64_t>);

    IfTestExist(TestEzVec2EmptyAND<float>);
    else IfTestExist(TestEzVec2EmptyAND<double>);
    else IfTestExist(TestEzVec2EmptyAND<int32_t>);
    else IfTestExist(TestEzVec2EmptyAND<int64_t>);
    else IfTestExist(TestEzVec2EmptyAND<uint32_t>);
    else IfTestExist(TestEzVec2EmptyAND<uint64_t>);

    IfTestExist(TestEzVec2EmptyOR<float>);
    else IfTestExist(TestEzVec2EmptyOR<double>);
    else IfTestExist(TestEzVec2EmptyOR<int32_t>);
    else IfTestExist(TestEzVec2EmptyOR<int64_t>);
    else IfTestExist(TestEzVec2EmptyOR<uint32_t>);
    else IfTestExist(TestEzVec2EmptyOR<uint64_t>);

    IfTestExist(TestEzVec2Min<float>);
    else IfTestExist(TestEzVec2Min<double>);
    else IfTestExist(TestEzVec2Min<int32_t>);
    else IfTestExist(TestEzVec2Min<int64_t>);
    else IfTestExist(TestEzVec2Min<uint32_t>);
    else IfTestExist(TestEzVec2Min<uint64_t>);

    IfTestExist(TestEzVec2Max<float>);
    else IfTestExist(TestEzVec2Max<double>);
    else IfTestExist(TestEzVec2Max<int32_t>);
    else IfTestExist(TestEzVec2Max<int64_t>);
    else IfTestExist(TestEzVec2Max<uint32_t>);
    else IfTestExist(TestEzVec2Max<uint64_t>);

    IfTestExist(TestEzVec2GetNormalized<float>);
    else IfTestExist(TestEzVec2GetNormalized<double>);

    IfTestExist(TestEzVec2Floor<float>);
    else IfTestExist(TestEzVec2Floor<double>);

    IfTestExist(TestEzVec2Fract<float>);
    else IfTestExist(TestEzVec2Fract<double>);

    IfTestExist(TestEzVec2Ceil<float>);
    else IfTestExist(TestEzVec2Ceil<double>);

    IfTestExist(TestEzVec2Mini<float>);
    else IfTestExist(TestEzVec2Mini<double>);
    else IfTestExist(TestEzVec2Mini<int32_t>);
    else IfTestExist(TestEzVec2Mini<int64_t>);
    else IfTestExist(TestEzVec2Mini<uint32_t>);
    else IfTestExist(TestEzVec2Mini<uint64_t>);

    IfTestExist(TestEzVec2Maxi<float>);
    else IfTestExist(TestEzVec2Maxi<double>);
    else IfTestExist(TestEzVec2Maxi<int32_t>);
    else IfTestExist(TestEzVec2Maxi<int64_t>);
    else IfTestExist(TestEzVec2Maxi<uint32_t>);
    else IfTestExist(TestEzVec2Maxi<uint64_t>);

    IfTestExist(TestEzVec2Dot<float>);
    else IfTestExist(TestEzVec2Dot<double>);
    else IfTestExist(TestEzVec2Dot<int32_t>);
    else IfTestExist(TestEzVec2Dot<int64_t>);
    else IfTestExist(TestEzVec2Dot<uint32_t>);
    else IfTestExist(TestEzVec2Dot<uint64_t>);

    IfTestExist(TestEzVec2Det<float>);
    else IfTestExist(TestEzVec2Det<double>);
    else IfTestExist(TestEzVec2Det<int32_t>);
    else IfTestExist(TestEzVec2Det<int64_t>);
    else IfTestExist(TestEzVec2Det<uint32_t>);
    else IfTestExist(TestEzVec2Det<uint64_t>);

    IfTestExist(TestEzVec2Reflect<float>);
    else IfTestExist(TestEzVec2Reflect<double>);

    IfTestExist(TestEzVec2Sign<float>);
    else IfTestExist(TestEzVec2Sign<double>);
    else IfTestExist(TestEzVec2Sign<int32_t>);
    else IfTestExist(TestEzVec2Sign<int64_t>);

    IfTestExist(TestEzVec2Sin<float>);
    else IfTestExist(TestEzVec2Sin<double>);

    IfTestExist(TestEzVec2Cos<float>);
    else IfTestExist(TestEzVec2Cos<double>);

    IfTestExist(TestEzVec2Tan<float>);
    else IfTestExist(TestEzVec2Tan<double>);

    IfTestExist(TestEzVec2Atan<float>);
    else IfTestExist(TestEzVec2Atan<double>);

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
