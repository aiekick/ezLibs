#pragma once

/*
MIT License

Copyright (c) 2014-2024 Stephane Cuillerdier (aka aiekick)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// ezMath is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <type_traits>
#include <cmath>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <limits>

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

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

namespace ez {

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#ifndef M_E
#define M_E 2.7182818284590452353602874713527
#endif

// This function rounds a floating-point number to 'n' decimal places.
// Only floating-point types (float, double, long double) are allowed.
template <typename T>
inline std::string round_n(T vValue, int n) {
    static_assert(std::is_floating_point<T>::value, "round_n is only valid for floating point types");
    std::stringstream tmp;
    tmp << std::setprecision(n) << std::fixed << vValue;
    return tmp.str();
}

/// This function is used to ensure that a floating point number is not a NaN or infinity.
inline bool floatIsValid(float x) {
    const union {
        float f;
        int32_t i;
    } v = {x};
    return (v.i & 0x7f800000) != 0x7f800000;
}

// Checks if two numbers are different according to epsilon precision.
template <typename T>
inline bool isDifferent(T vA, T vB) {
    return vA != vB;
}

template <>
inline bool isDifferent(float vA, float vB) {
     return std::fabs(vA - vB) > std::numeric_limits<float>::epsilon();
}

template <>
inline bool isDifferent(double vA, double vB) {
    return std::abs(vA - vB) > std::numeric_limits<double>::epsilon();
}

// Checks if two numbers are equal according to epsilon precision.
template <typename T>
inline bool isEqual(T vA, T vB) {
    return vA == vB;
}

template <>
inline bool isEqual(float vA, float vB) {
    return std::fabs(vA - vB) < std::numeric_limits<float>::epsilon();
}

template <>
inline bool isEqual(double vA, double vB) {
    return std::abs(vA - vB) < std::numeric_limits<double>::epsilon();
}

// Rounds a floating-point number to the nearest integer.
// Only floating-point types (float, double, long double) are allowed.
template <typename T>
inline T round(T v) {
    static_assert(std::is_floating_point<T>::value, "round is only valid for floating point types");
    return static_cast<T>(std::round(v));
}

// Returns the largest integer less than or equal to the floating-point number.
// Only floating-point types (float, double, long double) are allowed.
template <typename T>
inline T floor(T v) {
    static_assert(std::is_floating_point<T>::value, "floor is only valid for floating point types");
    return static_cast<T>(std::floor(v));
}

// Returns the smallest integer greater than or equal to the floating-point number.
// Only floating-point types (float, double, long double) are allowed.
template <typename T>
inline T ceil(T v) {
    static_assert(std::is_floating_point<T>::value, "ceil is only valid for floating point types");
    return static_cast<T>(std::ceil(v));
}

// Returns the fractional part of a floating-point number.
// Only floating-point types (float, double, long double) are allowed.
template <typename T>
inline T fract(T v) {
    static_assert(std::is_floating_point<T>::value, "fract is only valid for floating point types");
    return v - floor(v);
}

// Computes the cosine of a floating-point number.
// Only floating-point types (float, double, long double) are allowed.
template <typename T>
inline T cos(T v) {
    static_assert(std::is_floating_point<T>::value, "cos is only valid for floating point types");
    return std::cos(v);
}

// Computes the arc cosine of a floating-point number.
// Only floating-point types (float, double, long double) are allowed.
template <typename T>
inline T acos(T v) {
    static_assert(std::is_floating_point<T>::value, "acos is only valid for floating point types");
    return std::acos(v);
}

// Computes the sine of a floating-point number.
// Only floating-point types (float, double, long double) are allowed.
template <typename T>
inline T sin(T v) {
    static_assert(std::is_floating_point<T>::value, "sin is only valid for floating point types");
    return std::sin(v);
}

// Computes the arc sine of a floating-point number.
// Only floating-point types (float, double, long double) are allowed.
template <typename T>
inline T asin(T v) {
    static_assert(std::is_floating_point<T>::value, "asin is only valid for floating point types");
    return std::asin(v);
}

// Computes the tangent of a floating-point number.
// Only floating-point types (float, double, long double) are allowed.
template <typename T>
inline T tan(T v) {
    static_assert(std::is_floating_point<T>::value, "tan is only valid for floating point types");
    return std::tan(v);
}

// Computes the arc tangent of a floating-point number.
// Only floating-point types (float, double, long double) are allowed.
template <typename T>
inline T atan(T v) {
    static_assert(std::is_floating_point<T>::value, "atan is only valid for floating point types");
    return std::atan(v);
}

// Computes the sqrt of a floating-point number.
// Only floating-point types (float, double, long double) are allowed.
template <typename T>
inline T sqrt(T v) {
    static_assert(std::is_floating_point<T>::value, "sqrt is only valid for floating point types");
    return std::sqrt(v);
}

// Returns the smaller of two values.
// Works with both integral and floating-point types.
template <typename T>
inline T mini(T a, T b) {
    return a < b ? a : b;
}

// Returns the larger of two values.
// Works with both integral and floating-point types.
template <typename T>
inline T maxi(T a, T b) {
    return a > b ? a : b;
}

// Clamps a value between 0 and 1.
// Works with both integral and floating-point types.
template <typename T>
inline T clamp(T n) {
    return n >= static_cast<T>(0) && n <= static_cast<T>(1) ? n : static_cast<T>(n > static_cast<T>(0));
}

// Clamps a value between 0 and b.
// Works with both integral and floating-point types.
template <typename T>
inline T clamp(T n, T b) {
    return n >= static_cast<T>(0) && n <= b ? n : static_cast<T>(n > static_cast<T>(0)) * b;
}

// Clamps a value between a and b.
// Works with both integral and floating-point types.
template <typename T>
inline T clamp(T n, T a, T b) {
    return n >= a && n <= b ? n : n < a ? a : b;
}

// Computes the absolute value of a number.
// Works with both integral and floating-point types.
template <typename T>
inline T abs(T a) {
    static_assert(std::is_arithmetic<T>::value, "abs is only valid for arithmetic types");
    return (a < 0) ? a * static_cast<T>(-1) : a;
}

// Determines the sign of a number (-1 for negative, 1 for positive).
// Works with both integral and floating-point types.
template <typename T>
inline T sign(T a) {
    static_assert(std::is_signed<T>::value, "sign is only valid for signed types");
    if (a < 0) {
        return static_cast<T>(-1);
    } else if (a > 0) {
        return static_cast<T>(1);
    }
    return static_cast<T>(0);
}

// Returns 0 if b < a, otherwise returns 1.
// Works with both integral and floating-point types.
template <typename T>
inline T step(T a, T b) {
    static_assert(std::is_arithmetic<T>::value, "step is only valid for arithmetic types");
    return (b < a) ? static_cast<T>(0) : static_cast<T>(1);
}

// Computes the floating-point remainder of the division operation.
// Only floating-point types (float, double, long double) are allowed.
template <typename T>
inline T mod(T v, T l) {
    static_assert(std::is_floating_point<T>::value, "mod is only valid for floating point types");
    return std::fmod(v, l);
}

// Computes the inverse of the linear interpolation mix.
// Only floating-point types (float, double, long double) are allowed.
template <typename T>
inline T invMix(T i, T s, T r) {
    static_assert(std::is_floating_point<T>::value, "invMix is only valid for floating point types");
    return (r - i) / (s - i);
}

// Linearly interpolates between a and b by t.
// Only floating-point types (float, double, long double) are allowed.
template <typename T>
inline T lerp(T a, T b, T t) {
    static_assert(std::is_floating_point<T>::value, "lerp is only valid for floating point types");
    return a * (static_cast<T>(1.0) - t) + b * t;
}

// Exponentially interpolates between a and b by t.
// Only floating-point types (float, double, long double) are allowed.
template <typename T>
inline T eerp(T a, T b, T t) {
    static_assert(std::is_floating_point<T>::value, "eerp is only valid for floating point types");
    if (a == static_cast<T>(0))
        return static_cast<T>(0);
    return std::pow(a * (b / a), t);
}

// Performs linear interpolation (lerp) between a and b by t.
// Only floating-point types (float, double, long double) are allowed.
template <typename T>
inline T mix(T a, T b, T t) {
    static_assert(std::is_floating_point<T>::value, "mix is only valid for floating point types");
    return lerp(a, b, t);
}

}  // namespace ez

#include "ezStr.hpp"
#include "ezVec2.hpp"
#include "ezVec3.hpp"
#include "ezVec4.hpp"
#include "ezAABB.hpp"
#include "ezQuat.hpp"
#include "ezPlane.hpp"
#include "ezAABBCC.hpp"

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
