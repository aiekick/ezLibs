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

// ezMat4 is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <array>
#include <type_traits>
#include <cmath>
#include <initializer_list>

namespace ez {

template<typename T>
class Mat4 {
    static_assert(std::is_arithmetic<T>::value, "Mat4 requires arithmetic T");
public:
    static constexpr int Rows = 4;
    static constexpr int Cols = 4;

private:
    // Column-major storage: m[col * Rows + row]
    std::array<T, 16> m_data{
        { T(0),T(0),T(0),T(0),
          T(0),T(0),T(0),T(0),
          T(0),T(0),T(0),T(0),
          T(0),T(0),T(0),T(0) }
    };

    // Small vector helpers (3D)
    static std::array<T,3> v3_add(const std::array<T,3>& a, const std::array<T,3>& b) {
        return { a[0]+b[0], a[1]+b[1], a[2]+b[2] };
    }
    static std::array<T,3> v3_sub(const std::array<T,3>& a, const std::array<T,3>& b) {
        return { a[0]-b[0], a[1]-b[1], a[2]-b[2] };
    }
    static std::array<T,3> v3_scale(const std::array<T,3>& a, T s) {
        return { a[0]*s, a[1]*s, a[2]*s };
    }
    static T v3_dot(const std::array<T,3>& a, const std::array<T,3>& b) {
        return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
    }
    static std::array<T,3> v3_cross(const std::array<T,3>& a, const std::array<T,3>& b) {
        return {
            a[1]*b[2] - a[2]*b[1],
            a[2]*b[0] - a[0]*b[2],
            a[0]*b[1] - a[1]*b[0]
        };
    }
    static std::array<T,3> v3_normalize(const std::array<T,3>& v, T vEpsilon = T(1e-12)) {
        double len = std::sqrt(static_cast<double>(v3_dot(v,v)));
        if (len <= static_cast<double>(vEpsilon)) return { T(0), T(0), T(0) };
        T inv = static_cast<T>(1.0 / len);
        return { v[0]*inv, v[1]*inv, v[2]*inv };
    }

public:
    Mat4() = default;

    /// Diagonal constructor
    explicit Mat4(T vDiagonal) {
        m_data = { vDiagonal, T(0), T(0), T(0),
                   T(0), vDiagonal, T(0), T(0),
                   T(0), T(0), vDiagonal, T(0),
                   T(0), T(0), T(0), vDiagonal };
    }

    /// From elements (column-major)
    Mat4(T c0r0, T c0r1, T c0r2, T c0r3,
         T c1r0, T c1r1, T c1r2, T c1r3,
         T c2r0, T c2r1, T c2r2, T c2r3,
         T c3r0, T c3r1, T c3r2, T c3r3) {
        m_data = { c0r0, c0r1, c0r2, c0r3,
                   c1r0, c1r1, c1r2, c1r3,
                   c2r0, c2r1, c2r2, c2r3,
                   c3r0, c3r1, c3r2, c3r3 };
    }

    explicit Mat4(const std::array<T, 16>& vData) : m_data(vData) {}

    static Mat4 Identity() { return Mat4(T(1)); }
    static Mat4 Zero() { return Mat4(); }

    T& operator()(int vRowIndex, int vColumnIndex) {
        return m_data[static_cast<size_t>(vColumnIndex * Rows + vRowIndex)];
    }
    T operator()(int vRowIndex, int vColumnIndex) const {
        return m_data[static_cast<size_t>(vColumnIndex * Rows + vRowIndex)];
    }

    const T* data() const { return m_data.data(); }
    T* data() { return m_data.data(); }

    Mat4 operator*(const Mat4& vOther) const {
        Mat4 result = Zero();
        for (int columnIndex = 0; columnIndex < Cols; ++columnIndex) {
            for (int rowIndex = 0; rowIndex < Rows; ++rowIndex) {
                T sum = T(0);
                for (int k = 0; k < Cols; ++k) {
                    sum += (*this)(rowIndex, k) * vOther(k, columnIndex);
                }
                result(rowIndex, columnIndex) = sum;
            }
        }
        return result;
    }

