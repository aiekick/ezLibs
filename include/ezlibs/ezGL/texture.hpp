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
#endif // STB_IMAGE_WRITER_INCLUDE

#include <cassert>
#include <memory>
#include <string>

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
#ifdef STB_IMAGE_READER_INCLUDE
    // wrap (repeat|mirror|clamp), filter (linear|nearest)
    static TexturePtr createFromFile(const std::string& vFilePathName, bool vInvertY, std::string vWrap, std::string vFilter, bool vEnableMipMap) {
        auto res = std::make_shared<Texture>();
        res->m_This = res;
        if (!res->initFromFile(vFilePathName, vInvertY, vWrap, vFilter, vEnableMipMap)) {
            res.reset();
        }
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
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
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
#endif // STB_IMAGE_READER_INCLUDE
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

#ifdef STB_IMAGE_WRITER_INCLUDE
    bool saveToPng(const std::string& vFilePathName) const {
        if (vFilePathName.empty() || m_TexId == 0)
            return false;

        // 1) Dimensions
        glBindTexture(GL_TEXTURE_2D, m_TexId);
        GLint w = 0, h = 0;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
        if (w <= 0 || h <= 0)
            return false;

        // 2) Sauvegarde état pack + configuration propre
        GLint prevAlign = 0, prevRowLen = 0, prevSkipRows = 0, prevSkipPix = 0;
        glGetIntegerv(GL_PACK_ALIGNMENT, &prevAlign);
        glGetIntegerv(GL_PACK_ROW_LENGTH, &prevRowLen);
        glGetIntegerv(GL_PACK_SKIP_ROWS, &prevSkipRows);
        glGetIntegerv(GL_PACK_SKIP_PIXELS, &prevSkipPix);

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glPixelStorei(GL_PACK_ROW_LENGTH, 0);
        glPixelStorei(GL_PACK_SKIP_ROWS, 0);
        glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

        // 3) Lecture RGBA8
        std::vector<unsigned char> pixels(static_cast<size_t>(w) * static_cast<size_t>(h) * 4u);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

        // 4) Écriture PNG (top-left) : flip vertical pour correspondre aux viewers
        stbi_flip_vertically_on_write(1);
        const int ok = stbi_write_png(vFilePathName.c_str(), w, h, 4, pixels.data(), w * 4);

        // 5) Restauration état pack
        glPixelStorei(GL_PACK_ALIGNMENT, prevAlign);
        glPixelStorei(GL_PACK_ROW_LENGTH, prevRowLen);
        glPixelStorei(GL_PACK_SKIP_ROWS, prevSkipRows);
        glPixelStorei(GL_PACK_SKIP_PIXELS, prevSkipPix);

        return ok != 0;
    }
#endif//STB_IMAGE_WRITER_INCLUDE

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
        const GLenum vInternalFormat,  // ex: GL_RGBA8, GL_R16F, ...
        const GLenum vAllocFormat,     // ex: GL_RGBA, GL_RED, GL_RG
        const GLenum vPixelFormat,     // ex: GL_UNSIGNED_BYTE, GL_HALF_FLOAT, GL_FLOAT
        const std::string vWrap,
        const std::string vFilter) {
        auto res = std::make_shared<Texture2DArray>();
        res->m_This = res;
        if (!res->init(vSx, vSy, vLayers, vInternalFormat, vAllocFormat, vPixelFormat, vWrap, vFilter)) {
            res.reset();
        }
        return res;
    }

    // Factory "vide" avec defaults (RGBA8 / UBYTE)
    static Texture2DArrayPtr createEmpty(const GLsizei vSx, const GLsizei vSy, const GLsizei vLayers, const std::string vWrap, const std::string vFilter) {
        auto res = std::make_shared<Texture2DArray>();
        res->m_This = res;
        if (!res->init(vSx, vSy, vLayers, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, vWrap, vFilter)) {
            res.reset();
        }
        return res;
    }

