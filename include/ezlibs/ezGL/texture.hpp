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

#ifdef STB_IMAGE_READER_INCLUDE
#include STB_IMAGE_READER_INCLUDE
#endif  // STB_IMAGE_READER_INCLUDE

#ifdef STB_IMAGE_WRITER_INCLUDE
#include STB_IMAGE_WRITER_INCLUDE
#endif  // STB_IMAGE_WRITER_INCLUDE

#include <algorithm>
#include <cassert>
#include <cstring>
#include <memory>
#include <string>
#include <array>

namespace ez {
namespace gl {

class Texture;
typedef std::shared_ptr<Texture> TexturePtr;
typedef std::weak_ptr<Texture> TextureWeak;

class Texture {
public:
    // wrap (repeat|mirror|clamp), filter (linear|nearest)
    static TexturePtr createEmpty(const GLsizei vSx, const GLsizei vSy, const std::string vWrap, const std::string vFilter, const bool vEnableMipMap) {
        auto res = std::make_shared<Texture>();
        res->m_This = res;
        if (!res->initEmpty(vSx, vSy, vWrap, vFilter, vEnableMipMap)) { res.reset(); }
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
        const bool vEnableMipMap,
        const std::array<GLenum, 4> vSwizzle = {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}) {
        auto res = std::make_shared<Texture>();
        res->m_This = res;
        if (!res->initFromBuffer(vBuffer, vSx, vSy, vInternalFormat, vFormat, vPixelFormat, vWrap, vFilter, vEnableMipMap, vSwizzle)) { res.reset(); }
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
        const bool vEnableMipMap,
        const std::array<GLenum, 4> vSwizzle = {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}) {
        auto res = std::make_shared<Texture>();
        res->m_This = res;
        if (!res->initFromBuffer(vBuffer, vSx, vSy, vChannelsCount, vPixelFormat, vWrap, vFilter, vEnableMipMap, vSwizzle)) { res.reset(); }
        return res;
    }
#ifdef STB_IMAGE_READER_INCLUDE
    // wrap (repeat|mirror|clamp), filter (linear|nearest)
    static TexturePtr createFromFile(const std::string& vFilePathName, bool vInvertY, std::string vWrap, std::string vFilter, bool vEnableMipMap) {
        auto res = std::make_shared<Texture>();
        res->m_This = res;
        if (!res->initFromFile(vFilePathName, vInvertY, vWrap, vFilter, vEnableMipMap)) { res.reset(); }
        return res;
    }
#endif  // STB_IMAGE_READER_INCLUDE

private:
    TextureWeak m_This;
    GLuint m_TexId = 0U;
    bool m_EnableMipMap = false;
    GLsizei m_Width = 0U;
    GLsizei m_Height = 0U;
    GLuint m_ChannelsCount = 0U;
    GLuint m_BytesPerPixel = 0U;
    GLenum m_Format = GL_RGBA;
    GLenum m_InternalFormat = GL_RGBA32F;
    GLenum m_PixelFormat = GL_FLOAT;
    GLenum m_WrapS = GL_REPEAT;
    GLenum m_WrapT = GL_REPEAT;
    GLenum m_MinFilter = GL_LINEAR_MIPMAP_LINEAR;
    GLenum m_MagFilter = GL_LINEAR;

public:
    Texture() = default;
    ~Texture() { unit(); }