    /// Multiply by 4D column vector (x,y,z,w)
    std::array<T, 4> mulVec(const std::array<T, 4>& v) const {
        return {
            (*this)(0,0)*v[0] + (*this)(0,1)*v[1] + (*this)(0,2)*v[2] + (*this)(0,3)*v[3],
            (*this)(1,0)*v[0] + (*this)(1,1)*v[1] + (*this)(1,2)*v[2] + (*this)(1,3)*v[3],
            (*this)(2,0)*v[0] + (*this)(2,1)*v[1] + (*this)(2,2)*v[2] + (*this)(2,3)*v[3],
            (*this)(3,0)*v[0] + (*this)(3,1)*v[1] + (*this)(3,2)*v[2] + (*this)(3,3)*v[3]
        };
    }

    Mat4 transpose() const {
        return Mat4(
            (*this)(0,0), (*this)(1,0), (*this)(2,0), (*this)(3,0),
            (*this)(0,1), (*this)(1,1), (*this)(2,1), (*this)(3,1),
            (*this)(0,2), (*this)(1,2), (*this)(2,2), (*this)(3,2),
            (*this)(0,3), (*this)(1,3), (*this)(2,3), (*this)(3,3)
        );
    }

    /// Determinant (Laplace expansion via precomputed minors)
    T determinant() const {
        // Expand using first row of the transposed cofactor structure (optimized common form)
        const T a00 = (*this)(0,0), a01 = (*this)(0,1), a02 = (*this)(0,2), a03 = (*this)(0,3);
        const T a10 = (*this)(1,0), a11 = (*this)(1,1), a12 = (*this)(1,2), a13 = (*this)(1,3);
        const T a20 = (*this)(2,0), a21 = (*this)(2,1), a22 = (*this)(2,2), a23 = (*this)(2,3);
        const T a30 = (*this)(3,0), a31 = (*this)(3,1), a32 = (*this)(3,2), a33 = (*this)(3,3);

        const T b00 = a00*a11 - a01*a10;
        const T b01 = a00*a12 - a02*a10;
        const T b02 = a00*a13 - a03*a10;
        const T b03 = a01*a12 - a02*a11;
        const T b04 = a01*a13 - a03*a11;
        const T b05 = a02*a13 - a03*a12;
        const T b06 = a20*a31 - a21*a30;
        const T b07 = a20*a32 - a22*a30;
        const T b08 = a20*a33 - a23*a30;
        const T b09 = a21*a32 - a22*a31;
        const T b10 = a21*a33 - a23*a31;
        const T b11 = a22*a33 - a23*a32;

        return b00*b11 - b01*b10 + b02*b09 + b03*b08 - b04*b07 + b05*b06;
    }

