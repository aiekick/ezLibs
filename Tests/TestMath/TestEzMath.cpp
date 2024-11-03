#include <TestEzMath.h>
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

bool TestEzMathRoundN() {
    if (ez::round_n(1.23456, 2) != "1.23")  // double
        return false;
    if (ez::round_n(1.23456f, 2) != "1.23")  // float
        return false;
    if (ez::round_n(1.23456, 4) != "1.2346")  // double
        return false;
    if (ez::round_n(1.23456f, 4) != "1.2346")  // float
        return false;
    if (ez::round_n(1.2, 1) != "1.2")  // double
        return false;
    if (ez::round_n(1.2f, 1) != "1.2")  // float
        return false;
    if (ez::round_n(1.2, 2) != "1.20")  // double
        return false;
    if (ez::round_n(1.2f, 2) != "1.20")  // float
        return false;
    if (ez::round_n(0.0000, 2) != "0.00")  // double
        return false;
    if (ez::round_n(0.0000f, 2) != "0.00")  // float
        return false;
    if (ez::round_n(0.0000, 0) != "0")  // double
        return false;
    if (ez::round_n(0.0000f, 0) != "0")  // float
        return false;
    return true;
}

bool TestEzMathFloatIsValid() {
    if (!ez::floatIsValid(1.0f))
        return false;
    if (!ez::floatIsValid(0.0f))
        return false;
    if (!ez::floatIsValid(-1.0f))
        return false;
    if (ez::floatIsValid(std::numeric_limits<float>::infinity()))
        return false;
    if (ez::floatIsValid(std::numeric_limits<float>::quiet_NaN()))
        return false;
    return true;
}

template <typename T>
bool TestEzMathIsDifferent() {
    if (!ez::isDifferent<T>(-5, 5))
        return false;
    if (!ez::isDifferent<T>(0, 5))
        return false;
    if (ez::isDifferent<T>(-0, 0))
        return false;
    if (!ez::isDifferent<T>(1, 1 + 1))
        return false;
    if (!ez::isDifferent<T>(1, 1 - 1))
        return false;
    return true;
}

template <>
bool TestEzMathIsDifferent<uint32_t>() {
    if (!ez::isDifferent<uint32_t>(0, 5))
        return false;
    if (ez::isDifferent<uint32_t>(-0, 0))
        return false;
    if (!ez::isDifferent<uint32_t>(1, 1 + 1))
        return false;
    if (!ez::isDifferent<uint32_t>(1, 1 - 1))
        return false;
    return true;
}

template <>
bool TestEzMathIsDifferent<uint64_t>() {
    if (!ez::isDifferent<uint64_t>(0, 5))
        return false;
    if (ez::isDifferent<uint64_t>(-0, 0))
        return false;
    if (!ez::isDifferent<uint64_t>(1, 1 + 1))
        return false;
    if (!ez::isDifferent<uint64_t>(1, 1 - 1))
        return false;
    return true;
}

template <>
bool TestEzMathIsDifferent<float>() {
    if (!ez::isDifferent<float>(-5.0f, 5.0f))
        return false;
    if (!ez::isDifferent<float>(0.0f, 5.0f))
        return false;
    if (ez::isDifferent<float>(-0.0f, 0.0f))
        return false;
    if (!ez::isDifferent<float>(1.0f, 1.0f + std::numeric_limits<float>::epsilon() * 2.0f))
        return false;
    if (!ez::isDifferent<float>(1.0f, 1.0f - std::numeric_limits<float>::epsilon() * 2.0f))
        return false;
    return true;
}

template <>
bool TestEzMathIsDifferent<double>() {
    if (!ez::isDifferent<double>(-5.0, 5.0))
        return false;
    if (!ez::isDifferent<double>(0.0, 5.0))
        return false;
    if (ez::isDifferent<double>(-0.0, 0.0))
        return false;
    if (!ez::isDifferent<double>(1.0, 1.0 + std::numeric_limits<double>::epsilon() * 2.0))
        return false;
    if (!ez::isDifferent<double>(1.0, 1.0 - std::numeric_limits<double>::epsilon() * 2.0))
        return false;
    return true;
}