    bool initEmpty(const GLsizei vSx, const GLsizei vSy, const std::string vWrap, const std::string vFilter, const bool vEnableMipMap) {
        ASSERT_THROW(vSx > 0, "");
        ASSERT_THROW(vSy > 0, "");
        m_Width = vSx;
        m_Height = vSy;
        m_EnableMipMap = vEnableMipMap;
        glGenTextures(1, &m_TexId);
        CheckGLErrors;
        glBindTexture(GL_TEXTURE_2D, m_TexId);
        CheckGLErrors;
        m_setFormat(GL_FLOAT, 4);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, vSx, vSy, 0, m_Format, m_PixelFormat, nullptr);
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
        const bool vEnableMipMap,
        const std::array<GLenum, 4> vSwizzle) {
        ASSERT_THROW(vBuffer != nullptr, "");
        ASSERT_THROW(vSx > 0, "");
        ASSERT_THROW(vSy > 0, "");
        m_Width = vSx;
        m_Height = vSy;
        glGenTextures(1, &m_TexId);
        CheckGLErrors;
        glBindTexture(GL_TEXTURE_2D, m_TexId);
        CheckGLErrors;
        m_setSwizzle(vSwizzle);
        m_InternalFormat = vInternalFormat;
        m_Format = vFormat;
        m_PixelFormat = vPixelFormat;
        m_ChannelsCount = m_channelsCountFromFormat(vFormat);
        m_BytesPerPixel = m_bytesPerChannelFromType(vPixelFormat) * m_ChannelsCount;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
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
        const bool vEnableMipMap,
        const std::array<GLenum, 4> vSwizzle) {
        ASSERT_THROW(vBuffer != nullptr, "");
        ASSERT_THROW(vSx > 0, "");
        ASSERT_THROW(vSy > 0, "");
        ASSERT_THROW(vChannelsCount > 0, "");
        m_Width = vSx;
        m_Height = vSy;
        glGenTextures(1, &m_TexId);
        CheckGLErrors;
        glBindTexture(GL_TEXTURE_2D, m_TexId);
        CheckGLErrors;
        m_setSwizzle(vSwizzle);
        m_setFormat(vPixelFormat, vChannelsCount);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
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

#ifdef STB_IMAGE_READER_INCLUDE
    // wrap (repeat|mirror|clamp), filter (linear|nearest)
    bool initFromFile(
        const std::string& vFilePathName,
        const bool vInvertY,
        const std::string vWrap,
        const std::string vFilter,
        const bool vEnableMipMap,
        const std::array<GLenum, 4> vSwizzle = {GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA}) {
        ASSERT_THROW(!vFilePathName.empty(), "");
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
            m_setSwizzle(vSwizzle);
            m_setFormat(GL_UNSIGNED_BYTE, chans);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
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
#endif  // STB_IMAGE_READER_INCLUDE

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
            glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, vSx, vSy, 0, m_Format, m_PixelFormat, nullptr);
            CheckGLErrors;
            glFinish();
            m_Width = vSx;
            m_Height = vSy;
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
    bool isValid() const { return (glIsTexture(m_TexId) == GL_TRUE); }

    void bind() {
        glBindTexture(GL_TEXTURE_2D, m_TexId);
        CheckGLErrors;
    }

    void unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
        CheckGLErrors;
    }

    template <typename TTYPE>
    TTYPE getTexId() const {
        return static_cast<TTYPE>(m_TexId);
    }
    GLuint getTexId() const { return m_TexId; }
    GLuint getId() const { return m_TexId; }
    std::array<GLsizei, 2U> getSize() const { return {m_Width, m_Height}; }
    GLsizei getWidth() const { return m_Width; }
    GLsizei getHeight() const { return m_Height; }
    GLuint getChannelsCount() const { return m_ChannelsCount; }
    GLuint getBytesPerPixel() const { return m_BytesPerPixel; }
    GLenum getFormat() const { return m_Format; }
    GLenum getInternalFormat() const { return m_InternalFormat; }
    GLenum getPixelFormat() const { return m_PixelFormat; }

    template <typename T = uint8_t, size_t Channels = 4>
    bool setPixels(const GLsizei vX, const GLsizei vY, const GLsizei vWidth, const GLsizei vHeight, const T* vBuffer) {
        if (m_TexId == 0U || vBuffer == nullptr) { return false; }
        constexpr size_t bytesPerPixel = sizeof(T) * Channels;
        ASSERT_THROW(bytesPerPixel == m_BytesPerPixel, "Pixel stride mismatch with texture format");
        glBindTexture(GL_TEXTURE_2D, m_TexId);
        CheckGLErrors;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        CheckGLErrors;
        glTexSubImage2D(GL_TEXTURE_2D, 0, vX, vY, vWidth, vHeight, m_Format, m_PixelFormat, reinterpret_cast<const void*>(vBuffer));
        CheckGLErrors;
        glBindTexture(GL_TEXTURE_2D, 0);
        CheckGLErrors;
        return true;
    }

