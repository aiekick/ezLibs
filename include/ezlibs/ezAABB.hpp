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

// EzAABB is part od the EzLibs project : https://github.com/aiekick/EzLibs.git

namespace ez {

template <typename T>
struct AABB  // copy of b2AABB struct
{
    vec2<T> lowerBound;  ///< the lower left vertex
    vec2<T> upperBound;  ///< the upper right vertex

    AABB() : lowerBound((T)0), upperBound((T)0) {
    }
    AABB(vec2<T> vlowerBound, vec2<T> vUpperBound) : lowerBound(vlowerBound), upperBound(vUpperBound) {
    }
    AABB(vec4<T> vVec4)
    //: lowerBound(vlowerBound), upperBound(vUpperBound)
    {
        lowerBound.x = vVec4.x;
        lowerBound.y = vVec4.y;
        upperBound.x = vVec4.z;
        upperBound.y = vVec4.w;
    }
    AABB(std::string vec, char c)  // may be in format "0.2f,0.3f,0.4f,0.8f" left, bottom, right, top
    {
        std::vector<float> result = str::stringToNumberVector<float>(vec, c);
        const size_t s = result.size();
        if (s > 0)
            lowerBound.x = result[0];
        if (s > 1)
            lowerBound.y = result[1];
        if (s > 2)
            upperBound.x = result[2];
        if (s > 3)
            upperBound.y = result[3];
    }

#ifdef COCOS2D
    AABB(cocos2d::Rect rect) : lowerBound(rect.origin), upperBound(lowerBound + rect.size) {
    }
#endif

#ifdef USE_BOX2D
    AABB(const b2AABB& aabb) : lowerBound(aabb.lowerBound), upperBound(aabb.upperBound) {
    }
#endif

    /// Verify that the bounds are sorted.
    // bool IsValid() const;

    /// Add a vector to this vector.
    void operator+=(const vec2<T>& v) {
        lowerBound += v;
        upperBound += v;
    }

