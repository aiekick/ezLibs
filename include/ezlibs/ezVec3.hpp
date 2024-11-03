#pragma once

#ifndef EZ_TOOLS_VEC3
#define EZ_TOOLS_VEC3
#endif  // EZ_TOOLS_VEC3

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

// EzVec3 is part od the EzLibs project : https://github.com/aiekick/EzLibs.git

#include <type_traits>
#include <cmath>
#include <string>
#include <vector>

namespace ez {

// Restrict the template to relevant types only (e.g., disable bool)
template <typename T>
struct vec3 {
    static_assert(!std::is_same<T, bool>::value, "vec3 cannot be instantiated with bool type");

    T x = static_cast<T>(0), y = static_cast<T>(0), z = static_cast<T>(0);

    // Default constructor
    vec3() : x(static_cast<T>(0)), y(static_cast<T>(0)), z(static_cast<T>(0)) {}

    // Constructor with type conversion
    template <typename U>
    vec3(const vec3<U>& a) : x(static_cast<T>(a.x)), y(static_cast<T>(a.y)), z(static_cast<T>(a.z)) {}

    // Constructor with type conversion
    template <typename U>
    vec3(const U& a) : x(static_cast<T>(a.x)), y(static_cast<T>(a.y)), z(static_cast<T>(a.z)) {}

    // Constructor for uniform initialization
    vec3(T xyz) : x(xyz), y(xyz), z(xyz) {}

    // Constructor with specific values
    vec3(T x, T y, T z) : x(x), y(y), z(z) {}

    // Constructor using a vec2 and a scalar
    // Assumption: vec2<T> is defined elsewhere, as it is referenced but not included here.
    vec3(const vec2<T>& xy, T z) : x(xy.x), y(xy.y), z(z) {}

    // Constructor from string
    vec3(const std::string& vec, char c = ';', vec3<T>* def = nullptr) {
        if (def) {
            x = def->x;
            y = def->y;
            z = def->z;
        }
        std::vector<T> result = str::stringToNumberVector<T>(vec, c);
        const size_t s = result.size();
        if (s > 0)
            x = result[0];
        if (s > 1)
            y = result[1];
        if (s > 2)
            z = result[2];
    }

    // Indexing operator
    T& operator[](size_t i) { return (&x)[i]; }

    // Offset the vector
    vec3 Offset(T vX, T vY, T vZ) const { return vec3(x + vX, y + vY, z + vZ); }

    // Set the vector's components
    void Set(T vX, T vY, T vZ) {
        x = vX;
        y = vY;
        z = vZ;
    }

    operator vec2<T>() const { return vec2<T>(x, y); }

    // Negation operator
    vec3 operator-() const {
        static_assert(std::is_signed<T>::value, "Negate is only valid for signed types");
        return vec3(-x, -y, -z);
    }

    // Logical NOT operator, only for integral types
    vec3 operator!() const {
        static_assert(std::is_integral<T>::value, "Logical NOT is only valid for integral types");
        return vec3(!x, !y, !z);
    }

    // Extract 2D vectors from 3D vector
    vec2<T> xy() const { return vec2<T>(x, y); }

    vec2<T> xz() const { return vec2<T>(x, z); }

    vec2<T> yz() const { return vec2<T>(y, z); }

    // Cyclic permutation
    vec3 yzx() const { return vec3(y, z, x); }

    // Pre-increment and pre-decrement operators
    vec3& operator++() {
        ++x;
        ++y;
        ++z;
        return *this;
    }

    vec3& operator--() {
        --x;
        --y;
        --z;
        return *this;
    }

    // Post-increment and post-decrement operators
    vec3 operator++(int) {
        vec3 tmp = *this;
        ++*this;
        return tmp;
    }

    vec3 operator--(int) {
        vec3 tmp = *this;
        --*this;
        return tmp;
    }

    // Compound assignment operators
    void operator+=(T a) {
        x += a;
        y += a;
        z += a;
    }

    void operator+=(const vec3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
    }

    void operator-=(T a) {
        x -= a;
        y -= a;
        z -= a;
    }

    void operator-=(const vec3& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
    }

    void operator*=(T a) {
        x *= a;
        y *= a;
        z *= a;
    }

    void operator*=(const vec3& v) {
        x *= v.x;
        y *= v.y;
        z *= v.z;
    }

    void operator/=(T a) {
        x /= a;
        y /= a;
        z /= a;
    }

    void operator/=(const vec3& v) {
        x /= v.x;
        y /= v.y;
        z /= v.z;
    }

    // Length of the vector
    T length() const { return static_cast<T>(ez::sqrt(lengthSquared())); }

    // Squared length of the vector
    T lengthSquared() const { return x * x + y * y + z * z; }

