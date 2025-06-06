#pragma once

#ifndef EZ_TOOLS_VEC2
#define EZ_TOOLS_VEC2
#endif  // EZ_TOOLS_VEC2

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

// ezVec2 is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <cmath>
#include <type_traits>
#include <string>
#include <vector>

#ifdef min
#undef min
#endif  // min

#ifdef max
#undef max
#endif  // max

// Namespace ez
namespace ez {

// Disable specific types for template functions
template <typename T>
struct is_valid_type : std::integral_constant<bool, (std::is_floating_point<T>::value || (std::is_integral<T>::value && sizeof(T) > 1))> {};

// Vector 2D template class
template <typename T>
struct vec2 {
    // Disable this template if T is not an integral or floating point type
    static_assert(is_valid_type<T>::value, "Invalid type for vec2. Only integral types larger than (u)int16_t and floating-point types are allowed.");

    T x = static_cast<T>(0), y = static_cast<T>(0);

    // Constructors
    vec2() = default;

    // Constructor with type conversion
    template <typename U>
    vec2(const vec2<U>& a) : x(static_cast<T>(a.x)), y(static_cast<T>(a.y)) {}

    // Constructor with type conversion
    template <typename U>
    vec2(const U& a) : x(static_cast<T>(a.x)), y(static_cast<T>(a.y)) {}

    vec2(T a) : x(a), y(a) {}

    vec2(T a, T b) : x(a), y(b) {}

#ifdef EZ_STR
    vec2(const std::vector<std::string>& vArray) {
        const size_t s = vArray.size();
        if (s > 0) {
            str::stringToNumber(vArray.at(0), x);
        }
        if (s > 1) {
            str::stringToNumber(vArray.at(1), y);
        }
    };

    vec2(const std::string& vec, char c = ';', const vec2<T>* def = nullptr) {
        if (def) {
            x = def->x;
            y = def->y;
        }
        std::vector<T> result = str::stringToNumberVector<T>(vec, c);
        const size_t s = result.size();
        if (s > 0) {
            x = result.at(0);
        }
        if (s > 1) {
            x = result.at(1);
        }
    }
#endif

    // Element access operator
    T& operator[](size_t i) { return (&x)[i]; }

    // Offset function
    vec2 Offset(T vX, T vY) const { return vec2(x + vX, y + vY); }

    // Set function
    void Set(T vX, T vY) {
        x = vX;
        y = vY;
    }

    vec2 lerp(const vec2<T>& vPos, T vLerpValue) {
        static_assert(std::is_floating_point<T>::value, "lerp is only valid for floating point types");
        return vec2(  //
            ez::lerp(x, vPos.x, vLerpValue),
            ez::lerp(y, vPos.y, vLerpValue));

    }

    vec2 lerp(const vec2<T>& vPos, const vec2<T>& vLerpValue) {
        static_assert(std::is_floating_point<T>::value, "lerp is only valid for floating point types");
        return vec2(  //
            ez::lerp(x, vPos.x, vLerpValue.x),
            ez::lerp(y, vPos.y, vLerpValue.y));
    }

    // Negation operator
    vec2 operator-() const {
        static_assert(std::is_signed<T>::value, "Negation operator is only valid for signed types");
        return vec2(-x, -y);
    }

    // Logical NOT operator
    vec2 operator!() const {
        static_assert(std::is_integral<T>::value, "Logical NOT is only valid for integral types");
        return vec2(!x, !y);
    }

    // Increment and decrement operators
    vec2& operator++() {
        ++x;
        ++y;
        return *this;
    }

    vec2& operator--() {
        --x;
        --y;
        return *this;
    }

    vec2 operator++(int) {
        vec2 tmp = *this;
        ++*this;
        return tmp;
    }

    vec2 operator--(int) {
        vec2 tmp = *this;
        --*this;
        return tmp;
    }

    // Compound assignment operators
    void operator+=(T a) {
        x += a;
        y += a;
    }

    void operator+=(const vec2<T>& v) {
        x += v.x;
        y += v.y;
    }

    void operator-=(T a) {
        x -= a;
        y -= a;
    }

