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

// ezPlane is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

namespace ez {

template <typename T>
class plane {
public:
    vec3<T> n;
    T d;

public:
    plane() = default;
    plane(vec3<T> vp) : n(vp.GetNormalized()), d(dotS(vp, n)) {
    }
    plane(vec3<T> vp, vec3<T> vn) : n(vn), d(dotS(vp, n)) {
    }
    plane(vec3<T> a, vec3<T> b, vec3<T> c) {
        auto AB = b - a;
        auto AC = c - a;
        n = cCross(AC, AB).GetNormalized();
        d = dotS(a, n);
    }
    plane(vec3<T> vn, double vd) : n(vn), d(vd) {
    }
};

typedef plane<double> dplane;
typedef plane<float> fplane;

template <typename T>
inline bool get_plane_intersection(const plane<T>& a, const plane<T>& b, const plane<T>& c, vec3<T>& out_res) {
    vec3<T> m1 = vec3<T>(a.n.x, b.n.x, c.n.x);
    vec3<T> m2 = vec3<T>(a.n.y, b.n.y, c.n.y);
    vec3<T> m3 = vec3<T>(a.n.z, b.n.z, c.n.z);
    vec3<T> d = vec3<T>(a.d, b.d, c.d);

    vec3<T> u = cCross<T>(m2, m3);
    T denom = dotS<T>(m1, u);

    if (fabs(denom) < std::numeric_limits<T>::epsilon()) {
        // Planes don't actually intersect in a point
        // Throw exception maybe?
        return false;
    }

    vec3<T> v = cCross<T>(m1, d);
    out_res.x = dotS<T>(d, u) / denom;
    out_res.y = dotS<T>(m3, v) / denom;
    out_res.z = -dotS<T>(m2, v) / denom;

    return true;
}

template <typename T>
inline bool is_on_plane(const plane<T>& pln, const vec3<T>& p) {
    return (fabs(dotS<T>(pln.n, p) - pln.d) < std::numeric_limits<T>::epsilon());
}

}  // namespace ez
