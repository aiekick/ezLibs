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

// ezFile is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

namespace ez {
namespace imgui {}   // namespace imgui
namespace implot {}  // namespace implot
}  // namespace ez

#include <cstdint>
#include <ezlibs/ezVec2.hpp>
#include <ezlibs/ezVec4.hpp>

#ifndef IM_VEC2_CLASS_EXTRA
#define IM_VEC2_CLASS_EXTRA                        \
    ImVec2(const float v) : x(v), y(v) {}          \
    ImVec2(const ez::fvec2& v) : x(v.x), y(v.y) {} \
    ImVec2(const ez::dvec2& v) : x(static_cast<float>(v.x)), y(static_cast<float>(v.y)) {}
#endif

#ifndef IM_VEC4_CLASS_EXTRA
#define IM_VEC4_CLASS_EXTRA                                        \
    ImVec4(const float v) : x(v), y(v), z(v), w(v) {}              \
    ImVec4(const ez::fvec4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {} \
    ImVec4(const ez::dvec4& v) : x(static_cast<float>(v.x)), y(static_cast<float>(v.y)), z(static_cast<float>(v.z)), w(static_cast<float>(v.w)) {}
#endif

#ifndef IM_PLOT_RECT_CLASS_EXTRA
#define IM_PLOT_RECT_CLASS_EXTRA \
    ImPlotRect(const ez::dvec4& v) { \
        X.Min = v.x;                 \
        X.Max = v.y;                 \
        Y.Min = v.z;                 \
        Y.Max = v.w;                 \
    }
#endif

#ifdef IMGUI_API
enum ImGuiKey : int32_t;
inline std::istream& operator>>(std::istream& vIn, ImGuiKey& vType) {
    int32_t key{0};
    vIn >> key;
    vType = static_cast<ImGuiKey>(vType);
    return vIn;
}

inline std::ostream& operator<<(std::ostream& vOut, const ImGuiKey vType) {
    vOut << static_cast<int32_t>(vType);
    return vOut;
}

#endif  // IMGUI_API