    template <typename T = uint8_t, size_t Channels = 4>
    std::vector<T> getPixels(const GLsizei& vSX, const GLsizei& vSY) {
        ASSERT_THROW((vSX > 0) && (vSY > 0), "");
        ASSERT_THROW((m_Width >= vSX) && (m_Height >= vSY), "");
        constexpr size_t bytesPerPixel = sizeof(T) * Channels;
        ASSERT_THROW(bytesPerPixel == m_BytesPerPixel, "Pixel stride mismatch with texture format");

        glBindTexture(GL_TEXTURE_2D, m_TexId);
        GLint prevAlign = 0, prevRowLen = 0, prevSkipRows = 0, prevSkipPix = 0;
        glGetIntegerv(GL_PACK_ALIGNMENT, &prevAlign);
        glGetIntegerv(GL_PACK_ROW_LENGTH, &prevRowLen);
        glGetIntegerv(GL_PACK_SKIP_ROWS, &prevSkipRows);
        glGetIntegerv(GL_PACK_SKIP_PIXELS, &prevSkipPix);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glPixelStorei(GL_PACK_ROW_LENGTH, 0);
        glPixelStorei(GL_PACK_SKIP_ROWS, 0);
        glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

        const size_t pixelCount = static_cast<size_t>(vSX) * static_cast<size_t>(vSY);
        std::vector<uint8_t> raw(pixelCount * m_BytesPerPixel);
        glGetTexImage(GL_TEXTURE_2D, 0, m_Format, m_PixelFormat, raw.data());
        CheckGLErrors;

        glPixelStorei(GL_PACK_ALIGNMENT, prevAlign);
        glPixelStorei(GL_PACK_ROW_LENGTH, prevRowLen);
        glPixelStorei(GL_PACK_SKIP_ROWS, prevSkipRows);
        glPixelStorei(GL_PACK_SKIP_PIXELS, prevSkipPix);
        glBindTexture(GL_TEXTURE_2D, 0);

        std::vector<T> pixels(pixelCount * Channels);
        std::memcpy(pixels.data(), raw.data(), raw.size());
        return pixels;
    }

    void generateMipmap() {
        if (m_TexId > 0U) {
            glBindTexture(GL_TEXTURE_2D, m_TexId);
            CheckGLErrors;
            glGenerateMipmap(GL_TEXTURE_2D);
            CheckGLErrors;
            glBindTexture(GL_TEXTURE_2D, 0);
            CheckGLErrors;
        }
    }

    void clear() {
        if (m_TexId > 0U) {
            const size_t totalBytes = static_cast<size_t>(m_Width) * static_cast<size_t>(m_Height) * m_BytesPerPixel;
            std::vector<uint8_t> buffer(totalBytes, 0);
            glBindTexture(GL_TEXTURE_2D, m_TexId);
            CheckGLErrors;
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            CheckGLErrors;
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, m_Format, m_PixelFormat, buffer.data());
            CheckGLErrors;
            glBindTexture(GL_TEXTURE_2D, 0);
            CheckGLErrors;
        }
    }

