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

// ezGL is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include "ezGL.hpp"
#include <array>
#include <vector>
#include <memory>

namespace ez {
namespace gl {

namespace detail {

// Replace if-constexpr chain with template specializations
template <typename T>
struct GLTypeOf;

// Par défaut, le type scalaire c'est T lui-même
template <typename T>
struct ScalarType {
    using type = T;
};

// Spécialisations pour tes types vec
template <>
struct ScalarType<ez::fvec4> {
    using type = float;
};
template <>
struct ScalarType<ez::fvec3> {
    using type = float;
};
template <>
struct ScalarType<ez::fvec2> {
    using type = float;
};
template <>
struct ScalarType<ez::u8vec4> {
    using type = uint8_t;
};

template <>
struct GLTypeOf<uint8_t> {
    static const GLenum value = GL_UNSIGNED_BYTE;
};
template <>
struct GLTypeOf<float> {
    static const GLenum value = GL_FLOAT;
};
template <>
struct GLTypeOf<int32_t> {
    static const GLenum value = GL_INT;
};
template <>
struct GLTypeOf<uint32_t> {
    static const GLenum value = GL_UNSIGNED_INT;
};
template <>
struct GLTypeOf<int16_t> {
    static const GLenum value = GL_SHORT;
};
template <>
struct GLTypeOf<uint16_t> {
    static const GLenum value = GL_UNSIGNED_SHORT;
};

// ScalarType stays the same — already C++11 compatible

template <typename T>
struct GLTypeForT {
    static const GLenum value = GLTypeOf<typename ScalarType<T>::type>::value;
};

// glFormatFor: constexpr with single return for C++11
constexpr GLenum glFormatFor(size_t channels) { return (channels == 1) ? GL_RED : (channels == 2) ? GL_RG : (channels == 3) ? GL_RGB : GL_RGBA; }

}  // namespace detail

class FBO;
typedef std::shared_ptr<FBO> FBOPtr;
typedef std::weak_ptr<FBO> FBOWeak;

class FBO {
private:
    FBOWeak m_This;
    GLuint m_FBOId = 0U;
    GLsizei m_SizeX = 0;
    GLsizei m_SizeY = 0;
    GLuint m_CountBuffers = 0U;
    bool m_UseMipMapping = false;
    std::vector<TexturePtr> m_Textures;
    GLenum* m_ColorDrawBuffers = nullptr;

public:
    static FBOPtr create(const GLsizei& vSX, const GLsizei& vSY, const GLuint vCountBuffers, const bool vUseMipMapping) {
        auto res = std::make_shared<FBO>();
        res->m_This = res;
        if (!res->init(vSX, vSY, vCountBuffers, vUseMipMapping)) {
            res.reset();
        }
        return res;
    }

public:
    FBO() = default;
    virtual ~FBO() { unit(); }

    bool init(const GLsizei& vSX, const GLsizei& vSY, const GLuint vCountBuffers, const bool vUseMipMapping) {
        bool res = false;
        m_SizeX = vSX;
        m_SizeY = vSY;
        m_CountBuffers = vCountBuffers;
        m_UseMipMapping = vUseMipMapping;
        if (m_CountBuffers > 0U) {
            glGenFramebuffers(1, &m_FBOId);
            CheckGLErrors;
            glBindFramebuffer(GL_FRAMEBUFFER, m_FBOId);
            CheckGLErrors;
            m_Textures.resize(m_CountBuffers);
            m_ColorDrawBuffers = new GLenum[m_CountBuffers];
            for (GLuint idx = 0U; idx < vCountBuffers; ++idx) {
                m_Textures[idx] = Texture::createEmpty(vSX, vSY, "clamp", "nearest", vUseMipMapping);
                if (m_Textures[idx] != nullptr) {
                    m_ColorDrawBuffers[idx] = GL_COLOR_ATTACHMENT0 + (GLenum)idx;
                    glFramebufferTexture2D(GL_FRAMEBUFFER, m_ColorDrawBuffers[idx], GL_TEXTURE_2D, m_Textures[idx]->getTexId(), 0);
                    CheckGLErrors;
                }
            }
            glFinish();
            res = check();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            CheckGLErrors;
        }
        return res;
    }

    bool bind() {
#ifdef PROFILER_SCOPED
        PROFILER_SCOPED("FBO", "bind");
#endif
        if (m_FBOId > 0) {
            glBindFramebuffer(GL_FRAMEBUFFER, m_FBOId);
            CheckGLErrors;
            return true;
        }
        return false;
    }