    /// Subtract a vector from this vector.
    void operator-=(const vec2<T>& v) {
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
    vec2<T> GetCenter() const {
        return (lowerBound + upperBound) / (T)2;
    }

    /// Get the extents of the AABB (half-widths).
    vec2<T> GetExtents() const {
        return (upperBound - lowerBound) / (T)2;
    }

    /// Get the perimeter length
    T GetPerimeter() const {
        const float wx = upperBound.x - lowerBound.x;
        const float wy = upperBound.y - lowerBound.y;
        return (T)2 * (wx + wy);
    }

    /// Combine an AABB into this one.
    void Combine(const AABB<T>& aabb) {
        lowerBound = mini(lowerBound, aabb.lowerBound);
        upperBound = maxi(upperBound, aabb.upperBound);
    }

    /// Combine two AABBs into this one.
    void Combine(const AABB<T>& aabb1, const AABB<T>& aabb2) {
        lowerBound = mini(aabb1.lowerBound, aabb2.lowerBound);
        upperBound = maxi(aabb1.upperBound, aabb2.upperBound);
    }

    /// Combine a point into this one.
    void Combine(const vec2<T>& pt) {
        lowerBound.x = mini(lowerBound.x, pt.x);
        lowerBound.y = mini(lowerBound.y, pt.y);
        upperBound.x = maxi(upperBound.x, pt.x);
        upperBound.y = maxi(upperBound.y, pt.y);
    }

    /// Set vlowerBound at mini and vUpperBound at maxi
    void Set(vec2<T> vlowerBound, vec2<T> vUpperBound) {
        lowerBound.x = mini(vlowerBound.x, vUpperBound.x);
        lowerBound.y = mini(vlowerBound.y, vUpperBound.y);
        upperBound.x = maxi(vlowerBound.x, vUpperBound.x);
        upperBound.y = maxi(vlowerBound.y, vUpperBound.y);
    }

    /// Does this aabb contain the provided AABB.
    bool Contains(const AABB<T>& aabb) const {
        bool result = true;
        result &= lowerBound.x <= aabb.lowerBound.x;
        result &= lowerBound.y <= aabb.lowerBound.y;
        result &= aabb.upperBound.x <= upperBound.x;
        result &= aabb.upperBound.y <= upperBound.y;
        return result;
    }

    /// Does this aabb contain the provided vec2<T>.
    bool ContainsPoint(const vec2<T>& pt) const {
        bool result = true;
        result &= lowerBound.x <= pt.x;
        result &= lowerBound.y <= pt.y;
        result &= pt.x <= upperBound.x;
        result &= pt.y <= upperBound.y;
        return result;
    }

    bool Intersects(const AABB<T>& other) {
        bool result = true;
        result |= lowerBound.x <= other.lowerBound.x;
        result |= lowerBound.y <= other.lowerBound.y;
        result |= other.upperBound.x <= upperBound.x;
        result |= other.upperBound.y <= upperBound.y;
        return result;
    }
#ifdef USE_BOX2D
    b2AABB Tob2AABB() {
        b2AABB v;
        v.lowerBound = lowerBound.Tob2Vec2();
        v.upperBound = upperBound.Tob2Vec2();
        return v;
    }
#endif
    vec2<T> Size() {
        return vec2<T>(upperBound - lowerBound);
    }

#ifdef USE_IMGUI
    ImVec4 ToImVec4() {
        const ImVec4 v = ImVec4(lowerBound.x, lowerBound.y, upperBound.x, upperBound.y);
        return v;
    }
#endif
};
typedef AABB<int> iAABB;
typedef AABB<double> dAABB;
typedef AABB<float> fAABB;

/// Add a float to a AABB.
template <typename T>
inline AABB<T> operator+(const AABB<T>& v, float f) {
    return AABB<T>(v.lowerBound + f, v.upperBound + f);
}

/// Add a AABB to a AABB.
template <typename T>
inline AABB<T> operator+(const AABB<T>& v, AABB<T> f) {
    return AABB<T>(v.lowerBound + f.lowerBound, v.upperBound + f.upperBound);
}

/// Substract a float from a AABB.
template <typename T>
inline AABB<T> operator-(const AABB<T>& v, float f) {
    return AABB<T>(v.lowerBound - f, v.upperBound - f);
}

/// Substract a AABB to a AABB.
template <typename T>
inline AABB<T> operator-(const AABB<T>& v, AABB<T> f) {
    return AABB<T>(v.lowerBound - f.lowerBound, v.upperBound - f.upperBound);
}

/// Multiply a float with a AABB.
template <typename T>
inline AABB<T> operator*(const AABB<T>& v, float f) {
    return AABB<T>(v.lowerBound * f, v.upperBound * f);
}

/// Multiply a AABB with a AABB.
template <typename T>
inline AABB<T> operator*(const AABB<T>& v, AABB<T> f) {
    return AABB<T>(v.lowerBound * f.lowerBound, v.upperBound * f.upperBound);
}

/// Divide a AABB by a float.
template <typename T>
inline AABB<T> operator/(const AABB<T>& v, float f) {
    return AABB<T>(v.lowerBound / f, v.upperBound / f);
}

/// Divide a AABB by a float.
template <typename T>
inline AABB<T> operator/(AABB<T>& v, float f) {
    return AABB<T>(v.lowerBound / f, v.upperBound / f);
}

/// Divide a AABB by a AABB.
template <typename T>
inline AABB<T> operator/(const AABB<T>& v, AABB<T> f) {
    return AABB<T>(v.lowerBound / f.lowerBound, v.upperBound / f.upperBound);
}

template <typename T>
inline vec4<T> vecFromAABB(AABB<T> aabb) {
    return vec4<T>(aabb.lowerBound.x, aabb.lowerBound.y, aabb.upperBound.x, aabb.upperBound.y);
}

}  // namespace ez
