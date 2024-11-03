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

// EzQuat is part od the EzLibs project : https://github.com/aiekick/EzLibs.git

namespace ez {
    	
template <typename T>
struct quat {
    // x,y,z => axis
    // w => angle
    T x, y, z, w;
    quat() : x((T)0), y((T)0), z((T)0), w((T)1) {
    }
    quat(T xyzw) : x(xyzw), y(xyzw), z(xyzw), w(xyzw) {
    }
    quat(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {
    }
    T getTx() {
        return x * w;
    }
    T getTy() {
        return y * w;
    }
    T getTz() {
        return z * w;
    }
    void add(const quat<T>& vQuat) {
        x += vQuat.x;
        y += vQuat.y;
        z += vQuat.z;
        w += vQuat.w;
    }
    void sub(const quat<T>& vQuat) {
        x -= vQuat.x;
        y -= vQuat.y;
        z -= vQuat.z;
        w -= vQuat.w;
    }
    void mul(const quat<T>& vQuat) {
        const T tx = x, ty = y, tz = z, tw = w;
        x = tx * vQuat.w + ty * vQuat.z - tz * vQuat.y + tw * vQuat.x;
        y = -tx * vQuat.z + ty * vQuat.w + tz * vQuat.x + tw * vQuat.y;
        z = tx * vQuat.y - ty * vQuat.x + tz * vQuat.w + tw * vQuat.z;
        w = -tx * vQuat.x - ty * vQuat.y - tz * vQuat.z + tw * vQuat.w;
    }
    void conjugate(const quat<T>& vQuat) {
        x = -vQuat.x;
        y = -vQuat.y;
        z = -vQuat.z;
        w = vQuat.w;
    }
    void normalize() {
        static_assert(std::is_floating_point<T>::value, "Only valid for floating point types");
        // to do
    }
    void scale(T s) {
        x *= s;
        y *= s;
        z *= s;
        w *= s;
    }
};
typedef quat<float> fquat;
typedef quat<double> dquat;

}  // namespace ez