    void clear(const float vR, const float vG, const float vB, const float vA) {
        if (m_TexId == 0U) { return; }
        const size_t pixelCount = static_cast<size_t>(m_Width) * static_cast<size_t>(m_Height);
        if (m_PixelFormat == GL_FLOAT) {
            std::vector<float> buffer(pixelCount * m_ChannelsCount);
            for (size_t i = 0; i < buffer.size(); i += m_ChannelsCount) {
                if (m_ChannelsCount >= 1) buffer[i] = vR;
                if (m_ChannelsCount >= 2) buffer[i + 1] = vG;
                if (m_ChannelsCount >= 3) buffer[i + 2] = vB;
                if (m_ChannelsCount >= 4) buffer[i + 3] = vA;
            }
            setPixels<float>(0, 0, m_Width, m_Height, buffer.data());
        } else {
            const auto toU8 = [](float v) -> uint8_t { return static_cast<uint8_t>(ez::clamp(v, 0.0f, 1.0f) * 255.0f); };
            std::vector<uint8_t> buffer(pixelCount * m_ChannelsCount);
            for (size_t i = 0; i < buffer.size(); i += m_ChannelsCount) {
                if (m_ChannelsCount >= 1) buffer[i] = toU8(vR);
                if (m_ChannelsCount >= 2) buffer[i + 1] = toU8(vG);
                if (m_ChannelsCount >= 3) buffer[i + 2] = toU8(vB);
                if (m_ChannelsCount >= 4) buffer[i + 3] = toU8(vA);
            }
            setPixels(0, 0, m_Width, m_Height, buffer.data());
        }
    }

#ifdef STB_IMAGE_WRITER_INCLUDE
    bool saveToPng(const std::string& vFilePathName) const {
        if (vFilePathName.empty() || m_TexId == 0) { return false; }

        GLint w = 0, h = 0;
        glBindTexture(GL_TEXTURE_2D, m_TexId);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
        if (w <= 0 || h <= 0) { return false; }

        // Toujours lire en RGBA uint8 pour le PNG, avec conversion GPU
        const size_t pixelCount = static_cast<size_t>(w) * static_cast<size_t>(h);
        std::vector<uint8_t> pixels(pixelCount * 4);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        CheckGLErrors;
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_flip_vertically_on_write(1);
        const int ok = stbi_write_png(vFilePathName.c_str(), w, h, 4, pixels.data(), w * 4);
        return ok != 0;
    }
#endif  // STB_IMAGE_WRITER_INCLUDE

private:
    void m_setSwizzle(std::array<GLenum, 4> vSwizzle) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, vSwizzle[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, vSwizzle[1]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, vSwizzle[2]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, vSwizzle[3]);
    }

    // wrap (repeat|mirror|clamp), filter (linear|nearest)
    void m_setParameters(const std::string& vWrap, const std::string& vFilter, const bool vEnableMipMap) {
        if (vWrap == "repeat") {
            m_WrapS = GL_REPEAT;
            m_WrapT = GL_REPEAT;
        } else if (vWrap == "mirror") {
            m_WrapS = GL_MIRRORED_REPEAT;
            m_WrapT = GL_MIRRORED_REPEAT;
        } else {
            m_WrapS = GL_CLAMP_TO_EDGE;
            m_WrapT = GL_CLAMP_TO_EDGE;
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_WrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_WrapT);
        CheckGLErrors;

        if (vFilter == "linear") {
            if (vEnableMipMap) {
                m_MinFilter = GL_LINEAR_MIPMAP_LINEAR;
                m_MagFilter = GL_LINEAR;
            } else {
                m_MinFilter = GL_LINEAR;
                m_MagFilter = GL_LINEAR;
            }
        } else if (vFilter == "nearest") {
            if (vEnableMipMap) {
                m_MinFilter = GL_NEAREST_MIPMAP_NEAREST;
                m_MagFilter = GL_NEAREST;
            } else {
                m_MinFilter = GL_NEAREST;
                m_MagFilter = GL_NEAREST;
            }
        } else {
            m_MinFilter = GL_LINEAR;
            m_MagFilter = GL_LINEAR;
        }
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_MinFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_MagFilter);
        CheckGLErrors;

        if (m_EnableMipMap) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
            CheckGLErrors;
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 8);
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
        ASSERT_THROW(
            vPixelFormat == GL_UNSIGNED_BYTE ||  // format BYTE
                vPixelFormat == GL_FLOAT,        // format FLOAT
            "");
        m_PixelFormat = vPixelFormat;
        m_ChannelsCount = vChannelsCount;
        const size_t bytesPerChannel = (vPixelFormat == GL_FLOAT) ? sizeof(float) : sizeof(uint8_t);
        m_BytesPerPixel = bytesPerChannel * static_cast<size_t>(vChannelsCount);
        switch (vChannelsCount) {
            case 1: {
                m_Format = GL_RED;
                if (vPixelFormat == GL_UNSIGNED_BYTE) {
                    m_InternalFormat = GL_R8;
                } else if (vPixelFormat == GL_FLOAT) {
                    m_InternalFormat = GL_R32F;
                }
            } break;
            case 2: {
                m_Format = GL_RG;
                if (vPixelFormat == GL_UNSIGNED_BYTE) {
                    m_InternalFormat = GL_RG8;
                } else if (vPixelFormat == GL_FLOAT) {
                    m_InternalFormat = GL_RG32F;
                }
            } break;
            case 3: {
                m_Format = GL_RGB;
                if (vPixelFormat == GL_UNSIGNED_BYTE) {
                    m_InternalFormat = GL_RGB8;
                } else if (vPixelFormat == GL_FLOAT) {
                    m_InternalFormat = GL_RGB32F;
                }
            } break;
            case 4: {
                m_Format = GL_RGBA;
                if (vPixelFormat == GL_UNSIGNED_BYTE) {
                    m_InternalFormat = GL_RGBA8;
                } else if (vPixelFormat == GL_FLOAT) {
                    m_InternalFormat = GL_RGBA32F;
                }
            } break;
        }
    }

    static GLuint m_channelsCountFromFormat(const GLenum vFormat) {
        switch (vFormat) {
            case GL_RED: return 1;
            case GL_RG: return 2;
            case GL_RGB: return 3;
            case GL_RGBA: return 4;
            default: return 4;
        }
    }

    static size_t m_bytesPerChannelFromType(const GLenum vPixelFormat) {
        switch (vPixelFormat) {
            case GL_UNSIGNED_BYTE: return sizeof(uint8_t);
            case GL_FLOAT: return sizeof(float);
            case GL_INT: return sizeof(int32_t);
            case GL_UNSIGNED_INT: return sizeof(uint32_t);
            case GL_SHORT: return sizeof(int16_t);
            case GL_UNSIGNED_SHORT: return sizeof(uint16_t);
            default: return sizeof(uint8_t);
        }
    }
};

