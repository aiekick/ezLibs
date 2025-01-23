#pragma once

#ifndef EZ_TOOLS_VEC4
#define EZ_TOOLS_VEC4
#endif  // EZ_TOOLS_VEC4

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

// ezVec4 is part od the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <cmath>
#include <string>
#include <vector>
#include <type_traits>

namespace ez {

// Restrict the template to relevant types only (e.g., disable bool)
template <typename T>
struct vec4 {
    static_assert(!std::is_same<T, bool>::value, "vec4 cannot be instantiated with bool type");

    T x = 0;
    T y = 0;
    T z = 0;
    T w = 0;

    // Default constructor
    vec4() = default;

    // Constructor with type conversion
    template <typename U>
    vec4(const vec4<U>& a) : x(static_cast<T>(a.x)), y(static_cast<T>(a.y)), z(static_cast<T>(a.z)), w(static_cast<T>(a.w)) {}

    // Constructor with type conversion
    template <typename U>
    vec4(const U& a) : x(static_cast<T>(a.x)), y(static_cast<T>(a.y)), z(static_cast<T>(a.z)), w(static_cast<T>(a.w)) {}

    // Constructor using vec2 components
    vec4(const vec2<T>& xy, const vec2<T>& zw) : x(xy.x), y(xy.y), z(zw.x), w(zw.y) {}

    // Constructor using vec3 and a scalar
    vec4(const vec3<T>& xyz, T w) : x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}

    // Constructor with a single scalar for all components
    vec4(T xyzw) : x(xyzw), y(xyzw), z(xyzw), w(xyzw) {}

    // Constructor with specific values
    vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

    // Constructor from a string
    vec4(const std::string& vec, char c = ';', vec4<T>* def = nullptr) {
        if (def) {
            x = def->x;
            y = def->y;
            z = def->z;
            w = def->w;
        }
        std::vector<T> result = str::stringToNumberVector<T>(vec, c);
        const size_t s = result.size();
        if (s > 0)
            x = result[0];
        if (s > 1)
            y = result[1];
        if (s > 2)
            z = result[2];
        if (s > 3)
            w = result[3];
    }

    // Alternate constructor from a string with size checking
    vec4(const std::string& vec, char c = ';', int n = 4, vec4<T>* def = nullptr) {
        if (def) {
            x = def->x;
            y = def->y;
            z = def->z;
            w = def->w;
        }
        std::vector<T> result = str::stringToNumberVector<T>(vec, c);
        const size_t s = result.size();
        if (static_cast<int>(s) != n) {
            if (s == 1) {
                x = y = z = w = result[0];
            } else if (s == 2) {
                x = y = result[0];
                z = w = result[1];
            } else if (s == 3) {
                x = result[0];
                y = result[1];
                z = result[2];
            }
        } else {
            if (s > 0)
                x = result[0];
            if (s > 1)
                y = result[1];
            if (s > 2)
                z = result[2];
            if (s > 3)
                w = result[3];
        }
    }

    // Indexing operator
    T& operator[](size_t i) { return (&x)[i]; }

    // Offset the vector
    vec4 Offset(T vX, T vY, T vZ, T vW) const { return vec4(x + vX, y + vY, z + vZ, w + vW); }

    // Set the vector's components
    void Set(T vX, T vY, T vZ, T vW) {
        x = vX;
        y = vY;
        z = vZ;
        w = vW;
    }

    // Negation operator
    vec4 operator-() const {
        static_assert(std::is_signed<T>::value, "Negate is only valid for signed types");
        return vec4(-x, -y, -z, -w);
    }

    // Logical NOT operator, only for integral types
    vec4 operator!() const {
        static_assert(std::is_integral<T>::value, "Logical NOT is only valid for integral types");
        return vec4(!x, !y, !z, !w);
    }

    // Extract 2D and 3D vectors from 4D vector
    vec2<T> xy() const { return vec2<T>(x, y); }

    vec3<T> xyz() const { return vec3<T>(x, y, z); }

    vec2<T> zw() const { return vec2<T>(z, w); }