template <typename T>
bool TestEzMathIsEqual() {
    if (ez::isEqual<T>(0, 5))
        return false;
    if (!ez::isEqual<T>(-0, 0))
        return false;
    if (ez::isEqual<T>(1, 1 + 1))
        return false;
    if (ez::isEqual<T>(1, 1 - 1))
        return false;
    return true;
}

template <int32_t>
bool TestEzMathIsEqual() {
    if (!ez::isEqual<int32_t>(-5, -5))
        return false;
    if (ez::isEqual<int32_t>(0, 5))
        return false;
    if (!ez::isEqual<int32_t>(-0, 0))
        return false;
    if (ez::isEqual<int32_t>(1, 1 + 1))
        return false;
    if (ez::isEqual<int32_t>(1, 1 - 1))
        return false;
    return true;
}

template <int64_t>
bool TestEzMathIsEqual() {
    if (!ez::isEqual<int64_t>(-5, -5))
        return false;
    if (ez::isEqual<int64_t>(0, 5))
        return false;
    if (!ez::isEqual<int64_t>(-0, 0))
        return false;
    if (ez::isEqual<int64_t>(1, 1 + 1))
        return false;
    if (ez::isEqual<int64_t>(1, 1 - 1))
        return false;
    return true;
}

template <>
bool TestEzMathIsEqual<float>() {
    if (!ez::isEqual<float>(-5.0f, -5.0f))
        return false;
    if (ez::isEqual<float>(0.0f, 5.0f))
        return false;
    if (!ez::isEqual<float>(-0.0f, 0.0f))
        return false;
    if (ez::isEqual<float>(1.0f, 1.0f + std::numeric_limits<float>::epsilon()))
        return false;
    if (ez::isEqual<float>(1.0f, 1.0f - std::numeric_limits<float>::epsilon()))
        return false;
    return true;
}

template <>
bool TestEzMathIsEqual<double>() {
    if (!ez::isEqual<double>(-5, -5))
        return false;
    if (ez::isEqual<double>(0, 5))
        return false;
    if (!ez::isEqual<double>(-0, 0))
        return false;
    if (ez::isEqual<double>(1, 1 + std::numeric_limits<double>::epsilon()))
        return false;
    if (ez::isEqual<double>(1, 1 - std::numeric_limits<double>::epsilon()))
        return false;
    return true;
}

template <typename T>
bool TestEzMathRound() {
    if (ez::isDifferent(ez::round<T>(0.5), static_cast<T>(1.0)))
        return false;
    if (ez::isDifferent(ez::round<T>(0.0), static_cast<T>(0.0)))
        return false;
    if (ez::isDifferent(ez::round<T>(1.2), static_cast<T>(1.0)))
        return false;
    return true;
}

template <typename T>
bool TestEzMathFloor() {
    if (ez::isDifferent(ez::floor<T>(0.5), static_cast<T>(0.0)))
        return false;
    if (ez::isDifferent(ez::floor<T>(0.0), static_cast<T>(0.0)))
        return false;
    if (ez::isDifferent(ez::floor<T>(1.2), static_cast<T>(1.0)))
        return false;
    return true;
}

template <typename T>
bool TestEzMathCeil() {
    if (ez::isDifferent(ez::ceil<T>(0.5), static_cast<T>(1.0)))
        return false;
    if (ez::isDifferent(ez::ceil<T>(0.0), static_cast<T>(0.0)))
        return false;
    if (ez::isDifferent(ez::ceil<T>(1.2), static_cast<T>(2.0)))
        return false;
    return true;
}