class Texture2DArray;
typedef std::shared_ptr<Texture2DArray> Texture2DArrayPtr;
typedef std::weak_ptr<Texture2DArray> Texture2DArrayWeak;

class Texture2DArray {
public:
    static Texture2DArrayPtr create(
        const GLsizei vSx,
        const GLsizei vSy,
        const GLsizei vLayers,
        const GLenum vInternalFormat,
        const GLenum vAllocFormat,
        const GLenum vPixelFormat,
        const std::string& vWrap,
        const std::string& vFilter,
        const bool vUseMipMapping) {
        auto res = std::make_shared<Texture2DArray>();
        res->m_This = res;
        if (!res->init(vSx, vSy, vLayers, vInternalFormat, vAllocFormat, vPixelFormat, vWrap, vFilter, vUseMipMapping)) { res.reset(); }
        return res;
    }

private:
    GLuint m_TexId = 0U;

    GLsizei m_Width = 0;
    GLsizei m_Height = 0;
    GLsizei m_Layers = 0;

    GLenum m_InternalFormat = GL_RGBA8;
    GLenum m_AllocFormat = GL_RGBA;
    GLenum m_PixelFormat = GL_UNSIGNED_BYTE;

    std::string m_wrap;
    std::string m_filter;

    std::vector<int> m_FreeList;

    bool m_useMipMapping = false;
    GLsizei m_MipCount = 0;
    bool m_useImmutable = false;
    std::vector<bool> m_LevelDeclared;
    GLint m_MinDeclaredLevel = -1;
    GLint m_MaxDeclaredLevel = -1;

    Texture2DArrayWeak m_This;

public:
    Texture2DArray() = default;
    ~Texture2DArray() { unit(); }

    bool init(
        const GLsizei vSx,
        const GLsizei vSy,
        const GLsizei vLayers,
        const GLenum vInternalFormat,
        const GLenum vAllocFormat,
        const GLenum vPixelFormat,
        const std::string& vWrap,
        const std::string& vFilter,
        const bool vUseMipMapping) {
        unit();
        ASSERT_THROW(vSx > 0 && vSy > 0 && vLayers > 0, "");
        m_Width = vSx;
        m_Height = vSy;
        m_Layers = vLayers;
        m_InternalFormat = vInternalFormat;
        m_AllocFormat = vAllocFormat;
        m_PixelFormat = vPixelFormat;
        m_wrap = vWrap;
        m_filter = vFilter;
        m_useMipMapping = vUseMipMapping;

        GLint maxLayers = 0;
        glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxLayers);
        if (vLayers > maxLayers) {
#ifdef EZ_TOOLS_LOG
            LogVarError("The layers count is superior to the max admissible of %i for your GPU", maxLayers);
#endif
            return false;
        }