    operator vec2<T>() const { return vec2<T>(x, y); }

    operator vec3<T>() const { return vec3<T>(x, y, z); }

    // Pre-increment and pre-decrement operators
    vec4& operator++() {
        ++x;
        ++y;
        ++z;
        ++w;
        return *this;
    }

    vec4& operator--() {
        --x;
        --y;
        --z;
        --w;
        return *this;
    }

    // Post-increment and post-decrement operators
    vec4 operator++(int) {
        vec4 tmp = *this;
        ++*this;
        return tmp;
    }

    vec4 operator--(int) {
        vec4 tmp = *this;
        --*this;
        return tmp;
    }

    // Compound assignment operators
    void operator+=(T a) {
        x += a;
        y += a;
        z += a;
        w += a;
    }

    void operator+=(const vec4& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        w += v.w;
    }

    void operator-=(T a) {
        x -= a;
        y -= a;
        z -= a;
        w -= a;
    }

    void operator-=(const vec4& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        w -= v.w;
    }

    void operator*=(T a) {
        x *= a;
        y *= a;
        z *= a;
        w *= a;
    }

    void operator*=(const vec4& v) {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        w *= v.w;
    }

    void operator/=(T a) {
        x /= a;
        y /= a;
        z /= a;
        w /= a;
    }

    void operator/=(const vec4& v) {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        w /= v.w;
    }

    // Size and position operations
    vec2<T> SizeLBRT() const { return vec2<T>(z - x, w - y); }  // Considers vec4 as a rectangle with LBRT (Left, Bottom, Right, Top)

    vec2<T> pos() const { return xy(); }

    vec2<T> size() const { return zw(); }

    // Length of the vector
    T length() const { return static_cast<T>(ez::sqrt(lengthSquared())); }

    // Squared length of the vector
    T lengthSquared() const { return x * x + y * y + z * z + w * w; }

    // Normalize the vector
    T normalize() {
        T _length = length();
        if (_length < std::numeric_limits<T>::epsilon())
            return static_cast<T>(0);
        T _invLength = static_cast<T>(1) / _length;
        x *= _invLength;
        y *= _invLength;
        z *= _invLength;
        w *= _invLength;
        return _length;
    }

    // Get a normalized copy of the vector
    vec4 GetNormalized() const {
        vec4 n = *this;
        n.normalize();
        return n;
    }

    // Check if all components are zero (AND)
    bool emptyAND() const { return x == static_cast<T>(0) && y == static_cast<T>(0) && z == static_cast<T>(0) && w == static_cast<T>(0); }

    // Check if any component is zero (OR)
    bool emptyOR() const { return x == static_cast<T>(0) || y == static_cast<T>(0) || z == static_cast<T>(0) || w == static_cast<T>(0); }

    // Sum of components
    T sum() const { return x + y + z + w; }

    // Sum of absolute values of components
    T sumAbs() const { return ez::abs(x) + ez::abs(y) + ez::abs(z) + ez::abs(w); }

    // Convert to string
    std::string string(char c = ';') const { return ez::str::toStr(x) + c + ez::str::toStr(y) + c + ez::str::toStr(z) + c + ez::str::toStr(w); }

    // Minimum component
    T mini() const { return ez::mini(x, ez::mini(y, ez::mini(z, w))); }

    // Maximum component
    T maxi() const { return ez::maxi(x, ez::maxi(y, ez::maxi(z, w))); }
};

// Operators for vec4
template <typename T>
inline vec4<T> operator+(vec4<T> v, T f) {
    return vec4<T>(v.x + f, v.y + f, v.z + f, v.w + f);
}

template <typename T>
inline vec4<T> operator+(T f, vec4<T> v) {
    return vec4<T>(v.x + f, v.y + f, v.z + f, v.w + f);
}

template <typename T>
inline vec4<T> operator+(vec4<T> v, const vec4<T>& f) {
    return vec4<T>(v.x + f.x, v.y + f.y, v.z + f.z, v.w + f.w);
}

template <typename T>
inline vec4<T> operator-(vec4<T> v, T f) {
    return vec4<T>(v.x - f, v.y - f, v.z - f, v.w - f);
}