    // Normalize the vector
    T normalize() {
        T _length = length();
        if (_length < std::numeric_limits<T>::epsilon())
            return static_cast<T>(0);
        T _invLength = static_cast<T>(1) / _length;
        x *= _invLength;
        y *= _invLength;
        z *= _invLength;
        return _length;
    }

    // Get a normalized copy of the vector
    vec3 GetNormalized() const {
        vec3 n(x, y, z);
        n.normalize();
        return n;
    }

    // Sum of components
    T sum() const { return x + y + z; }

    // Sum of absolute values of components
    T sumAbs() const { return ez::abs(x) + ez::abs(y) + ez::abs(z); }

    // Check if all components are zero (AND)
    bool emptyAND() const { return x == static_cast<T>(0) && y == static_cast<T>(0) && z == static_cast<T>(0); }

    // Check if any component is zero (OR)
    bool emptyOR() const { return x == static_cast<T>(0) || y == static_cast<T>(0) || z == static_cast<T>(0); }

    // Convert to string
    std::string string(char c = ';') const { return ez::str::toStr(x) + c + ez::str::toStr(y) + c + ez::str::toStr(z); }

    // Minimum component
    T mini() const { return ez::mini(x, ez::mini(y, z)); }

    // Maximum component
    T maxi() const { return ez::maxi(x, ez::maxi(y, z)); }
};

// Operators for vec3
template <typename T>
inline vec3<T> operator+(const vec3<T>& v, T f) {
    return vec3<T>(v.x + f, v.y + f, v.z + f);
}

template <typename T>
inline vec3<T> operator+(T f, const vec3<T>& v) {
    return vec3<T>(v.x + f, v.y + f, v.z + f);
}

template <typename T>
inline vec3<T> operator+(const vec3<T>& v, const vec3<T>& f) {
    return vec3<T>(v.x + f.x, v.y + f.y, v.z + f.z);
}

template <typename T>
inline vec3<T> operator-(const vec3<T>& v, T f) {
    return vec3<T>(v.x - f, v.y - f, v.z - f);
}

template <typename T>
inline vec3<T> operator-(T f, const vec3<T>& v) {
    return vec3<T>(f - v.x, f - v.y, f - v.z);
}

template <typename T>
inline vec3<T> operator-(const vec3<T>& v, const vec3<T>& f) {
    return vec3<T>(v.x - f.x, v.y - f.y, v.z - f.z);
}

template <typename T>
inline vec3<T> operator*(const vec3<T>& v, T f) {
    return vec3<T>(v.x * f, v.y * f, v.z * f);
}

template <typename T>
inline vec3<T> operator*(T f, const vec3<T>& v) {
    return vec3<T>(v.x * f, v.y * f, v.z * f);
}

template <typename T>
inline vec3<T> operator*(const vec3<T>& v, const vec3<T>& f) {
    return vec3<T>(v.x * f.x, v.y * f.y, v.z * f.z);
}

template <typename T>
inline vec3<T> operator/(const vec3<T>& v, T f) {
    return vec3<T>(v.x / f, v.y / f, v.z / f);
}

template <typename T>
inline vec3<T> operator/(T f, const vec3<T>& v) {
    return vec3<T>(f / v.x, f / v.y, f / v.z);
}

template <typename T>
inline vec3<T> operator/(const vec3<T>& v, const vec3<T>& f) {
    return vec3<T>(v.x / f.x, v.y / f.y, v.z / f.z);
}

// Comparison operators
template <typename T>
inline bool operator<(const vec3<T>& v, const vec3<T>& f) {
    return v.x < f.x && v.y < f.y && v.z < f.z;
}

template <typename T>
inline bool operator<(const vec3<T>& v, T f) {
    return v.x < f && v.y < f && v.z < f;
}

template <typename T>
inline bool operator<(T f, const vec3<T>& v) {
    return f < v.x && f < v.y && f < v.z;
}

template <typename T>
inline bool operator>(const vec3<T>& v, const vec3<T>& f) {
    return v.x > f.x && v.y > f.y && v.z > f.z;
}

template <typename T>
inline bool operator>(const vec3<T>& v, T f) {
    return v.x > f && v.y > f && v.z > f;
}

template <typename T>
inline bool operator>(T f, const vec3<T>& v) {
    return f > v.x && f > v.y && f > v.z;
}

template <typename T>
inline bool operator<=(const vec3<T>& v, const vec3<T>& f) {
    return v.x <= f.x && v.y <= f.y && v.z <= f.z;
}

template <typename T>
inline bool operator<=(const vec3<T>& v, T f) {
    return v.x <= f && v.y <= f && v.z <= f;
}

template <typename T>
inline bool operator<=(T f, const vec3<T>& v) {
    return f <= v.x && f <= v.y && f <= v.z;
}

template <typename T>
inline bool operator>=(const vec3<T>& v, const vec3<T>& f) {
    return v.x >= f.x && v.y >= f.y && v.z >= f.z;
}

template <typename T>
inline bool operator>=(const vec3<T>& v, T f) {
    return v.x >= f && v.y >= f && v.z >= f;
}

template <typename T>
inline bool operator>=(T f, const vec3<T>& v) {
    return f >= v.x && f >= v.y && f >= v.z;
}

template <typename T>
inline bool operator!=(const vec3<T>& v, const vec3<T>& f) {
    return v.x != f.x || v.y != f.y || v.z != f.z;
}

template <>
inline bool operator!=(const vec3<float>& v, const vec3<float>& f) {
    return ez::isDifferent(v.x, f.x) || ez::isDifferent(v.y, f.y) || ez::isDifferent(v.z, f.z);
}

template <>
inline bool operator!=(const vec3<double>& v, const vec3<double>& f) {
    return ez::isDifferent(v.x, f.x) || ez::isDifferent(v.y, f.y) || ez::isDifferent(v.z, f.z);
}

template <typename T>
inline bool operator!=(const vec3<T>& v, T f) {
    return v.x != f || v.y != f || v.z != f;
}

template <typename T>
inline bool operator!=(T f, const vec3<T>& v) {
    return f != v.x || f != v.y || f != v.z;
}

template <typename T>
inline bool operator==(const vec3<T>& v, const vec3<T>& f) {
    return v.x == f.x && v.y == f.y && v.z == f.z;
}

template <>
inline bool operator==(const vec3<float>& v, const vec3<float>& f) {
    return ez::isEqual(v.x, f.x) && ez::isEqual(v.y, f.y) && ez::isEqual(v.z, f.z);
}

template <>
inline bool operator==(const vec3<double>& v, const vec3<double>& f) {
    return ez::isEqual(v.x, f.x) && ez::isEqual(v.y, f.y) && ez::isEqual(v.z, f.z);
}

template <typename T>
inline bool operator==(const vec3<T>& v, T f) {
    return v.x == f && v.y == f && v.z == f;
}

template <typename T>
inline bool operator==(T f, const vec3<T>& v) {
    return f == v.x && f == v.y && f == v.z;
}

// Utility functions
template <typename T>
inline vec3<T> mini(const vec3<T>& a, const vec3<T>& b) {
    return vec3<T>(ez::mini(a.x, b.x), ez::mini(a.y, b.y), ez::mini(a.z, b.z));
}

template <typename T>
inline vec3<T> maxi(const vec3<T>& a, const vec3<T>& b) {
    return vec3<T>(ez::maxi(a.x, b.x), ez::maxi(a.y, b.y), ez::maxi(a.z, b.z));
}

template <typename T>
inline vec3<T> floor(const vec3<T>& a) {
    return vec3<T>(ez::floor(a.x), ez::floor(a.y), ez::floor(a.z));
}

template <typename T>
inline vec3<T> ceil(const vec3<T>& a) {
    return vec3<T>(ez::ceil(a.x), ez::ceil(a.y), ez::ceil(a.z));
}

template <typename T>
inline vec3<T> abs(const vec3<T>& a) {
    return vec3<T>(ez::abs(a.x), ez::abs(a.y), ez::abs(a.z));
}

template <typename T>
inline T dotS(const vec3<T>& a, const vec3<T>& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

template <typename T>
inline vec3<T> cCross(const vec3<T>& a, const vec3<T>& b) {
    return vec3<T>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

template <typename T>
inline vec3<T> cReflect(const vec3<T>& I, const vec3<T>& N) {
    return I - static_cast<T>(2) * dotS(N, I) * N;
}

// Type aliases for common vector types
using dvec3 = vec3<double>;
using fvec3 = vec3<float>;
using f32vec3 = vec3<float>;
using f64vec3 = vec3<double>;
using ivec3 = vec3<int>;
using i8vec3 = vec3<int8_t>;
using i16vec3 = vec3<int16_t>;
using i32vec3 = vec3<int32_t>;
using i64vec3 = vec3<int64_t>;
using u8vec3 = vec3<uint8_t>;
using u16vec3 = vec3<uint16_t>;
using uvec3 = vec3<uint32_t>;
using u32vec3 = vec3<uint32_t>;
using u64vec3 = vec3<uint64_t>;

// Specialization for float32 validation
inline bool valid(const fvec3& a) {
    return floatIsValid(a.x) && floatIsValid(a.y) && floatIsValid(a.z);
}

}  // namespace ez
