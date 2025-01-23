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

// ezGui is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <vector>

#include <ezlibs/ezTools.hpp>

namespace ez {

class Gui {
public:

    struct Vertex {
        fvec2 pos;
        fvec2 uv;
        fvec4 color;
        Vertex() = default;
        Vertex(fvec2 vPos, fvec2 vUV, fvec4 vColor) : pos(vPos), uv(vUV), color(vColor) {
        }
    };

public:
    bool Begin(const std::string& vLabel, const bool* vOpened = nullptr, const fvec2& vSize = fvec2(0.0f, 0.0f)) {
    
    }

    void End() {
    
    }
};

}  // namespace ez