private:
    GLuint m_TexId = 0U;

    GLsizei m_Width = 0;
    GLsizei m_Height = 0;
    GLsizei m_Layers = 0;

    GLenum m_InternalFormat = GL_RGBA8;       // format interne de stockage
    GLenum m_AllocFormat = GL_RGBA;           // format pour TexImage (déclaration)
    GLenum m_PixelFormat = GL_UNSIGNED_BYTE;  // type pour TexImage

    GLsizei m_MipCount = 0;

    std::vector<int> m_FreeList;        // pile de layers libres
    std::vector<bool> m_LevelDeclared;  // niveaux (L) déjà glTexImage3D
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
        const std::string vWrap,
        const std::string vFilter) {
        unit();
        assert(vSx > 0 && vSy > 0 && vLayers > 0);
        m_Width = vSx;
        m_Height = vSy;
        m_Layers = vLayers;
        m_InternalFormat = vInternalFormat;
        m_AllocFormat = vAllocFormat;
        m_PixelFormat = vPixelFormat;

        glGenTextures(1, &m_TexId);
        CheckGLErrors;
        if (m_TexId == 0U)
            return false;

        m_setParameters(vWrap, vFilter);

        // Prépare free-list
        m_FreeList.clear();
        m_FreeList.reserve((size_t)vLayers);
        for (int i = vLayers - 1; i >= 0; --i)
            m_FreeList.push_back(i);

        // Mips théoriques, mais non alloués au départ
        m_MipCount = m_computeMipCount(m_Width, m_Height);
        m_LevelDeclared.assign((size_t)m_MipCount, false);
        m_MinDeclaredLevel = -1;
        m_MaxDeclaredLevel = -1;

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
        m_FreeList.clear();
        m_LevelDeclared.clear();
        m_MinDeclaredLevel = m_MaxDeclaredLevel = -1;
    }

    // Alloue une layer libre et uploade le mip 'vLevel' avec 'vpPixels'
    // Retourne l'index de layer, ou -1 si saturation/erreur.
    int addTile(const GLsizei vLevel, const void* vpPixels, const GLenum vFormat, const GLenum vType, const std::string vFilter = "linear") {
        if (m_TexId == 0U || vpPixels == nullptr)
            return -1;
        if (vLevel < 0 || vLevel >= m_MipCount)
            return -1;
        if (m_FreeList.empty())
            return -1;

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
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, vLevel, 0, 0, layer, w, h, 1, vFormat, vType, vpPixels);
        CheckGLErrors;
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        CheckGLErrors;

        // Si on commence à avoir >=2 niveaux déclarés, active le tri-linéaire selon vFilter
        m_enableMipTrilinearIfPossible(vFilter);

        return layer;
    }

    // Met à jour un mip sur une layer existante
    bool uploadTile(const GLint vLayer, const GLsizei vLevel, const void* vpPixels, const GLenum vFormat, const GLenum vType, const std::string vFilter = "linear") {
        if (m_TexId == 0U || vpPixels == nullptr)
            return false;
        if (vLayer < 0 || vLayer >= m_Layers)
            return false;
        if (vLevel < 0 || vLevel >= m_MipCount)
            return false;

        if (!m_ensureLevelDeclared(vLevel))
            return false;

        const GLsizei w = std::max<GLsizei>(1, m_Width >> vLevel);
        const GLsizei h = std::max<GLsizei>(1, m_Height >> vLevel);

        glBindTexture(GL_TEXTURE_2D_ARRAY, m_TexId);
        CheckGLErrors;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        CheckGLErrors;
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, vLevel, 0, 0, vLayer, w, h, 1, vFormat, vType, vpPixels);
        CheckGLErrors;
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        CheckGLErrors;

        m_enableMipTrilinearIfPossible(vFilter);
        return true;
    }

    // Libère la layer (réutilisable par addTile)
    void removeTile(const GLint vLayer) {
        if (m_TexId == 0U)
            return;
        if (vLayer < 0 || vLayer >= m_Layers)
            return;
        // éviter doublons
        const bool found = (std::find(m_FreeList.begin(), m_FreeList.end(), vLayer) != m_FreeList.end());
        if (!found)
            m_FreeList.push_back(vLayer);
    }

    // Paramètres (wrap/filter) — safe à appeler à tout moment
    void setParameters(const std::string vWrap, const std::string vFilter) {
        if (m_TexId == 0U)
            return;
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
        if (m_TexId == 0U)
            return;
        if (m_MinDeclaredLevel < 0 || m_MaxDeclaredLevel < 0)
            return;
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_TexId);
        CheckGLErrors;
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, m_MinDeclaredLevel);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, m_MaxDeclaredLevel);
        CheckGLErrors;
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        CheckGLErrors;
    }
    bool m_ensureLevelDeclared(const GLsizei vLevel) {
        if (vLevel < 0 || vLevel >= m_MipCount)
            return false;
        if (m_LevelDeclared.empty())
            return false;
        if (m_LevelDeclared[(size_t)vLevel]) {
            m_updateLodClamp();
            return true;
        }
        const GLsizei w = std::max<GLsizei>(1, m_Width >> vLevel);
        const GLsizei h = std::max<GLsizei>(1, m_Height >> vLevel);

        glBindTexture(GL_TEXTURE_2D_ARRAY, m_TexId);
        CheckGLErrors;
        glTexImage3D(GL_TEXTURE_2D_ARRAY, vLevel, m_InternalFormat, w, h, m_Layers, 0, m_AllocFormat, m_PixelFormat, nullptr);
        CheckGLErrors;
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        CheckGLErrors;

        m_LevelDeclared[(size_t)vLevel] = true;
        if (m_MinDeclaredLevel < 0 || vLevel < m_MinDeclaredLevel)
            m_MinDeclaredLevel = vLevel;
        if (m_MaxDeclaredLevel < 0 || vLevel > m_MaxDeclaredLevel)
            m_MaxDeclaredLevel = vLevel;
        m_updateLodClamp();
        return true;
    }
    void m_setParameters(const std::string vWrap, const std::string vFilter) {
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_TexId);
        CheckGLErrors;

        // Wrap
        GLenum wrap = GL_CLAMP_TO_EDGE;
        if (vWrap == "repeat")
            wrap = GL_REPEAT;
        else if (vWrap == "mirror")
            wrap = GL_MIRRORED_REPEAT;
        // "clamp" par défaut
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, wrap);
        CheckGLErrors;

        // Filter
        if (vFilter == "nearest") {
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // on remettra MIPMAP si plusieurs niveaux
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        } else {
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        CheckGLErrors;

        // Par défaut, on désactive le sampling de niveaux non déclarés
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BASE_LEVEL, 1000);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, -1000);
        CheckGLErrors;

        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        CheckGLErrors;
    }
    void m_enableMipTrilinearIfPossible(const std::string vFilter) {
        // Si on a au moins deux niveaux contigus, autoriser le tri-linéaire si demandé (linear).
        if (m_MinDeclaredLevel < 0 || m_MaxDeclaredLevel < 0)
            return;
        if (m_MaxDeclaredLevel <= m_MinDeclaredLevel)
            return;  // un seul niveau
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_TexId);
        CheckGLErrors;
        const bool linear = (vFilter != "nearest");
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);
        // MAG reste GL_LINEAR ou GL_NEAREST selon vFilter (déjà posé)
        CheckGLErrors;
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        CheckGLErrors;
    }
};

}  // namespace gl
}  // namespace ez