template <typename T>
inline vec4<T> operator-(T f, vec4<T> v) {
    return vec4<T>(f - v.x, f - v.y, f - v.z, f - v.w);
}

template <typename T>
inline vec4<T> operator-(vec4<T> v, const vec4<T>& f) {
    return vec4<T>(v.x - f.x, v.y - f.y, v.z - f.z, v.w - f.w);
}

template <typename T>
inline vec4<T> operator*(vec4<T> v, T f) {
    return vec4<T>(v.x * f, v.y * f, v.z * f, v.w * f);
}

template <typename T>
inline vec4<T> operator*(T f, vec4<T> v) {
    return vec4<T>(v.x * f, v.y * f, v.z * f, v.w * f);
}

template <typename T>
inline vec4<T> operator*(vec4<T> v, const vec4<T>& f) {
    return vec4<T>(v.x * f.x, v.y * f.y, v.z * f.z, v.w * f.w);
}

template <typename T>
inline vec4<T> operator/(vec4<T> v, T f) {
    return vec4<T>(v.x / f, v.y / f, v.z / f, v.w / f);
}

template <typename T>
inline vec4<T> operator/(T f, vec4<T> v) {
    return vec4<T>(f / v.x, f / v.y, f / v.z, f / v.w);
}

template <typename T>
inline vec4<T> operator/(vec4<T> v, const vec4<T>& f) {
    return vec4<T>(v.x / f.x, v.y / f.y, v.z / f.z, v.w / f.w);
}

// Comparison operators
template <typename T>
inline bool operator<(vec4<T> v, const vec4<T>& f) {
    return v.x < f.x && v.y < f.y && v.z < f.z && v.w < f.w;
}

template <typename T>
inline bool operator<(vec4<T> v, T f) {
    return v.x < f && v.y < f && v.z < f && v.w < f;
}

template <typename T>
inline bool operator<(T f, vec4<T> v) {
    return f < v.x && f < v.y && f < v.z && f < v.w;
}

template <typename T>
inline bool operator>(vec4<T> v, const vec4<T>& f) {
    return v.x > f.x && v.y > f.y && v.z > f.z && v.w > f.w;
}

template <typename T>
inline bool operator>(vec4<T> v, T f) {
    return v.x > f && v.y > f && v.z > f && v.w > f;
}

template <typename T>
inline bool operator>(T f, vec4<T> v) {
    return f > v.x && f > v.y && f > v.z && f > v.w;
}

template <typename T>
inline bool operator<=(vec4<T> v, const vec4<T>& f) {
    return v.x <= f.x && v.y <= f.y && v.z <= f.z && v.w <= f.w;
}

template <typename T>
inline bool operator<=(vec4<T> v, T f) {
    return v.x <= f && v.y <= f && v.z <= f && v.w <= f;
}

template <typename T>
inline bool operator<=(T f, vec4<T> v) {
    return f <= v.x && f <= v.y && f <= v.z && f <= v.w;
}

template <typename T>
inline bool operator>=(vec4<T> v, const vec4<T>& f) {
    return v.x >= f.x && v.y >= f.y && v.z >= f.z && v.w >= f.w;
}

template <typename T>
inline bool operator>=(vec4<T> v, T f) {
    return v.x >= f && v.y >= f && v.z >= f && v.w >= f;
}

template <typename T>
inline bool operator>=(T f, vec4<T> v) {
    return f >= v.x && f >= v.y && f >= v.z && f >= v.w;
}

template <typename T>
inline bool operator==(vec4<T> v, const vec4<T>& f) {
    return v.x == f.x && v.y == f.y && v.z == f.z && v.w == f.w;
}

template <typename T>
inline bool operator==(vec4<T> v, T f) {
    return v.x == f && v.y == f && v.z == f && v.w == f;
}

template <typename T>
inline bool operator==(T f, vec4<T> v) {
    return f == v.x && f == v.y && f == v.z && f == v.w;
}