template <typename T>
bool TestEzMathFract() {
    if (ez::isDifferent(ez::fract<T>(0.5), static_cast<T>(0.5)))
        return false;
    if (ez::isDifferent(ez::fract<T>(1.2), static_cast<T>(0.2)))
        return false;
    if (ez::isDifferent(ez::fract<T>(2.0), static_cast<T>(0.0)))
        return false;
    return true;
}

template <typename T>
bool TestEzMathCos() {
    if (ez::isDifferent(ez::cos<T>(0.0), static_cast<T>(1.0)))
        return false;
    if (ez::isDifferent(ez::cos<T>(M_PI / 2.0), static_cast<T>(0.0)))
        return false;
    return true;
}

template <typename T>
bool TestEzMathAcos() {
    if (ez::isDifferent(ez::acos<T>(1.0), static_cast<T>(0.0)))
        return false;
    if (ez::isDifferent(ez::acos<T>(0.0), static_cast<T>(M_PI / 2.0)))
        return false;
    return true;
}

template <typename T>
bool TestEzMathSin() {
    if (ez::isDifferent(ez::sin<T>(0.0), static_cast<T>(0.0)))
        return false;
    if (ez::isDifferent(ez::sin<T>(M_PI / 2.0), static_cast<T>(1.0)))
        return false;
    return true;
}

template <typename T>
bool TestEzMathAsin() {
    if (ez::isDifferent(ez::asin<T>(0.0), static_cast<T>(0.0)))
        return false;
    if (ez::isDifferent(ez::asin<T>(1.0), static_cast<T>(M_PI / 2.0)))
        return false;
    return true;
}

template <typename T>
bool TestEzMathTan() {
    if (ez::isDifferent(ez::tan<T>(0.0), static_cast<T>(0.0)))
        return false;
    return true;
}

template <typename T>
bool TestEzMathAtan() {
    if (ez::isDifferent(ez::atan<T>(0.0), static_cast<T>(0.0)))
        return false;
    if (ez::isDifferent(ez::atan<T>(1.0), static_cast<T>(M_PI / 4.0)))
        return false;
    return true;
}

template <typename T>
bool TestEzMathMini() {
    if (ez::mini<T>(1, 2) != 1)
        return false;
    if (ez::mini<T>(2, 1) != 1)
        return false;
    return true;
}

template <typename T>
bool TestEzMathMaxi() {
    if (ez::maxi<T>(1, 2) != 2)
        return false;
    if (ez::maxi<T>(2, 1) != 2)
        return false;
    return true;
}

template <typename T>
bool TestEzMathClamp() {
    if (ez::clamp<T>(-1, 0, 1) != 0)
        return false;
    if (ez::clamp<T>(2, 0, 1) != 1)
        return false;
    if (ez::isDifferent(ez::clamp<T>(0.5, 0, 1), static_cast<T>(0.5)))
        return false;
    return true;
}

template <>
bool TestEzMathClamp<int64_t>() {
    if (ez::clamp<int64_t>(-1, 0, 1) != 0)
        return false;
    if (ez::clamp<int64_t>(2, 0, 1) != 1)
        return false;
    return true;
}

template <>
bool TestEzMathClamp<float>() {
    if (ez::clamp<float>(-1.0f, 0.0f, 1.0f) != 0.0f)
        return false;
    if (ez::clamp<float>(2.0f, 0.0f, 1.0f) != 1.0f)
        return false;
    if (ez::isDifferent(ez::clamp<float>(0.5f, 0.0f, 1.0f), 0.5f))
        return false;
    return true;
}

template <>
bool TestEzMathClamp<double>() {
    if (ez::clamp<double>(-1.0, 0.0, 1.0) != 0.0)
        return false;
    if (ez::clamp<double>(2.0, 0.0, 1.0) != 1.0)
        return false;
    if (ez::isDifferent(ez::clamp<double>(0.5, 0.0, 1.0), 0.5))
        return false;
    return true;
}