        glGenTextures(1, &m_TexId);
        CheckGLErrors;
        if (m_TexId == 0U) { return false; }

        m_MipCount = m_useMipMapping ? m_computeMipCount(m_Width, m_Height) : 1;
        m_LevelDeclared.assign((size_t)m_MipCount, false);
        m_MinDeclaredLevel = -1;
        m_MaxDeclaredLevel = -1;

        m_setParameters(m_wrap, m_filter);

        m_useImmutable = (reinterpret_cast<void*>(glTexStorage3D) != nullptr);

        glBindTexture(GL_TEXTURE_2D_ARRAY, m_TexId);
        CheckGLErrors;

        if (m_useImmutable) {
            glTexStorage3D(GL_TEXTURE_2D_ARRAY, m_MipCount, m_InternalFormat, m_Width, m_Height, m_Layers);
            CheckGLErrors;
        }

        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        CheckGLErrors;

        m_FreeList.clear();
        m_FreeList.reserve((size_t)vLayers);
        for (int i = vLayers - 1; i >= 0; --i) { m_FreeList.push_back(i); }

        return check();
    }

    void unit() {
        if (m_TexId != 0U) {
            glDeleteTextures(1, &m_TexId);
            CheckGLErrors;
            m_TexId = 0U;
        }
        m_Width = m_Height = m_Layers = 0;
        m_InternalFormat = GL_RGBA8;
        m_AllocFormat = GL_RGBA;
        m_PixelFormat = GL_UNSIGNED_BYTE;
        m_MipCount = 0;
        m_useImmutable = false;
        m_FreeList.clear();
        m_LevelDeclared.clear();
        m_MinDeclaredLevel = m_MaxDeclaredLevel = -1;
    }

    int addLayer(const GLsizei vLevel, const void* vpPixels) {
        if (m_TexId == 0U || vpPixels == nullptr) { return -1; }
        if (vLevel < 0 || vLevel >= m_MipCount) { return -1; }
        if (m_FreeList.empty()) { return -2; }

        const int layer = m_FreeList.back();
        m_FreeList.pop_back();

        if (!m_ensureLevelDeclared(vLevel)) {
            m_FreeList.push_back(layer);
            return -1;
        }

        const GLsizei w = std::max<GLsizei>(1, m_Width >> vLevel);
        const GLsizei h = std::max<GLsizei>(1, m_Height >> vLevel);

        glBindTexture(GL_TEXTURE_2D_ARRAY, m_TexId);
        CheckGLErrors;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        CheckGLErrors;
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, vLevel, 0, 0, layer, w, h, 1, m_AllocFormat, m_PixelFormat, vpPixels);
        CheckGLErrors;
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        CheckGLErrors;

        m_enableMipTrilinearIfPossible(m_filter);

        return layer;
    }

    bool uploadLayer(const GLint vLayer, const GLsizei vLevel, const void* vpPixels) {
        if (m_TexId == 0U || vpPixels == nullptr) { return false; }
        if (vLayer < 0 || vLayer >= m_Layers) { return false; }
        if (vLevel < 0 || vLevel >= m_MipCount) { return false; }
        if (!m_ensureLevelDeclared(vLevel)) { return false; }

        const GLsizei w = std::max<GLsizei>(1, m_Width >> vLevel);
        const GLsizei h = std::max<GLsizei>(1, m_Height >> vLevel);

        glBindTexture(GL_TEXTURE_2D_ARRAY, m_TexId);
        CheckGLErrors;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        CheckGLErrors;
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, vLevel, 0, 0, vLayer, w, h, 1, m_AllocFormat, m_PixelFormat, vpPixels);
        CheckGLErrors;
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        CheckGLErrors;

        m_enableMipTrilinearIfPossible(m_filter);
        return true;
    }

    void removeLayer(const GLint vLayer) {
        if (m_TexId == 0U) { return; }
        if (vLayer < 0 || vLayer >= m_Layers) { return; }
        const bool found = (std::find(m_FreeList.begin(), m_FreeList.end(), vLayer) != m_FreeList.end());
        if (!found) { m_FreeList.push_back(vLayer); }
    }

    void setParameters(const std::string& vWrap, const std::string& vFilter) {
        if (m_TexId == 0U) { return; }
        m_setParameters(vWrap, vFilter);
        m_enableMipTrilinearIfPossible(vFilter);
    }

    bool check() const { return (glIsTexture(m_TexId) == GL_TRUE); }
    GLuint getTexId() const { return m_TexId; }
    GLsizei getWidth() const { return m_Width; }
    GLsizei getHeight() const { return m_Height; }
    GLsizei getLayers() const { return m_Layers; }
    GLsizei getMipCount() const { return m_MipCount; }

