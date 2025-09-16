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

// ezMat2 is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <array>
#include <type_traits>
#include <cmath>
#include <initializer_list>

namespace ez {

template<typename T>
class mat2 {
    static_assert(std::is_arithmetic<T>::value, "mat2 requires arithmetic T");
public:
    static constexpr int Rows = 2;
    static constexpr int Cols = 2;

private:
    // Column-major storage: m[col * Rows + row]
    std::array<T, 4> m_data{};

public:
    /// Default: zero matrix
    mat2() = default;

    /// Diagonal constructor
    explicit mat2(T vDiagonal) {
        m_data = { vDiagonal, T(0), T(0), vDiagonal };
    }

    /// From elements (column-major order): c0r0, c0r1, c1r0, c1r1
    mat2(T c0r0, T c0r1, T c1r0, T c1r1) {
        m_data = { c0r0, c0r1, c1r0, c1r1 };
    }

    /// From std::array in column-major order
    explicit mat2(const std::array<T, 4>& vData) : m_data(vData) {}

    /// Identity factory
    static mat2 Identity() {
        return mat2(T(1));
    }

    /// Zero factory
    static mat2 Zero() {
        return mat2();
    }

    /// Element access (row, col), 0-based
    T& operator()(int vRowIndex, int vColumnIndex) {
        return m_data[static_cast<size_t>(vColumnIndex * Rows + vRowIndex)];
    }
    T operator()(int vRowIndex, int vColumnIndex) const {
        return m_data[static_cast<size_t>(vColumnIndex * Rows + vRowIndex)];
    }

    /// Raw pointer to data (column-major)
    const T* data() const { return m_data.data(); }
    T* data() { return m_data.data(); }

    /// Matrix multiplication
    mat2 operator*(const mat2& vOther) const {
        mat2 result = Zero();
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

    /// Multiply by 2D column vector (x,y)
    std::array<T, 2> mulVec(const std::array<T, 2>& v) const {
        return {
            (*this)(0,0)*v[0] + (*this)(0,1)*v[1],
            (*this)(1,0)*v[0] + (*this)(1,1)*v[1]
        };
    }

    /// Transpose
    mat2 transpose() const {
        return mat2(
            (*this)(0,0), (*this)(1,0),
            (*this)(0,1), (*this)(1,1)
        );
    }

    /// Determinant
    T determinant() const {
        return (*this)(0,0) * (*this)(1,1) - (*this)(0,1) * (*this)(1,0);
    }

    /// Inverse (if det != 0)
    mat2 inverse(T vEpsilon = T(1e-12)) const {
        T det = determinant();
        mat2 inv;
        if (std::fabs(static_cast<double>(det)) <= static_cast<double>(vEpsilon)) {
            return inv; // returns zero matrix if non-invertible
        }
        T invDet = T(1) / det;
        inv(0,0) =  (*this)(1,1) * invDet;
        inv(0,1) = -(*this)(0,1) * invDet;
        inv(1,0) = -(*this)(1,0) * invDet;
        inv(1,1) =  (*this)(0,0) * invDet;
        return inv;
    }
};

typedef mat2<float> fmat2;
typedef mat2<double> dmat2;

} // namespace ez