// Specialisation for int32_t, only positive values
template <>
bool TestEzMathClamp<int32_t>() {
    if (ez::clamp<int32_t>(-1, 0, 1) != 0)
        return false;
    if (ez::clamp<int32_t>(-2, -5, 5) != -2)
        return false;
    if (ez::clamp<int32_t>(2, -5, -2) != -2)
        return false;
    return true;
}

// Specialisation for uint32_t, only positive values
template <>
bool TestEzMathClamp<uint32_t>() {
    if (ez::clamp<uint32_t>(2, 0, 10) != 2)
        return false;
    if (ez::clamp<uint32_t>(2, 0, 1) != 1)
        return false;
    if (ez::clamp<uint32_t>(2, 5, 10) != 5)
        return false;
    return true;
}

// Specialisation for uint64_t, only positive values
template <>
bool TestEzMathClamp<uint64_t>() {
    if (ez::clamp<uint64_t>(2, 0, 10) != 2)
        return false;
    if (ez::clamp<uint64_t>(2, 0, 1) != 1)
        return false;
    if (ez::clamp<uint64_t>(2, 5, 10) != 5)
        return false;
    return true;
}
template <typename T>
bool TestEzMathAbs() {
    if (ez::abs<T>(-1) != 1)
        return false;
    if (ez::abs<T>(1) != 1)
        return false;
    return true;
}

template <typename T>
bool TestEzMathSign() {
    if (ez::sign<T>(-1) != -1)
        return false;
    if (ez::sign<T>(1) != 1)
        return false;
    return true;
}

template <typename T>
bool TestEzMathStep() {
    if (ez::step<T>(0.5, 0.4) != 0)
        return false;
    if (ez::step<T>(0.5, 0.6) != 1)
        return false;
    return true;
}

template <typename T>
bool TestEzMathMod() {
    if (ez::isDifferent(ez::mod<T>(5.5, 2.0), static_cast<T>(1.5)))
        return false;
    if (ez::isDifferent(ez::mod<T>(4.0, 2.0), static_cast<T>(0.0)))
        return false;
    return true;
}

template <typename T>
bool TestEzMathInvMix() {
    if (ez::isDifferent(ez::invMix<T>(1, 2, 1.5), static_cast<T>(0.5)))
        return false;
    return true;
}

template <typename T>
bool TestEzMathLerp() {
    if (ez::isDifferent(ez::lerp<T>(0, 1, 0.5), static_cast<T>(0.5)))
        return false;
    if (ez::isDifferent(ez::lerp<T>(1, 2, 0.5), static_cast<T>(1.5)))
        return false;
    return true;
}

template <typename T>
bool TestEzMathEerp() {
    if (ez::isDifferent(ez::eerp<T>(0, 2, 0.5), static_cast<T>(0)))
        return false;
    if (ez::isDifferent(ez::eerp<T>(1, 2, 0.5), ez::sqrt<T>(2.0)))
        return false;
    return true;
}

template <typename T>
bool TestEzMathMix() {
    if (ez::isDifferent(ez::mix<T>(0, 1, 0.5), static_cast<T>(0.5)))
        return false;
    if (ez::isDifferent(ez::mix<T>(1, 2, 0.5), static_cast<T>(1.5)))
        return false;
    return true;
}

