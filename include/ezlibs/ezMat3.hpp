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

// ezMat3 is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <array>
#include <type_traits>
#include <cmath>
#include <initializer_list>

namespace ez {

template<typename T>
class mat3 {
    static_assert(std::is_arithmetic<T>::value, "mat3 requires arithmetic T");
public:
    static constexpr int Rows = 3;
    static constexpr int Cols = 3;

private:
    // Column-major storage: m[col * Rows + row]
    std::array<T, 9> m_data{};

    static std::array<T,3> cross(const std::array<T,3>& a, const std::array<T,3>& b) {
        return {
            a[1]*b[2] - a[2]*b[1],
            a[2]*b[0] - a[0]*b[2],
            a[0]*b[1] - a[1]*b[0]
        };
    }
    static T dot(const std::array<T,3>& a, const std::array<T,3>& b) {
        return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
    }
    static std::array<T,3> normalize(const std::array<T,3>& v, T vEpsilon = T(1e-12)) {
        double len = std::sqrt(static_cast<double>(dot(v,v)));
        if (len <= static_cast<double>(vEpsilon)) return { T(0), T(0), T(0) };
        T inv = static_cast<T>(1.0 / len);
        return { v[0]*inv, v[1]*inv, v[2]*inv };
    }

public:
    /// Default: zero matrix
    mat3() = default;

    /// Diagonal constructor
    explicit mat3(T vDiagonal) {
        m_data = { vDiagonal, T(0), T(0),
                   T(0), vDiagonal, T(0),
                   T(0), T(0), vDiagonal };
    }

    /// From elements (column-major)
    mat3(T c0r0, T c0r1, T c0r2,
         T c1r0, T c1r1, T c1r2,
         T c2r0, T c2r1, T c2r2) {
        m_data = { c0r0, c0r1, c0r2,
                   c1r0, c1r1, c1r2,
                   c2r0, c2r1, c2r2 };
    }

    explicit mat3(const std::array<T, 9>& vData) : m_data(vData) {}

    static mat3 Identity() { return mat3(T(1)); }
    static mat3 Zero() { return mat3(); }

    T& operator()(int vRowIndex, int vColumnIndex) {
        return m_data[static_cast<size_t>(vColumnIndex * Rows + vRowIndex)];
    }
    T operator()(int vRowIndex, int vColumnIndex) const {
        return m_data[static_cast<size_t>(vColumnIndex * Rows + vRowIndex)];
    }

    const T* data() const { return m_data.data(); }
    T* data() { return m_data.data(); }

    mat3 operator*(const mat3& vOther) const {
        mat3 result = Zero();
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

    /// Multiply by 3D column vector
    std::array<T, 3> mulVec(const std::array<T, 3>& v) const {
        return {
            (*this)(0,0)*v[0] + (*this)(0,1)*v[1] + (*this)(0,2)*v[2],
            (*this)(1,0)*v[0] + (*this)(1,1)*v[1] + (*this)(1,2)*v[2],
            (*this)(2,0)*v[0] + (*this)(2,1)*v[1] + (*this)(2,2)*v[2]
        };
    }

    mat3 transpose() const {
        return mat3(
            (*this)(0,0), (*this)(1,0), (*this)(2,0),
            (*this)(0,1), (*this)(1,1), (*this)(2,1),
            (*this)(0,2), (*this)(1,2), (*this)(2,2)
        );
    }

    T determinant() const {
        const T a = (*this)(0,0), b = (*this)(0,1), c = (*this)(0,2);
        const T d = (*this)(1,0), e = (*this)(1,1), f = (*this)(1,2);
        const T g = (*this)(2,0), h = (*this)(2,1), i = (*this)(2,2);
        return a*(e*i - f*h) - b*(d*i - f*g) + c*(d*h - e*g);
    }

    mat3 inverse(T vEpsilon = T(1e-12)) const {
        mat3 inv;
        T det = determinant();
        if (std::fabs(static_cast<double>(det)) <= static_cast<double>(vEpsilon)) {
            return inv; // zero matrix if non-invertible
        }
        const T a = (*this)(0,0), b = (*this)(0,1), c = (*this)(0,2);
        const T d = (*this)(1,0), e = (*this)(1,1), f = (*this)(1,2);
        const T g = (*this)(2,0), h = (*this)(2,1), i = (*this)(2,2);

        inv(0,0) =  (e*i - f*h);
        inv(0,1) = -(b*i - c*h);
        inv(0,2) =  (b*f - c*e);

        inv(1,0) = -(d*i - f*g);
        inv(1,1) =  (a*i - c*g);
        inv(1,2) = -(a*f - c*d);

        inv(2,0) =  (d*h - e*g);
        inv(2,1) = -(a*h - b*g);
        inv(2,2) =  (a*e - b*d);

        T invDet = T(1) / det;
        for (int columnIndex = 0; columnIndex < Cols; ++columnIndex) {
            for (int rowIndex = 0; rowIndex < Rows; ++rowIndex) {
                inv(rowIndex, columnIndex) *= invDet;
            }
        }
        return inv;
    }
};

typedef mat3<float> fmat3;
typedef mat3<double> dmat3;

} // namespace ez
