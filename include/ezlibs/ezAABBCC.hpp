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

// EzAABBCC is part od the EzLibs project : https://github.com/aiekick/EzLibs.git

namespace ez {

template <typename T>
struct AABBCC  // copy of b2AABB struct
{
    vec3<T> lowerBound;  ///< the lower left vertex
    vec3<T> upperBound;  ///< the upper right vertex

    AABBCC() : lowerBound((T)0), upperBound((T)0) {
    }
    AABBCC(vec3<T> vlowerBound, vec3<T> vUpperBound)
    //: lowerBound(vlowerBound), upperBound(vUpperBound)
    {
        lowerBound.x = mini(vlowerBound.x, vUpperBound.x);
        lowerBound.y = mini(vlowerBound.y, vUpperBound.y);
        lowerBound.z = mini(vlowerBound.z, vUpperBound.z);
        upperBound.x = maxi(vlowerBound.x, vUpperBound.x);
        upperBound.y = maxi(vlowerBound.y, vUpperBound.y);
        upperBound.z = maxi(vlowerBound.z, vUpperBound.z);
    }

    /// Verify that the bounds are sorted.
    // bool IsValid() const;

    /// Add a vector to this vector.
    void operator+=(const vec3<T>& v) {
        lowerBound += v;
        upperBound += v;
    }

    /// Subtract a vector from this vector.
    void operator-=(const vec3<T>& v) {
        lowerBound -= v;
        upperBound -= v;
    }

    /// Multiply this vector by a scalar.
    void operator*=(T a) {
        lowerBound *= a;
        upperBound *= a;
    }

    /// Divide this vector by a scalar.
    void operator/=(T a) {
        lowerBound /= a;
        upperBound /= a;
    }

    /// Get the center of the AABB.
    vec3<T> GetCenter() const {
        return (lowerBound + upperBound) / (T)2;
    }

    /// Get the extents of the AABB (half-widths).
    vec3<T> GetExtents() const {
        return (upperBound - lowerBound) / (T)2;
    }

    /// Get the perimeter length
    T GetPerimeter() const {
        const float wx = upperBound.x - lowerBound.x;
        const float wy = upperBound.y - lowerBound.y;
        const float wz = upperBound.z - lowerBound.z;
        return (T)2 * (wx + wy + wz);
    }

    /// Combine an AABB into this one.
    void Combine(const AABBCC<T>& aabb) {
        lowerBound = mini(lowerBound, aabb.lowerBound);
        upperBound = maxi(upperBound, aabb.upperBound);
    }

    /// Combine two AABBs into this one.
    void Combine(const AABBCC<T>& aabb1, const AABBCC<T>& aabb2) {
        lowerBound = mini(aabb1.lowerBound, aabb2.lowerBound);
        upperBound = maxi(aabb1.upperBound, aabb2.upperBound);
    }

    /// Combine a point into this one.
    void Combine(const vec3<T>& pt) {
        lowerBound.x = mini(lowerBound.x, pt.x);
        lowerBound.y = mini(lowerBound.y, pt.y);
        lowerBound.z = mini(lowerBound.z, pt.z);
        upperBound.x = maxi(upperBound.x, pt.x);
        upperBound.y = maxi(upperBound.y, pt.y);
        upperBound.z = maxi(upperBound.z, pt.z);
    }

    /// Does this aabb contain the provided AABB.
    bool Contains(const AABBCC<T>& aabb) const {
        bool result = true;
        result &= lowerBound.x <= aabb.lowerBound.x;
        result &= lowerBound.y <= aabb.lowerBound.y;
        result &= lowerBound.z <= aabb.lowerBound.z;
        result &= aabb.upperBound.x <= upperBound.x;
        result &= aabb.upperBound.y <= upperBound.y;
        result &= aabb.upperBound.z <= upperBound.z;
        return result;
    }

    /// Does this aabb contain the provided vec2<T>.
    bool ContainsPoint(const vec3<T>& pt) const {
        bool result = true;
        result &= lowerBound.x <= pt.x;
        result &= lowerBound.y <= pt.y;
        result &= lowerBound.z <= pt.z;
        result &= pt.x <= upperBound.x;
        result &= pt.y <= upperBound.y;
        result &= pt.z <= upperBound.z;
        return result;
    }

    bool Intersects(const AABBCC<T>& other) const {
        bool result = true;
        result |= lowerBound.x <= other.lowerBound.x;
        result |= lowerBound.y <= other.lowerBound.y;
        result |= lowerBound.z <= other.lowerBound.z;
        result |= other.upperBound.x <= upperBound.x;
        result |= other.upperBound.y <= upperBound.y;
        result |= other.upperBound.z <= upperBound.z;
        return result;
    }

    const vec3<T> Size() const {
        return vec3<T>(upperBound - lowerBound);
    }
};
typedef AABBCC<double> dAABBCC;
typedef AABBCC<float> fAABBCC;

/// Add a float to a AABBCC<T>.
template <typename T>
inline AABBCC<T> operator+(const AABBCC<T>& v, float f) {
    return AABBCC<T>(v.lowerBound + f, v.upperBound + f);
}

/// Add a AABBCC<T> to a AABBCC<T>.
template <typename T>
inline AABBCC<T> operator+(const AABBCC<T>& v, AABBCC<T> f) {
    return AABBCC<T>(v.lowerBound + f.lowerBound, v.upperBound + f.upperBound);
}

/// Substract a float from a AABBCC<T>.
template <typename T>
inline AABBCC<T> operator-(const AABBCC<T>& v, float f) {
    return AABBCC<T>(v.lowerBound - f, v.upperBound - f);
}

/// Substract a AABBCC<T> to a AABBCC<T>.
template <typename T>
inline AABBCC<T> operator-(const AABBCC<T>& v, AABBCC<T> f) {
    return AABBCC<T>(v.lowerBound - f.lowerBound, v.upperBound - f.upperBound);
}

/// Multiply a float with a AABBCC<T>.
template <typename T>
inline AABBCC<T> operator*(const AABBCC<T>& v, float f) {
    return AABBCC<T>(v.lowerBound * f, v.upperBound * f);
}

/// Multiply a AABBCC<T> with a AABBCC<T>.
template <typename T>
inline AABBCC<T> operator*(const AABBCC<T>& v, AABBCC<T> f) {
    return AABBCC<T>(v.lowerBound * f.lowerBound, v.upperBound * f.upperBound);
}

/// Divide a AABBCC<T> by a float.
template <typename T>
inline AABBCC<T> operator/(const AABBCC<T>& v, float f) {
    return AABBCC<T>(v.lowerBound / f, v.upperBound / f);
}

/// Divide a AABBCC<T> by a float.
template <typename T>
inline AABBCC<T> operator/(AABBCC<T>& v, float f) {
    return AABBCC<T>(v.lowerBound / f, v.upperBound / f);
}

/// Divide a AABBCC<T> by a AABBCC<T>.
template <typename T>
inline AABBCC<T> operator/(const AABBCC<T>& v, AABBCC<T> f) {
    return AABBCC<T>(v.lowerBound / f.lowerBound, v.upperBound / f.upperBound);
}


}  // namespace ez