////////////////////////////////////////////////////////////////////////////
//// ENTRY POINT ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define IfTestExist(v)            \
    if (vTest == std::string(#v)) \
    return v()

bool TestEzMath(const std::string& vTest) {
    IfTestExist(TestEzMathRoundN);

    else IfTestExist(TestEzMathFloatIsValid);

    else IfTestExist(TestEzMathIsDifferent<float>);
    else IfTestExist(TestEzMathIsDifferent<double>);
    else IfTestExist(TestEzMathIsDifferent<int32_t>);
    else IfTestExist(TestEzMathIsDifferent<uint32_t>);
    else IfTestExist(TestEzMathIsDifferent<int64_t>);
    else IfTestExist(TestEzMathIsDifferent<uint64_t>);

    else IfTestExist(TestEzMathIsEqual<float>);
    else IfTestExist(TestEzMathIsEqual<double>);
    else IfTestExist(TestEzMathIsEqual<int32_t>);
    else IfTestExist(TestEzMathIsEqual<uint32_t>);
    else IfTestExist(TestEzMathIsEqual<int64_t>);
    else IfTestExist(TestEzMathIsEqual<uint64_t>);

    else IfTestExist(TestEzMathRound<float>);
    else IfTestExist(TestEzMathRound<double>);

    else IfTestExist(TestEzMathFloor<float>);
    else IfTestExist(TestEzMathFloor<double>);

    else IfTestExist(TestEzMathCeil<float>);
    else IfTestExist(TestEzMathCeil<double>);

    else IfTestExist(TestEzMathFract<float>);
    else IfTestExist(TestEzMathFract<double>);

    else IfTestExist(TestEzMathCos<float>);
    else IfTestExist(TestEzMathCos<double>);

    else IfTestExist(TestEzMathAcos<float>);
    else IfTestExist(TestEzMathAcos<double>);

    else IfTestExist(TestEzMathSin<float>);
    else IfTestExist(TestEzMathSin<double>);

    else IfTestExist(TestEzMathAsin<float>);
    else IfTestExist(TestEzMathAsin<double>);

    else IfTestExist(TestEzMathTan<float>);
    else IfTestExist(TestEzMathTan<double>);

    else IfTestExist(TestEzMathAtan<float>);
    else IfTestExist(TestEzMathAtan<double>);

    else IfTestExist(TestEzMathMini<float>);
    else IfTestExist(TestEzMathMini<double>);
    else IfTestExist(TestEzMathMini<int32_t>);
    else IfTestExist(TestEzMathMini<uint32_t>);
    else IfTestExist(TestEzMathMini<int64_t>);
    else IfTestExist(TestEzMathMini<uint64_t>);

    else IfTestExist(TestEzMathMaxi<float>);
    else IfTestExist(TestEzMathMaxi<double>);
    else IfTestExist(TestEzMathMaxi<int32_t>);
    else IfTestExist(TestEzMathMaxi<uint32_t>);
    else IfTestExist(TestEzMathMaxi<int64_t>);
    else IfTestExist(TestEzMathMaxi<uint64_t>);

    else IfTestExist(TestEzMathClamp<float>);
    else IfTestExist(TestEzMathClamp<double>);
    else IfTestExist(TestEzMathClamp<int32_t>);
    else IfTestExist(TestEzMathClamp<uint32_t>);
    else IfTestExist(TestEzMathClamp<int64_t>);
    else IfTestExist(TestEzMathClamp<uint64_t>);

    else IfTestExist(TestEzMathAbs<float>);
    else IfTestExist(TestEzMathAbs<double>);
    else IfTestExist(TestEzMathAbs<int32_t>);
    else IfTestExist(TestEzMathAbs<int64_t>);

    else IfTestExist(TestEzMathSign<float>);
    else IfTestExist(TestEzMathSign<double>);
    else IfTestExist(TestEzMathSign<int32_t>);
    else IfTestExist(TestEzMathSign<int64_t>);

    else IfTestExist(TestEzMathStep<float>);
    else IfTestExist(TestEzMathStep<double>);

    else IfTestExist(TestEzMathMod<float>);
    else IfTestExist(TestEzMathMod<double>);

    else IfTestExist(TestEzMathLerp<float>);
    else IfTestExist(TestEzMathLerp<double>);

    else IfTestExist(TestEzMathEerp<float>);
    else IfTestExist(TestEzMathEerp<double>);

    else IfTestExist(TestEzMathMix<float>);
    else IfTestExist(TestEzMathMix<double>);

    else IfTestExist(TestEzMathInvMix<float>);
    else IfTestExist(TestEzMathInvMix<double>);

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