template <typename T>
inline bool operator!=(vec4<T> v, const vec4<T>& f) {
    return v.x != f.x || v.y != f.y || v.z != f.z || v.w != f.w;
}

template <typename T>
inline bool operator!=(vec4<T> v, T f) {
    return v.x != f || v.y != f || v.z != f || v.w != f;
}

template <typename T>
inline bool operator!=(T f, vec4<T> v) {
    return f != v.x || f != v.y || f != v.z || f != v.w;
}

// Utility functions
template <typename T>
inline vec4<T> mini(vec4<T> a, vec4<T> b) {
    return vec4<T>(ez::mini(a.x, b.x), ez::mini(a.y, b.y), ez::mini(a.z, b.z), ez::mini(a.w, b.w));
}

template <typename T>
inline vec4<T> maxi(vec4<T> a, vec4<T> b) {
    return vec4<T>(ez::maxi(a.x, b.x), ez::maxi(a.y, b.y), ez::maxi(a.z, b.z), ez::maxi(a.w, b.w));
}

template <typename T>
inline vec4<T> floor(vec4<T> a) {
    return vec4<T>(ez::floor(a.x), ez::floor(a.y), ez::floor(a.z), ez::floor(a.w));
}

template <typename T>
inline vec4<T> ceil(vec4<T> a) {
    return vec4<T>(ez::ceil(a.x), ez::ceil(a.y), ez::ceil(a.z), ez::ceil(a.w));
}

template <typename T>
inline vec4<T> abs(vec4<T> a) {
    return vec4<T>(ez::abs(a.x), ez::abs(a.y), ez::abs(a.z), ez::abs(a.w));
}

template <typename T>
inline vec4<T> sign(vec4<T> a) {
    return vec4<T>(a.x < static_cast<T>(0) ? static_cast<T>(-1) : static_cast<T>(1),
                   a.y < static_cast<T>(0) ? static_cast<T>(-1) : static_cast<T>(1),
                   a.z < static_cast<T>(0) ? static_cast<T>(-1) : static_cast<T>(1),
                   a.w < static_cast<T>(0) ? static_cast<T>(-1) : static_cast<T>(1));
}

template <typename T>
inline vec4<T> sin(vec4<T> a) {
    return vec4<T>(ez::sin(a.x), ez::sin(a.y), ez::sin(a.z), ez::sin(a.w));
}

template <typename T>
inline vec4<T> cos(vec4<T> a) {
    return vec4<T>(ez::cos(a.x), ez::cos(a.y), ez::cos(a.z), ez::cos(a.w));
}

template <typename T>
inline vec4<T> tan(vec4<T> a) {
    return vec4<T>(ez::tan(a.x), ez::tan(a.y), ez::tan(a.z), ez::tan(a.w));
}

// Type aliases for common vector types
using dvec4 = vec4<double>;
using fvec4 = vec4<float>;
using f32vec4 = vec4<float>;
using f64vec4 = vec4<double>;
using bvec4 = vec4<bool>;
using ivec4 = vec4<int>;
using i8vec4 = vec4<int8_t>;
using i16vec4 = vec4<int16_t>;
using i32vec4 = vec4<int32_t>;
using i64vec4 = vec4<int64_t>;
using u8vec4 = vec4<uint8_t>;
using u16vec4 = vec4<uint16_t>;
using uvec4 = vec4<uint32_t>;
using u32vec4 = vec4<uint32_t>;
using u64vec4 = vec4<uint64_t>;

// Specialization for float validation
inline bool valid(const fvec4& a) {
    return floatIsValid(a.x) && floatIsValid(a.y) && floatIsValid(a.z) && floatIsValid(a.w);
}

// Float-specific comparison operators
inline bool operator==(const fvec4& v, const fvec4& f) {
    return isEqual(f.x, v.x) && isEqual(f.y, v.y) && isEqual(f.z, v.z) && isEqual(f.w, v.w);
}

inline bool operator!=(const fvec4& v, const fvec4& f) {
    return isDifferent(f.x, v.x) || isDifferent(f.y, v.y) || isDifferent(f.z, v.z) || isDifferent(f.w, v.w);
}

}  // namespace ez