    /// Inverse (returns zero matrix if non-invertible)
    Mat4 inverse(T vEpsilon = T(1e-12)) const {
        const T a00 = (*this)(0,0), a01 = (*this)(0,1), a02 = (*this)(0,2), a03 = (*this)(0,3);
        const T a10 = (*this)(1,0), a11 = (*this)(1,1), a12 = (*this)(1,2), a13 = (*this)(1,3);
        const T a20 = (*this)(2,0), a21 = (*this)(2,1), a22 = (*this)(2,2), a23 = (*this)(2,3);
        const T a30 = (*this)(3,0), a31 = (*this)(3,1), a32 = (*this)(3,2), a33 = (*this)(3,3);

        const T b00 = a00*a11 - a01*a10;
        const T b01 = a00*a12 - a02*a10;
        const T b02 = a00*a13 - a03*a10;
        const T b03 = a01*a12 - a02*a11;
        const T b04 = a01*a13 - a03*a11;
        const T b05 = a02*a13 - a03*a12;
        const T b06 = a20*a31 - a21*a30;
        const T b07 = a20*a32 - a22*a30;
        const T b08 = a20*a33 - a23*a30;
        const T b09 = a21*a32 - a22*a31;
        const T b10 = a21*a33 - a23*a31;
        const T b11 = a22*a33 - a23*a32;

        const T det = b00*b11 - b01*b10 + b02*b09 + b03*b08 - b04*b07 + b05*b06;
        if (std::fabs(static_cast<double>(det)) <= static_cast<double>(vEpsilon)) {
            return Mat4::Zero();
        }
        const T invDet = T(1) / det;

        Mat4 inv;
        inv(0,0) = ( a11*b11 - a12*b10 + a13*b09) * invDet;
        inv(0,1) = (-a01*b11 + a02*b10 - a03*b09) * invDet;
        inv(0,2) = ( a31*b05 - a32*b04 + a33*b03) * invDet;
        inv(0,3) = (-a21*b05 + a22*b04 - a23*b03) * invDet;

        inv(1,0) = (-a10*b11 + a12*b08 - a13*b07) * invDet;
        inv(1,1) = ( a00*b11 - a02*b08 + a03*b07) * invDet;
        inv(1,2) = (-a30*b05 + a32*b02 - a33*b01) * invDet;
        inv(1,3) = ( a20*b05 - a22*b02 + a23*b01) * invDet;

        inv(2,0) = ( a10*b10 - a11*b08 + a13*b06) * invDet;
        inv(2,1) = (-a00*b10 + a01*b08 - a03*b06) * invDet;
        inv(2,2) = ( a30*b04 - a31*b02 + a33*b00) * invDet;
        inv(2,3) = (-a20*b04 + a21*b02 - a23*b00) * invDet;

        inv(3,0) = (-a10*b09 + a11*b07 - a12*b06) * invDet;
        inv(3,1) = ( a00*b09 - a01*b07 + a02*b06) * invDet;
        inv(3,2) = (-a30*b03 + a31*b01 - a32*b00) * invDet;
        inv(3,3) = ( a20*b03 - a21*b01 + a22*b00) * invDet;

        return inv;
    }    
	
	/// Vulkan bias matrix to convert from OpenGL clip space
    /// - Flips Y
    /// - Remaps z from [-1,1] (GL) to [0,1] (Vulkan)
    static Mat4 BiasVulkan() {
        Mat4 r = Mat4::Identity();
        r(1,1) = T(-1);     // flip Y
        r(2,2) = T(0.5);    // z scale
        r(3,2) = T(0.5);    // z offset
        return r;
    }


    /// Perspective matrix for Opengl (expects fovY in radians)
    static Mat4 PerspectiveOpengl(T vFovYRadians, T vAspect, T vNear, T vFar) {
        // Right-handed, OpenGL-style clip space
        const T f = T(1) / static_cast<T>(std::tan(static_cast<double>(vFovYRadians) * 0.5));
        Mat4 r;
        r(0, 0) = f / vAspect;
        r(1, 1) = f;
        r(2, 2) = (vFar + vNear) / (vNear - vFar);
        r(2, 3) = T(-1);
        r(3, 2) = (T(2) * vFar * vNear) / (vNear - vFar);
        return r;
    }

    /// Perspective matrix for Vulkan (NDC z in [0,1] + Y flipped)
    static Mat4 PerspectiveVulkan(T vFovYRadians, T vAspect, T vNear, T vFar) {
        Mat4 projGL = PerspectiveOpengl(vFovYRadians, vAspect, vNear, vFar);
        return BiasVulkan() * projGL;
    }

