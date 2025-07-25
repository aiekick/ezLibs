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

// ezGL is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#pragma once

#ifdef OPENGL_LOADER
#include OPENGL_LOADER
#endif  // OPENGL_LOADER

#ifdef PROFILER_INCLUDE
#include PROFILER_INCLUDE
#endif  // PROFILER_SCOPED

/*#ifdef PROFILER_SCOPED
#define PROFILER_SCOPED
#endif  // PROFILER_SCOPED*/

/*#ifdef PROFILER_SCOPED_PTR
#define PROFILER_SCOPED_PTR
#endif  // PROFILER_SCOPED_PTR*/

#ifdef MSVC
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#include <ezlibs/ezStr.hpp>
#include <ezlibs/ezLog.hpp>

#include <string>

namespace ez {
namespace gl {

static inline void checkGLErrors(const char* vFile, const char* vFunc, const int& vLine) {
#ifdef _DEBUG
    const GLenum err(glGetError());
    if (err != GL_NO_ERROR) {
        std::string error;
        switch (err) {
            case GL_INVALID_OPERATION: error = "INVALID_OPERATION"; break;
            case GL_INVALID_ENUM: error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE: error = "INVALID_VALUE"; break;
            case GL_OUT_OF_MEMORY: error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
            case GL_STACK_UNDERFLOW: error = "GL_STACK_UNDERFLOW"; break;
            case GL_STACK_OVERFLOW: error = "GL_STACK_OVERFLOW"; break;
        }
        LogVarLightError("[%s][%s][%i] GL Errors : %s", vFile, vFunc, vLine, error.c_str());
    }
#else
    (void)vFile;
    (void)vFunc;
    (void)vLine;
#endif
}

}  // namespace gl
}  // namespace ez

#define CheckGLErrors ez::gl::checkGLErrors(__FILE__, __FUNCTION__, __LINE__)

// the order is important
// so dont reorder these includes

// 'auto' mean auto configuration
// uniform params from code
// fbo params from code
// like SoGLSL

#include "texture.hpp"
#include "fbo.hpp"
#include "bufferBlock.hpp"
#include "mesh.hpp"
// #include "meshVfx.hpp"
#include "quadMesh.hpp"
#include "procMesh.hpp"
#include "uniforms.hpp"
#include "shader.hpp"
#include "shaderauto.hpp"
#include "program.hpp"
#include "programauto.hpp"
#include "quadVfx.hpp"
#include "quadVfxauto.hpp"