    void operator-=(const vec2<T>& v) {
        x -= v.x;
        y -= v.y;
    }

    void operator*=(T a) {
        x *= a;
        y *= a;
    }

    void operator*=(const vec2<T>& v) {
        x *= v.x;
        y *= v.y;
    }

    void operator/=(T a) {
        x /= a;
        y /= a;
    }

    void operator/=(const vec2<T>& v) {
        x /= v.x;
        y /= v.y;
    }

    // Comparison operators
    bool operator==(T a) const { return (x == a) && (y == a); }

    bool operator==(const vec2<T>& v) const { return (x == v.x) && (y == v.y); }

    bool operator!=(T a) const { return (x != a) || (y != a); }

    bool operator!=(const vec2<T>& v) const { return (x != v.x) || (y != v.y); }

    // Length and normalization
    T lengthSquared() const { return x * x + y * y; }

    T length() const { return static_cast<T>(ez::sqrt(lengthSquared())); }

    T normalize() {
        T len = length();
        if (len < static_cast<T>(1e-5))
            return static_cast<T>(0.0);
        T invLen = static_cast<T>(1.0) / len;
        x *= invLen;
        y *= invLen;
        return len;
    }

    vec2 GetNormalized() const {
        vec2 n(x, y);
        n.normalize();
        return n;
    }

    // Sum functions
    T sum() const { return x + y; }

    T sumAbs() const { return ez::abs(x) + ez::abs(y); }

    // Empty checks
    bool emptyAND() const { return x == static_cast<T>(0) && y == static_cast<T>(0); }

    bool emptyOR() const { return x == static_cast<T>(0) || y == static_cast<T>(0); }

#ifdef EZ_STR
    // Convert to string
    std::string string(char c = ';') const { return str::toStr(x) + c + str::toStr(y); }
    std::vector<std::string> array(char c = ';') const { return {str::toStr(x), str::toStr(y)}; }
#endif

    // Ratio functions
    template <typename U>
    U ratioXY() const {
        if (y != static_cast<T>(0))
            return static_cast<U>(x) / static_cast<U>(y);
        return static_cast<U>(0);
    }

    template <typename U>
    U ratioYX() const {
        if (x != static_cast<T>(0))
            return static_cast<U>(y) / static_cast<U>(x);
        return static_cast<U>(0);
    }

    // Min and max
    T min() const { return x < y ? x : y; }

