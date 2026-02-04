#include "TestEzGLTexture.h"
#include "glContext.h"
#include <ezlibs/ezGL/ezGL.hpp>
#include <ezlibs/ezCTest.hpp>
#include <ezlibs/ezMath.hpp>

bool TestEzGL_Texture_CreateEmpty() {
    CTEST_ASSERT(GLContext::initGLContext());

    auto texture = ez::gl::Texture::createEmpty(256, 256, "repeat", "linear", false);
    CTEST_ASSERT(texture != nullptr);
    CTEST_ASSERT(texture->isValid());
    CTEST_ASSERT(texture->getWidth() == 256);
    CTEST_ASSERT(texture->getHeight() == 256);

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Texture_CreateEmpty_WithMipmap() {
    CTEST_ASSERT(GLContext::initGLContext());

    auto texture = ez::gl::Texture::createEmpty(128, 128, "clamp", "nearest", true);
    CTEST_ASSERT(texture != nullptr);
    CTEST_ASSERT(texture->isValid());

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Texture_CreateFromBuffer_RGBA() {
    CTEST_ASSERT(GLContext::initGLContext());

    uint8_t buffer[256 * 256 * 4];
    for (int i = 0; i < 256 * 256 * 4; ++i) {
        buffer[i] = static_cast<uint8_t>(i % 256);
    }

    auto texture = ez::gl::Texture::createFromBuffer(
        buffer, 256, 256, 4, GL_UNSIGNED_BYTE, "repeat", "linear", false);
    CTEST_ASSERT(texture != nullptr);
    CTEST_ASSERT(texture->isValid());
    CTEST_ASSERT(texture->getWidth() == 256);
    CTEST_ASSERT(texture->getHeight() == 256);

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Texture_CreateFromBuffer_RGB() {
    CTEST_ASSERT(GLContext::initGLContext());

    uint8_t buffer[128 * 128 * 3];
    for (int i = 0; i < 128 * 128 * 3; ++i) {
        buffer[i] = static_cast<uint8_t>(i % 256);
    }

    auto texture = ez::gl::Texture::createFromBuffer(
        buffer, 128, 128, 3, GL_UNSIGNED_BYTE, "mirror", "linear", false);
    CTEST_ASSERT(texture != nullptr);
    CTEST_ASSERT(texture->isValid());

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Texture_CreateFromBuffer_Grayscale() {
    CTEST_ASSERT(GLContext::initGLContext());

    uint8_t buffer[64 * 64];
    for (int i = 0; i < 64 * 64; ++i) {
        buffer[i] = static_cast<uint8_t>(i % 256);
    }

    auto texture = ez::gl::Texture::createFromBuffer(
        buffer, 64, 64, 1, GL_UNSIGNED_BYTE, "clamp", "nearest", false);
    CTEST_ASSERT(texture != nullptr);
    CTEST_ASSERT(texture->isValid());

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Texture_CreateFromBuffer_WithFormat() {
    CTEST_ASSERT(GLContext::initGLContext());

    uint8_t buffer[256 * 256 * 4];
    for (int i = 0; i < 256 * 256 * 4; ++i) {
        buffer[i] = static_cast<uint8_t>(i % 256);
    }

    auto texture = ez::gl::Texture::createFromBuffer(
        buffer, 256, 256, GL_RGBA32F, GL_RGBA, GL_UNSIGNED_BYTE, "repeat", "linear", false);
    CTEST_ASSERT(texture != nullptr);
    CTEST_ASSERT(texture->isValid());

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Texture_Bind() {
    CTEST_ASSERT(GLContext::initGLContext());

    auto texture = ez::gl::Texture::createEmpty(64, 64, "repeat", "linear", false);
    CTEST_ASSERT(texture != nullptr);

    texture->bind();
    CTEST_ASSERT(texture->getId() != 0);

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Texture_Unbind() {
    CTEST_ASSERT(GLContext::initGLContext());

    auto texture = ez::gl::Texture::createEmpty(64, 64, "repeat", "linear", false);
    CTEST_ASSERT(texture != nullptr);

    texture->bind();
    texture->unbind();
    CTEST_ASSERT(texture->getId() != 0);

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Texture_Resize() {
    CTEST_ASSERT(GLContext::initGLContext());

    auto texture = ez::gl::Texture::createEmpty(64, 64, "repeat", "linear", false);
    CTEST_ASSERT(texture != nullptr);
    CTEST_ASSERT(texture->getWidth() == 64);
    CTEST_ASSERT(texture->getHeight() == 64);

    CTEST_ASSERT(texture->resize(128, 128));
    CTEST_ASSERT(texture->getWidth() == 128);
    CTEST_ASSERT(texture->getHeight() == 128);

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Texture_GetChannelsCount() {
    CTEST_ASSERT(GLContext::initGLContext());

    auto texture = ez::gl::Texture::createEmpty(64, 64, "repeat", "linear", false);
    CTEST_ASSERT(texture != nullptr);
    CTEST_ASSERT(texture->getChannelsCount() > 0);

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Texture_SetPixels() {
    CTEST_ASSERT(GLContext::initGLContext());
    auto texture = ez::gl::Texture::createEmpty(5, 5, "repeat", "linear", false);
    CTEST_ASSERT(texture != nullptr);

    constexpr size_t len = 5 * 5;
    std::vector<ez::fvec4> buffer(len);
    for (auto& it : buffer) { it = ez::fvec4(1.0f, 0.5f, 0.0f, 1.0f); }

    CTEST_ASSERT((texture->setPixels<ez::fvec4, 1>(0, 0, 5, 5, buffer.data())));

    buffer.clear();
    CTEST_ASSERT(buffer.empty());

    buffer = texture->getPixels<ez::fvec4, 1>(5, 5);
    CTEST_ASSERT(!buffer.empty());

    CTEST_ASSERT(buffer[20] == ez::fvec4(1.0f, 0.5f, 0.0f, 1.0f));

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Texture_SetPixels_PartialUpdate() {
    CTEST_ASSERT(GLContext::initGLContext());
    auto texture = ez::gl::Texture::createEmpty(5, 5, "repeat", "linear", false);
    CTEST_ASSERT(texture != nullptr);

    // Initialise toute la texture à zéro
    constexpr size_t totalLen = 5 * 5;
    std::vector<ez::fvec4> zeros(totalLen, ez::fvec4(0.0f, 0.0f, 0.0f, 0.0f));
    CTEST_ASSERT((texture->setPixels<ez::fvec4, 1>(0, 0, 5, 5, zeros.data())));

    // Update partiel sur la zone (1,1) -> (4,4)
    constexpr size_t len = 4 * 4;
    std::vector<ez::fvec4> buffer(len);
    for (auto& it : buffer) { it = ez::fvec4(1.0f, 0.5f, 0.0f, 1.0f); }
    CTEST_ASSERT((texture->setPixels<ez::fvec4, 1>(1, 1, 4, 4, buffer.data())));

    // Relecture
    auto pixels = texture->getPixels<ez::fvec4, 1>(5, 5);
    CTEST_ASSERT(!pixels.empty());

    // Row 0 : toute la ligne est intacte (y=0, hors de la zone)
    CTEST_ASSERT(pixels[0] == ez::fvec4(0.0f, 0.0f, 0.0f, 0.0f));  // (0,0)
    CTEST_ASSERT(pixels[1] == ez::fvec4(0.0f, 0.0f, 0.0f, 0.0f));  // (1,0)
    CTEST_ASSERT(pixels[4] == ez::fvec4(0.0f, 0.0f, 0.0f, 0.0f));  // (4,0)

    // Row 1 : x=0 intact, x=1..4 modifié
    CTEST_ASSERT(pixels[5] == ez::fvec4(0.0f, 0.0f, 0.0f, 0.0f));  // (0,1)
    CTEST_ASSERT(pixels[6] == ez::fvec4(1.0f, 0.5f, 0.0f, 1.0f));  // (1,1)
    CTEST_ASSERT(pixels[7] == ez::fvec4(1.0f, 0.5f, 0.0f, 1.0f));  // (2,1)
    CTEST_ASSERT(pixels[8] == ez::fvec4(1.0f, 0.5f, 0.0f, 1.0f));  // (3,1)
    CTEST_ASSERT(pixels[9] == ez::fvec4(1.0f, 0.5f, 0.0f, 1.0f));  // (4,1)

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Texture_GenerateMipmap() {
    CTEST_ASSERT(GLContext::initGLContext());

    auto texture = ez::gl::Texture::createEmpty(256, 256, "repeat", "linear", true);
    CTEST_ASSERT(texture != nullptr);

    texture->generateMipmap();
    CTEST_ASSERT(texture->isValid());

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Texture_Clear() {
    CTEST_ASSERT(GLContext::initGLContext());

    auto texture = ez::gl::Texture::createEmpty(64, 64, "repeat", "linear", false);
    CTEST_ASSERT(texture != nullptr);

    texture->clear();
    CTEST_ASSERT(texture->isValid());

    GLContext::unitGLContext();
    return true;
}

bool TestEzGL_Texture_Clear_WithColor() {
    CTEST_ASSERT(GLContext::initGLContext());

    auto texture = ez::gl::Texture::createEmpty(64, 64, "repeat", "linear", false);
    CTEST_ASSERT(texture != nullptr);

    texture->clear(255, 0, 0, 255);
    CTEST_ASSERT(texture->isValid());

    GLContext::unitGLContext();
    return true;
}

////////////////////////////////////////////////////////////////////////////
//// ENTRY POINT ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

bool TestEzGL_Texture(const std::string& vTest) {
    IfTestExist(TestEzGL_Texture_CreateEmpty);
    IfTestExist(TestEzGL_Texture_CreateEmpty_WithMipmap);
    IfTestExist(TestEzGL_Texture_CreateFromBuffer_RGBA);
    IfTestExist(TestEzGL_Texture_CreateFromBuffer_RGB);
    IfTestExist(TestEzGL_Texture_CreateFromBuffer_Grayscale);
    IfTestExist(TestEzGL_Texture_CreateFromBuffer_WithFormat);
    IfTestExist(TestEzGL_Texture_Bind);
    IfTestExist(TestEzGL_Texture_Unbind);
    IfTestExist(TestEzGL_Texture_Resize);
    IfTestExist(TestEzGL_Texture_GetChannelsCount);
    IfTestExist(TestEzGL_Texture_SetPixels);
    IfTestExist(TestEzGL_Texture_SetPixels_PartialUpdate);
    IfTestExist(TestEzGL_Texture_GenerateMipmap);
    IfTestExist(TestEzGL_Texture_Clear);
    IfTestExist(TestEzGL_Texture_Clear_WithColor);
    return false;
}