    // --- Orthographic projections (OpenGL-style: z in [-1, 1]) ---
    // Off-center orthographic
    static Mat4 OrthoOpengl(T vLeft, T vRight, T vBottom, T vTop, T vNear, T vFar) {
        Mat4 r = Mat4::Zero();
        r(0, 0) = T(2) / (vRight - vLeft);
        r(1, 1) = T(2) / (vTop - vBottom);
        r(2, 2) = T(-2) / (vFar - vNear);
        r(3, 3) = T(1);
        r(3, 0) = -(vRight + vLeft) / (vRight - vLeft);
        r(3, 1) = -(vTop + vBottom) / (vTop - vBottom);
        r(3, 2) = -(vFar + vNear) / (vFar - vNear);
        return r;
    }

    // Symmetric orthographic (helper)
    static Mat4 OrthoSymmetricOpengl(T vWidth, T vHeight, T vNear, T vFar) {
        const T halfW = vWidth * T(0.5);
        const T halfH = vHeight * T(0.5);
        return Ortho(-halfW, +halfW, -halfH, +halfH, vNear, vFar);
    }

    // Vulkan orthographic (NDC z in [0,1] + Y flip)
    static Mat4 OrthoVulkan(T vLeft, T vRight, T vBottom, T vTop, T vNear, T vFar) {
        return BiasVulkan() * OrthoOpengl(vLeft, vRight, vBottom, vTop, vNear, vFar);
    }
    static Mat4 OrthoSymmetricVulkan(T vWidth, T vHeight, T vNear, T vFar) {
        return OrthoVulkan(-vWidth * T(0.5), vWidth * T(0.5),
            -vHeight * T(0.5), vHeight * T(0.5),
            vNear, vFar);
    }

    // --- Perspective frustum projections (OpenGL-style: z in [-1, 1]) ---
    // Off-center perspective frustum (matches glFrustum semantics)
    static Mat4 FrustumOpengl(T vLeft, T vRight, T vBottom, T vTop, T vNear, T vFar) {
        Mat4 r = Mat4::Zero();
        r(0, 0) = (T(2) * vNear) / (vRight - vLeft);
        r(1, 1) = (T(2) * vNear) / (vTop - vBottom);
        r(0, 2) = (vRight + vLeft) / (vRight - vLeft);
        r(1, 2) = (vTop + vBottom) / (vTop - vBottom);
        r(2, 2) = -(vFar + vNear) / (vFar - vNear);
        r(2, 3) = T(-1);
        r(3, 2) = -(T(2) * vFar * vNear) / (vFar - vNear);
        return r;
    }

    // Vulkan frustum (NDC z in [0,1] + Y flip)
    static Mat4 FrustumVulkan(T vLeft, T vRight, T vBottom, T vTop, T vNear, T vFar) {
        return BiasVulkan() * FrustumOpengl(vLeft, vRight, vBottom, vTop, vNear, vFar);
    }

    /// LookAt matrix (right-handed)
    static Mat4 LookAt(const std::array<T,3>& vEye,
                       const std::array<T,3>& vCenter,
                       const std::array<T,3>& vUp) {
        const std::array<T,3> forward = v3_normalize(v3_sub(vCenter, vEye));
        const std::array<T,3> side = v3_normalize(v3_cross(forward, vUp));
        const std::array<T,3> up = v3_cross(side, forward);

        Mat4 r = Mat4::Identity();
        r(0,0) = side[0];   r(1,0) = side[1];   r(2,0) = side[2];
        r(0,1) = up[0];     r(1,1) = up[1];     r(2,1) = up[2];
        r(0,2) = -forward[0]; r(1,2) = -forward[1]; r(2,2) = -forward[2];

        r(3,0) = - (side[0]*vEye[0] + side[1]*vEye[1] + side[2]*vEye[2]);
        r(3,1) = - (up[0]*vEye[0] + up[1]*vEye[1] + up[2]*vEye[2]);
        r(3,2) =   (forward[0]*vEye[0] + forward[1]*vEye[1] + forward[2]*vEye[2]);

        return r;
    }
};

} // namespace ez