    void clearBuffer(const std::array<float, 4U>& vColor) {
#ifdef PROFILER_SCOPED
        PROFILER_SCOPED("FBO", "clearBuffer");
#endif
        if (bind()) {
            {
#ifdef PROFILER_SCOPED
                PROFILER_SCOPED("FBO", "clearColor");
#endif
                glClearColor(vColor[0], vColor[1], vColor[2], vColor[3]);
                glClear(GL_COLOR_BUFFER_BIT);
            }
            unbind();
        }
    }

    void updateMipMaping() {
        if (m_UseMipMapping) {
#ifdef PROFILER_SCOPED
            PROFILER_SCOPED("FBO", "updateMipMaping %u", m_FBOId);
#endif
            for (auto& tex_ptr : m_Textures) {
                if (tex_ptr != nullptr) {
                    tex_ptr->updateMipMaping();
                }
            }
        }
    }
    template <typename T = uint8_t, size_t Channels = 4>
    std::vector<T> getPixels(const GLsizei& vPX, const GLsizei& vPY, const GLsizei& vSX, const GLsizei& vSY) {
        ASSERT_THROW((vPX >= 0) && (vPY >= 0) && (vSX > 0) && (vSY > 0), "");
        ASSERT_THROW((m_SizeX >= vPX + vSX) && (m_SizeY >= vPY + vSY), "");
        const GLenum glType = detail::GLTypeForT<T>::value;
        const GLenum glFormat = detail::glFormatFor(sizeof(T) / sizeof(typename detail::ScalarType<T>::type) * Channels);
        const size_t pixelCount = static_cast<size_t>(vSX) * static_cast<size_t>(vSY);
        std::vector<T> pixels(pixelCount * Channels);
        if (bind()) {
            glPixelStorei(GL_PACK_ALIGNMENT, 1);
            glReadPixels(vPX, vPY, vSX, vSY, glFormat, glType, pixels.data());
            CheckGLErrors;
            unbind();
        }
        return pixels;
    }

    void selectBuffers() {
#ifdef PROFILER_SCOPED
        PROFILER_SCOPED("FBO", "glDrawBuffers");
#endif
        glDrawBuffers(m_CountBuffers, m_ColorDrawBuffers);
        CheckGLErrors;
    }

    void unbind() {
#ifdef PROFILER_SCOPED
        PROFILER_SCOPED("FBO", "unbind");
#endif
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        CheckGLErrors;
        glBindTexture(GL_TEXTURE_2D, 0);
        CheckGLErrors;
    }

    GLuint getBuffersCount() const { return m_CountBuffers; }

    GLuint getTextureId(const size_t& vBufferIdx = 0U) const {
        if (m_Textures.size() > vBufferIdx) {
            return m_Textures[vBufferIdx]->getTexId();
        }
        return 0U;
    }

    bool resize(const GLsizei& vNewSx, const GLsizei& vNewSy) {
        bool res = false;
        if (m_FBOId > 0) {
            m_SizeX = vNewSx;
            m_SizeY = vNewSy;
            glBindFramebuffer(GL_FRAMEBUFFER, m_FBOId);
            CheckGLErrors;
            for (GLuint idx = 0U; idx < m_CountBuffers; ++idx) {
                if (m_Textures[idx] != nullptr) {
                    // m_Textures[idx] = Texture::createEmpty(vNewSx, vNewSy, m_UseMipMapping);
                    if (m_Textures[idx]->resize(vNewSx, vNewSy)) {
                        if (m_Textures[idx] != nullptr) {
                            m_ColorDrawBuffers[idx] = GL_COLOR_ATTACHMENT0 + (GLenum)idx;
                            glFramebufferTexture2D(GL_FRAMEBUFFER, m_ColorDrawBuffers[idx], GL_TEXTURE_2D, m_Textures[idx]->getTexId(), 0);
                            CheckGLErrors;
                        }
                    }
                }
            }
            glFinish();
            res = check();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            CheckGLErrors;
        }
        return res;
    }

    bool check() {
        if (GL_TRUE == glIsFramebuffer(m_FBOId)) {
            CheckGLErrors;
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
                CheckGLErrors;
                return true;
            }
        }
        return false;
    }

    void unit() {
        glDeleteFramebuffers(1, &m_FBOId);
        CheckGLErrors;
    }

    void blitOnScreen(const GLint vX, const GLint vY, const GLint vW, const GLint vH, const GLint vAttachementID, GLbitfield vMask, GLenum vFilter) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        CheckGLErrors;

        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBOId);
        CheckGLErrors;

        glReadBuffer(GL_COLOR_ATTACHMENT0 + vAttachementID);
        CheckGLErrors;

        glDrawBuffer(GL_BACK);
        CheckGLErrors;