    T max() const { return x > y ? x : y; }
};

// Operators
template <typename T>
inline vec2<T> operator+(const vec2<T>& v, T f) {
    return vec2<T>(v.x + f, v.y + f);
}

template <typename T>
inline vec2<T> operator+(T f, const vec2<T>& v) {
    return vec2<T>(v.x + f, v.y + f);
}

template <typename T>
inline vec2<T> operator+(const vec2<T>& v, const vec2<T>& f) {
    return vec2<T>(v.x + f.x, v.y + f.y);
}

template <typename T>
inline vec2<T> operator-(const vec2<T>& v, T f) {
    return vec2<T>(v.x - f, v.y - f);
}

template <typename T>
inline vec2<T> operator-(T f, const vec2<T>& v) {
    return vec2<T>(f - v.x, f - v.y);
}

template <typename T>
inline vec2<T> operator-(const vec2<T>& v, const vec2<T>& f) {
    return vec2<T>(v.x - f.x, v.y - f.y);
}

template <typename T>
inline vec2<T> operator*(const vec2<T>& v, T f) {
    return vec2<T>(v.x * f, v.y * f);
}

template <typename T>
inline vec2<T> operator*(T f, const vec2<T>& v) {
    return vec2<T>(v.x * f, v.y * f);
}

template <typename T>
inline vec2<T> operator*(const vec2<T>& v, const vec2<T>& f) {
    return vec2<T>(v.x * f.x, v.y * f.y);
}

template <typename T>
inline vec2<T> operator/(const vec2<T>& v, T f) {
    return vec2<T>(v.x / f, v.y / f);
}

template <typename T>
inline vec2<T> operator/(T f, const vec2<T>& v) {
    return vec2<T>(f / v.x, f / v.y);
}

template <typename T>
inline vec2<T> operator/(const vec2<T>& v, const vec2<T>& f) {
    return vec2<T>(v.x / f.x, v.y / f.y);
}

// Comparison operators
template <typename T>
inline bool operator<(const vec2<T>& v, const vec2<T>& f) {
    return v.x < f.x && v.y < f.y;
}

template <typename T>
inline bool operator<(const vec2<T>& v, T f) {
    return v.x < f && v.y < f;
}

template <typename T>
inline bool operator>(const vec2<T>& v, const vec2<T>& f) {
    return v.x > f.x && v.y > f.y;
}

template <typename T>
inline bool operator>(const vec2<T>& v, T f) {
    return v.x > f && v.y > f;
}

template <typename T>
inline bool operator<=(const vec2<T>& v, const vec2<T>& f) {
    return v.x <= f.x && v.y <= f.y;
}

template <typename T>
inline bool operator<=(const vec2<T>& v, T f) {
    return v.x <= f && v.y <= f;
}

template <typename T>
inline bool operator>=(const vec2<T>& v, const vec2<T>& f) {
    return v.x >= f.x && v.y >= f.y;
}

template <typename T>
inline bool operator>=(const vec2<T>& v, T f) {
    return v.x >= f && v.y >= f;
}

template <typename T>
inline bool operator!=(const vec2<T>& v, const vec2<T>& f) {
    return f.x != v.x || f.y != v.y;
}

template <typename T>
inline bool operator==(const vec2<T>& v, const vec2<T>& f) {
    return f.x == v.x && f.y == v.y;
}

// Additional vector operations
template <typename T>
inline vec2<T> floor(const vec2<T>& a) {
    return vec2<T>(ez::floor(a.x), ez::floor(a.y));
}

template <typename T>
inline vec2<T> fract(const vec2<T>& a) {
    static_assert(std::is_floating_point<T>::value, "fract is only valid for theses types : float, double, long double");
    return vec2<T>(fract(a.x), fract(a.y));
}

template <typename T>
inline vec2<T> ceil(const vec2<T>& a) {
    return vec2<T>(ez::ceil(a.x), ez::ceil(a.y));
}

template <typename T>
inline vec2<T> mini(const vec2<T>& a, const vec2<T>& b) {
    return vec2<T>(ez::mini(a.x, b.x), ez::mini(a.y, b.y));
}

template <typename T>
inline vec2<T> maxi(const vec2<T>& a, const vec2<T>& b) {
    return vec2<T>(ez::maxi(a.x, b.x), ez::maxi(a.y, b.y));
}

// scalar prodcut not possible with (u)int8
template <typename T>
inline T dot(const vec2<T>& a, const vec2<T>& b) {
    return a.x * b.x + a.y * b.y;
}

template <typename T>
inline T det(const vec2<T>& a, const vec2<T>& b) {
    return a.x * b.y - a.y * b.x;
}

template <typename T>
inline T length(const vec2<T>& v) {
    return sqrt(dot(v,v));
}

template <typename T>
inline vec2<T> cross(const vec2<T>& a, const vec2<T>& b) {
    return vec2<T>(a.x * b.y - a.y * b.x, a.y * b.x - a.x * b.y);
}

template <typename T>
inline vec2<T> reflect(const vec2<T>& I, const vec2<T>& N) {
    static_assert(std::is_floating_point<T>::value, "fract is only valid for floating point types");
    return I - static_cast<T>(2) * ez::dot(N, I) * N;
}

template <typename T>
inline vec2<T> sign(const vec2<T>& a) {
    return vec2<T>(ez::sign(a.x), ez::sign(a.y));
}

template <typename T>
inline vec2<T> sin(const vec2<T>& a) {
    return vec2<T>(ez::sin(a.x), ez::sin(a.y));
}

template <typename T>
inline vec2<T> cos(const vec2<T>& a) {
    return vec2<T>(ez::cos(a.x), ez::cos(a.y));
}

template <typename T>
inline vec2<T> tan(const vec2<T>& a) {
    return vec2<T>(ez::tan(a.x), ez::tan(a.y));
}

template <typename T>
inline vec2<T> atan(const vec2<T>& a) {
    return vec2<T>(ez::atan(a.x), ez::atan(a.y));
}

// Clamps a value between 0 and 1.
// Works with both integral and floating-point types.
template <typename T>
inline vec2<T> clamp(vec2<T> n) {
    vec2<T> ret;
    ret.x = ez::clamp(n.x);
    ret.y = ez::clamp(n.y);
    return ret;
}

// Clamps a value between 0 and b.
// Works with both integral and floating-point types.
template <typename T>
inline vec2<T> clamp(vec2<T> n, T b) {
    vec2<T> ret;
    ret.x = ez::clamp(n.x);
    ret.y = ez::clamp(n.y);
    return ret;
}

// Clamps a value between a and b.
// Works with both integral and floating-point types.
template <typename T>
inline vec2<T> clamp(vec2<T> n, T a, T b) {
    vec2<T> ret;
    ret.x = ez::clamp(n.x, a, b);
    ret.y = ez::clamp(n.y, a, b);
    return ret;
}

// Using statements for different types of vec2
using dvec2 = vec2<double>;
using fvec2 = vec2<float>;
using f32vec2 = vec2<float>;
using f64vec2 = vec2<double>;
using i8vec2 = vec2<int8_t>;
using i16vec2 = vec2<int16_t>;
using ivec2 = vec2<int32_t>;
using i32vec2 = vec2<int32_t>;
using i64vec2 = vec2<int64_t>;
using u8vec2 = vec2<uint8_t>;
using u16vec2 = vec2<uint16_t>;
using uvec2 = vec2<uint32_t>;
using u32vec2 = vec2<uint32_t>;
using u64vec2 = vec2<uint64_t>;

// Conversion functions
inline fvec2 convert(const ivec2& v) {
    return fvec2(static_cast<float>(v.x), static_cast<float>(v.y));
}

inline ivec2 convert(const fvec2& v) {
    return ivec2(static_cast<int>(v.x), static_cast<int>(v.y));
}

#ifdef floatIsValid
// Float validation
inline bool valid(const fvec2& a) {
    return floatIsValid(a.x) && floatIsValid(a.y);
}
#endif

#ifdef isEqual
// Float comparison operators
inline bool operator==(const fvec2& v, const fvec2& f) {
    return isEqual(f.x, v.x) && isEqual(f.y, v.y);
}
#endif

#ifdef isDifferent
inline bool operator!=(const fvec2& v, const fvec2& f) {
    return isDifferent(f.x, v.x) || isDifferent(f.y, v.y);
}
#endif

// Function to compute the angle in radians from a vec2, only for floating-point types
// Allowed types: float, double, long double
template <typename T>
inline typename std::enable_if<std::is_floating_point<T>::value, T>::type radAngleFromVec2(const vec2<T>& vec) {
    T angle = static_cast<T>(0);
    if (vec.lengthSquared() > static_cast<T>(0) && vec.x != static_cast<T>(0)) {
        angle = ez::atan(vec.y / vec.x);
    }
    return angle;
}

// Function to compute the continuous angle in radians from a vec2 with an offset, only for floating-point types
// Allowed types: float, double, long double
template <typename T>
inline typename std::enable_if<std::is_floating_point<T>::value, T>::type radAngleContinuousFromVec2(const vec2<T>& vec, T angleOffset) {
    T angle = static_cast<T>(0);
    if (vec.x > static_cast<T>(0)) {
        angle = ez::atan(vec.y / vec.x);
    } else if (vec.x < static_cast<T>(0)) {
        angle = static_cast<T>(M_PI) - ez::atan(-vec.y / vec.x);
    }
    return angle - angleOffset;
}

template <typename T>
inline std::istream& operator>>(std::istream& vIn, vec2<T>& vType) {
    char separator;
    if (vIn >> vType.x >> separator >> vType.y) {
        if (separator != ';') {
            vIn.setstate(std::ios::failbit);
        }
    }
    return vIn;
}

template <typename T>
inline std::ostream& operator<<(std::ostream& vOut, const vec2<T>& vType) {
    vOut << vType.x << ";" << vType.y;
    return vOut;
}

}  // namespace ez