private:
    static GLsizei m_computeMipCount(const GLsizei vSx, const GLsizei vSy) {
        auto s = (vSx > vSy ? vSx : vSy);
        GLsizei m = 1;
        while (s > 1) {
            s >>= 1;
            ++m;
        }
        return m;
    }

    void m_updateLodClamp() {
        if (m_TexId == 0U) { return; }
        if (m_MinDeclaredLevel < 0 || m_MaxDeclaredLevel < 0) { return; }
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_TexId);
        CheckGLErrors;
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, m_MinDeclaredLevel);
        CheckGLErrors;
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, m_MaxDeclaredLevel);
        CheckGLErrors;
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        CheckGLErrors;
    }

    bool m_ensureLevelDeclared(const GLsizei vLevel) {
        if (vLevel < 0 || vLevel >= m_MipCount) { return false; }
        if (m_LevelDeclared.empty()) { return false; }
        if (m_LevelDeclared[(size_t)vLevel]) {
            m_updateLodClamp();
            return true;
        }

        if (!m_useImmutable) {
            const GLsizei w = std::max<GLsizei>(1, m_Width >> vLevel);
            const GLsizei h = std::max<GLsizei>(1, m_Height >> vLevel);

            glBindTexture(GL_TEXTURE_2D_ARRAY, m_TexId);
            CheckGLErrors;
            glTexImage3D(GL_TEXTURE_2D_ARRAY, vLevel, m_InternalFormat, w, h, m_Layers, 0, m_AllocFormat, m_PixelFormat, nullptr);
            CheckGLErrors;
            glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
            CheckGLErrors;
        }

        m_LevelDeclared[(size_t)vLevel] = true;

        if (m_MinDeclaredLevel < 0 || vLevel < m_MinDeclaredLevel) { m_MinDeclaredLevel = vLevel; }
        if (m_MaxDeclaredLevel < 0 || vLevel > m_MaxDeclaredLevel) { m_MaxDeclaredLevel = vLevel; }

        m_updateLodClamp();
        return true;
    }

    void m_setParameters(const std::string& vWrap, const std::string& vFilter) {
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_TexId);
        CheckGLErrors;

        GLenum wrap = GL_CLAMP_TO_EDGE;
        if (vWrap == "repeat") {
            wrap = GL_REPEAT;
        } else if (vWrap == "mirror") {
            wrap = GL_MIRRORED_REPEAT;
        }
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, wrap);
        CheckGLErrors;
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, wrap);
        CheckGLErrors;

        const bool linear = (vFilter != "nearest");
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, linear ? GL_LINEAR : GL_NEAREST);
        CheckGLErrors;
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);
        CheckGLErrors;

        const GLint initialBase = 0;
        const GLint initialMax = (m_useMipMapping && m_MipCount > 0) ? (m_MipCount - 1) : 0;
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, initialBase);
        CheckGLErrors;
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, initialMax);
        CheckGLErrors;

        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        CheckGLErrors;
    }

    void m_enableMipTrilinearIfPossible(const std::string& vFilter) {
        if (!m_useMipMapping) { return; }
        if (m_MinDeclaredLevel < 0 || m_MaxDeclaredLevel < 0) { return; }
        if (m_MaxDeclaredLevel <= m_MinDeclaredLevel) { return; }
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_TexId);
        CheckGLErrors;
        const bool linear = (vFilter != "nearest");
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);
        CheckGLErrors;
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        CheckGLErrors;
    }
};

}  // namespace gl
}  // namespace ez
