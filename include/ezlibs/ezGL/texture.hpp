/*
MIT License

Copyright (c) 2014-2023 Stephane Cuillerdier (aka aiekick)

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

#pragma once

#include "ezGL.hpp"

#ifdef STB_IMAGE_INCLUDE
#include STB_IMAGE_INCLUDE
#endif  // STB_IMAGE_INCLUDE

#include <cassert>
#include <memory>
#include <string>

namespace ez {
namespace gl {

class Texture;
typedef std::shared_ptr<Texture> TexturePtr;
typedef std::weak_ptr<Texture> TextureWeak;

class Texture {
private:
    TextureWeak m_This;
    GLuint m_TexId = 0U;
    bool m_EnableMipMap = false;
    GLsizei m_Width = 0U;
    GLsizei m_Height = 0U;
    GLuint m_ChannelsCount = 0U;
    GLenum m_Format = GL_RGBA;
    GLenum m_InternalFormat = GL_RGBA32F;
    GLenum m_PixelFormat = GL_FLOAT;
    GLenum m_WrapS = GL_REPEAT;
    GLenum m_WrapT = GL_REPEAT;
    GLenum m_MinFilter = GL_LINEAR_MIPMAP_LINEAR;
    GLenum m_MagFilter = GL_LINEAR;

public:
    // wrap (repeat|mirror|clamp), filter (linear|nearest)
    static TexturePtr createEmpty(const GLsizei vSx, const GLsizei vSy, const std::string vWrap, const std::string vFilter, const bool vEnableMipMap) {
        auto res = std::make_shared<Texture>();
        res->m_This = res;
        if (!res->initEmpty(vSx, vSy, vWrap, vFilter, vEnableMipMap)) {
            res.reset();
        }
        return res;
    }
    // wrap (repeat|mirror|clamp), filter (linear|nearest)
    static TexturePtr createFromBuffer(
        const uint8_t* vBuffer,
        const GLsizei vSx,
        const GLsizei vSy,
        const GLenum vInternalFormat,
        const GLenum vFormat,
        const GLenum vPixelFormat,
        const std::string vWrap,
        const std::string vFilter,
        const bool vEnableMipMap) {
        auto res = std::make_shared<Texture>();
        res->m_This = res;
        if (!res->initFromBuffer(vBuffer, vSx, vSy, vInternalFormat, vFormat, vPixelFormat, vWrap, vFilter, vEnableMipMap)) {
            res.reset();
        }
        return res;
    }
    // wrap (repeat|mirror|clamp), filter (linear|nearest)
    static TexturePtr createFromBuffer(
        const uint8_t* vBuffer,
        const GLsizei vSx,
        const GLsizei vSy,
        const GLint vChannelsCount,
        const GLenum vPixelFormat,
        const std::string vWrap,
        const std::string vFilter,
        const bool vEnableMipMap) {
        auto res = std::make_shared<Texture>();
        res->m_This = res;
        if (!res->initFromBuffer(vBuffer, vSx, vSy, vChannelsCount, vPixelFormat, vWrap, vFilter, vEnableMipMap)) {
            res.reset();
        }
        return res;
    }
#ifdef STB_IMAGE_INCLUDE
    // wrap (repeat|mirror|clamp), filter (linear|nearest)
    static TexturePtr createFromFile(const std::string& vFilePathName, bool vInvertY, std::string vWrap, std::string vFilter, bool vEnableMipMap) {
        auto res = std::make_shared<Texture>();
        res->m_This = res;
        if (!res->initFromFile(vFilePathName, vInvertY, vWrap, vFilter, vEnableMipMap)) {
            res.reset();
        }
        return res;
    }
#endif // STB_IMAGE_INCLUDE

public:
    Texture() = default;
    ~Texture() { unit(); }
    bool initEmpty(const GLsizei vSx, const GLsizei vSy, const std::string vWrap, const std::string vFilter, const bool vEnableMipMap) {
        assert(vSx > 0);
        assert(vSy > 0);
        m_Width = vSx;
        m_Height = vSy;
        m_EnableMipMap = vEnableMipMap;
        glGenTextures(1, &m_TexId);
        CheckGLErrors;
        glBindTexture(GL_TEXTURE_2D, m_TexId);
        CheckGLErrors;
        m_setFormat(GL_FLOAT, 4);
        glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, vSx, vSy, 0, m_Format, GL_FLOAT, nullptr);
        CheckGLErrors;
        glFinish();
        CheckGLErrors;
        m_setParameters(vWrap, vFilter, vEnableMipMap);
        glFinish();
        CheckGLErrors;
        glBindTexture(GL_TEXTURE_2D, 0);
        CheckGLErrors;
        return check();
    }
    // wrap (repeat|mirror|clamp), filter (linear|nearest)
    bool initFromBuffer(
        const uint8_t* vBuffer,
        const GLsizei vSx,
        const GLsizei vSy,
        const GLenum vInternalFormat,
        const GLenum vFormat,
        const GLenum vPixelFormat,
        const std::string vWrap,
        const std::string vFilter,
        const bool vEnableMipMap) {
        assert(vBuffer != nullptr);
        assert(vSx > 0);
        assert(vSy > 0);
        m_Width = vSx;
        m_Height = vSy;
        glGenTextures(1, &m_TexId);
        CheckGLErrors;
        glBindTexture(GL_TEXTURE_2D, m_TexId);
        CheckGLErrors;
        m_InternalFormat = vInternalFormat;
        m_Format = vFormat;
        m_PixelFormat = vPixelFormat;
        glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_Format, m_PixelFormat, vBuffer);
        CheckGLErrors;
        glFinish();
        CheckGLErrors;
        m_setParameters(vWrap, vFilter, vEnableMipMap);
        glFinish();
        CheckGLErrors;
        glBindTexture(GL_TEXTURE_2D, 0);
        CheckGLErrors;
        return check();
    }
    // wrap (repeat|mirror|clamp), filter (linear|nearest)
    bool initFromBuffer(
        const uint8_t* vBuffer,
        const GLsizei vSx,
        const GLsizei vSy,
        const GLint vChannelsCount,
        const GLenum vPixelFormat,
        const std::string vWrap,
        const std::string vFilter,
        const bool vEnableMipMap) {
        assert(vBuffer != nullptr);
        assert(vSx > 0);
        assert(vSy > 0);
        assert(vChannelsCount > 0);
        m_Width = vSx;
        m_Height = vSy;
        glGenTextures(1, &m_TexId);
        CheckGLErrors;
        glBindTexture(GL_TEXTURE_2D, m_TexId);
        CheckGLErrors;
        m_setFormat(vPixelFormat, vChannelsCount);
        glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_Format, m_PixelFormat, vBuffer);
        CheckGLErrors;
        glFinish();
        CheckGLErrors;
        m_setParameters(vWrap, vFilter, vEnableMipMap);
        glFinish();
        CheckGLErrors;
        glBindTexture(GL_TEXTURE_2D, 0);
        CheckGLErrors;
        return check();
    }
#ifdef STB_IMAGE_INCLUDE
    // wrap (repeat|mirror|clamp), filter (linear|nearest)
    bool initFromFile(
        const std::string& vFilePathName,
        const bool vInvertY,
        const std::string vWrap,
        const std::string vFilter,
        const bool vEnableMipMap) {
        assert(!vFilePathName.empty());
        stbi_set_flip_vertically_on_load(vInvertY);
        auto w = 0;
        auto h = 0;
        auto chans = 0;
        auto buffer = stbi_load(vFilePathName.c_str(), &w, &h, &chans, 0);
        if (buffer) {
            stbi_image_free(buffer);
            if (chans == 4) {
                buffer = stbi_load(vFilePathName.c_str(), &w, &h, &chans, STBI_rgb_alpha);
            } else if (chans == 3) {
                buffer = stbi_load(vFilePathName.c_str(), &w, &h, &chans, STBI_rgb);
            } else if (chans == 2) {
                buffer = stbi_load(vFilePathName.c_str(), &w, &h, &chans, STBI_grey_alpha);
            } else if (chans == 1) {
                buffer = stbi_load(vFilePathName.c_str(), &w, &h, &chans, STBI_grey);
            }
        }
        if (buffer) {
            m_Width = w;
            m_Height = h;
            glGenTextures(1, &m_TexId);
            CheckGLErrors;
            glBindTexture(GL_TEXTURE_2D, m_TexId);
            CheckGLErrors;
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            CheckGLErrors;
            m_setFormat(GL_UNSIGNED_BYTE, chans);
            glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_Format, m_PixelFormat, buffer);
            CheckGLErrors;
            glFinish();
            CheckGLErrors;
            m_setParameters(vWrap, vFilter, vEnableMipMap);
            glFinish();
            CheckGLErrors;
            glBindTexture(GL_TEXTURE_2D, 0);
            CheckGLErrors;
            stbi_image_free(buffer);
        } 
        return check();
    }
#endif // STB_IMAGE_INCLUDE
    void updateMipMaping() {
        if (m_EnableMipMap) {
#ifdef PROFILER_SCOPED
            PROFILER_SCOPED("Opengl", "glGenerateMipmap %u", m_TexId);
#endif
            glGenerateMipmap(GL_TEXTURE_2D);
            CheckGLErrors;
            glFinish();
            CheckGLErrors;
        }
    }
    bool resize(const GLsizei& vSx, const GLsizei& vSy) {
        if (m_TexId > 0U) {
            glBindTexture(GL_TEXTURE_2D, m_TexId);
            CheckGLErrors;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, vSx, vSy, 0, GL_RGBA, GL_FLOAT, nullptr);
            CheckGLErrors;
            glFinish();
            glBindTexture(GL_TEXTURE_2D, 0);
            CheckGLErrors;
            return true;
        }
        return false;
    }
    void unit() {
        glDeleteTextures(1, &m_TexId);
        CheckGLErrors;
    }
    bool check() { return (glIsTexture(m_TexId) == GL_TRUE); }
    GLuint getTexId() const { return m_TexId; }

private:
    // wrap (repeat|mirror|clamp), filter (linear|nearest)
    void m_setParameters(const std::string vWrap, const std::string vFilter, const bool vEnableMipMap) {
        if (vWrap == "repeat") {
            m_WrapS = GL_REPEAT;
            m_WrapT = GL_REPEAT;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_WrapS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_WrapT);
            CheckGLErrors;
        } else if (vWrap == "mirror") {
            m_WrapS = GL_MIRRORED_REPEAT;
            m_WrapT = GL_MIRRORED_REPEAT;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_WrapS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_WrapT);
            CheckGLErrors;
        } else if (vWrap == "clamp") {
            m_WrapS = GL_CLAMP_TO_EDGE;
            m_WrapT = GL_CLAMP_TO_EDGE;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_WrapS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_WrapT);
            CheckGLErrors;
        } else {
            m_WrapS = GL_CLAMP_TO_EDGE;
            m_WrapT = GL_CLAMP_TO_EDGE;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_WrapS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_WrapT);
            CheckGLErrors;
        }

        if (vFilter == "linear") {
            if (vEnableMipMap) {
                m_MinFilter = GL_LINEAR_MIPMAP_LINEAR;
                m_MagFilter = GL_LINEAR;
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_MinFilter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_MagFilter);
                CheckGLErrors;
            } else {
                m_MinFilter = GL_LINEAR;
                m_MagFilter = GL_LINEAR;
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_MinFilter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_MagFilter);
                CheckGLErrors;
            }
        } else if (vFilter == "nearest") {
            if (vEnableMipMap) {
                m_MinFilter = GL_NEAREST_MIPMAP_NEAREST;
                m_MagFilter = GL_NEAREST;
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_MinFilter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_MagFilter);
                CheckGLErrors;
            } else {
                m_MinFilter = GL_NEAREST;
                m_MagFilter = GL_NEAREST;
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_MinFilter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_MagFilter);
                CheckGLErrors;
            }
        } else {
            m_MinFilter = GL_LINEAR;
            m_MagFilter = GL_LINEAR;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_MinFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_MagFilter);
            CheckGLErrors;
        }
        if (m_EnableMipMap) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
            CheckGLErrors;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 8U);
            CheckGLErrors;
            updateMipMaping();
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
            CheckGLErrors;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
            CheckGLErrors;
        }
    }
    void m_setFormat(const GLenum vPixelFormat, const GLint& vChannelsCount) {
        assert(
            vPixelFormat == GL_UNSIGNED_BYTE ||  // format BYTE
            vPixelFormat == GL_FLOAT);  // format FLOAT
        m_PixelFormat = vPixelFormat;
        m_ChannelsCount = vChannelsCount;
        // 1:r, 2:rg, 3:rgb, 4:rgba
        switch (vChannelsCount) {
            case 1: {
                m_Format = GL_RED;
                if (vPixelFormat == GL_UNSIGNED_BYTE) {
                    m_InternalFormat = GL_RED;
                } else if (vPixelFormat == GL_FLOAT) {
                    m_InternalFormat = GL_R32F;
                }
            } break;
            case 2: {
                m_Format = GL_RG;
                if (vPixelFormat == GL_UNSIGNED_BYTE) {
                    m_InternalFormat = GL_RG;
                } else if (vPixelFormat == GL_FLOAT) {
                    m_InternalFormat = GL_RG32F;
                }
            } break;
            case 3: {
                m_Format = GL_RGB;
                if (vPixelFormat == GL_UNSIGNED_BYTE) {
                    m_InternalFormat = GL_RGB;
                } else if (vPixelFormat == GL_FLOAT) {
                    m_InternalFormat = GL_RGB32F;
                }
            } break;
            case 4: {
                m_Format = GL_RGBA;
                if (vPixelFormat == GL_UNSIGNED_BYTE) {
                    m_InternalFormat = GL_RGBA;
                } else if (vPixelFormat == GL_FLOAT) {
                    m_InternalFormat = GL_RGBA32F;
                }
            } break;
        }
    }
};

}  // namespace gl
}  // namespace ez