        glBlitFramebuffer(0, 0, m_SizeX, m_SizeY, vX, vY, vX + vW, vY + vH, vMask, vFilter);
        CheckGLErrors;
    }
};

/*
 * add front and back FBO
 * and switch between front and back after rendering
 */

class FBOPipeLine;
typedef std::shared_ptr<FBOPipeLine> FBOPipeLinePtr;
typedef std::weak_ptr<FBOPipeLine> FBOPipeLineWeak;
class FBOPipeLine {
private:
    FBOPipeLineWeak m_This;
    FBOPtr m_FrontFBOPtr = nullptr;
    FBOPtr m_BackFBOPtr = nullptr;
    bool m_MultiPass = false;

public:
    static FBOPipeLinePtr create(const GLsizei& vSX, const GLsizei& vSY, const GLuint vCountBuffers, const bool vUseMipMapping, const bool vMultiPass) {
        auto res = std::make_shared<FBOPipeLine>();
        res->m_This = res;
        if (!res->init(vSX, vSY, vCountBuffers, vUseMipMapping, vMultiPass)) {
            res.reset();
        }
        return res;
    }

public:
    FBOPipeLine() = default;
    virtual ~FBOPipeLine() { unit(); }
    bool init(const GLsizei& vSX, const GLsizei& vSY, const GLuint vCountBuffers, const bool vUseMipMapping, const bool vMultiPass) {
        bool res = true;
        m_MultiPass = vMultiPass;
        m_FrontFBOPtr = FBO::create(vSX, vSY, vCountBuffers, vUseMipMapping);
        if (m_FrontFBOPtr != nullptr) {
            if (m_MultiPass) {
                m_BackFBOPtr = FBO::create(vSX, vSY, vCountBuffers, vUseMipMapping);
                if (m_BackFBOPtr != nullptr) {
                    res = true;
                }
            } else {
                res = true;
            }
        }
        return res;
    }
    bool resize(const GLsizei& vNewSx, const GLsizei& vNewSy) {
        bool res = false;
        ASSERT_THROW(m_FrontFBOPtr != nullptr, "");
        res = m_FrontFBOPtr->resize(vNewSx, vNewSy);
        if (m_MultiPass) {
            ASSERT_THROW(m_BackFBOPtr != nullptr, "");
            res &= m_BackFBOPtr->resize(vNewSx, vNewSy);
        }
        return res;
    }
    void unit() {
        m_FrontFBOPtr.reset();
        m_BackFBOPtr.reset();
    }
    bool bind() {
        ASSERT_THROW(m_FrontFBOPtr != nullptr, "");
        return m_FrontFBOPtr->bind();
    }
    void clearBuffer(const std::array<float, 4U>& vColor) {
        ASSERT_THROW(m_FrontFBOPtr != nullptr, "");
        m_FrontFBOPtr->clearBuffer(vColor);
        if (m_MultiPass) {
            ASSERT_THROW(m_BackFBOPtr != nullptr, "");
            m_BackFBOPtr->clearBuffer(vColor);
        }
    }
    void updateMipMaping() {
        ASSERT_THROW(m_FrontFBOPtr != nullptr, "");
        m_FrontFBOPtr->updateMipMaping();
    }
    void selectBuffers() {
        ASSERT_THROW(m_FrontFBOPtr != nullptr, "");
        m_FrontFBOPtr->selectBuffers();
    }
    void unbind() {
        ASSERT_THROW(m_FrontFBOPtr != nullptr, "");
        m_FrontFBOPtr->unbind();
        if (m_MultiPass) {
            swapFBOs();
        }
    }
    GLuint getFrontTextureId(const size_t& vBufferIdx = 0U) const {
        ASSERT_THROW(m_FrontFBOPtr != nullptr, "");
        return m_FrontFBOPtr->getTextureId(vBufferIdx);
    }
    GLuint getBackTextureId(const size_t& vBufferIdx = 0U) const {
        ASSERT_THROW(m_MultiPass, "");
        ASSERT_THROW(m_BackFBOPtr != nullptr, "");
        return m_BackFBOPtr->getTextureId(vBufferIdx);
    }
    FBOWeak getFrontFBO() const { return m_FrontFBOPtr; }
    FBOWeak getBackFBO() const {
        ASSERT_THROW(m_MultiPass, "");
        return m_BackFBOPtr;
    }
    void swapFBOs() {
        ASSERT_THROW(m_MultiPass, "");
        FBOPtr tmp = m_BackFBOPtr;
        m_BackFBOPtr = m_FrontFBOPtr;
        m_FrontFBOPtr = tmp;
    }
};

}  // namespace gl
}  // namespace ez
