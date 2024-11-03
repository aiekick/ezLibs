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

// EzGuiRenderer is part od the EzLibs project : https://github.com/aiekick/EzLibs.git

#include <ezlibs/ezGL/ezGL.hpp>
#include <ezlibs/ezGui/ezGui.hpp>

#include <cstdint>
#include <string>
#include <unordered_map>

namespace ez {
namespace gui {

struct GuiDatas {
    GLfloat t[2] = {};  // tex
    GLfloat v[2] = {};  // vert
    GLfloat c[4] = {};  // color
};

class GuiMesh;
typedef std::shared_ptr<GuiMesh> GuiMeshPtr;
typedef std::weak_ptr<GuiMesh> GuiMeshWeak;

class GuiMesh : public ez::gl::Mesh<GuiDatas> {
public:
    static constexpr int32_t sBUFFER_SIZE = 16384;

private:
    GuiMeshWeak m_This;

public:
    static GuiMeshPtr create() {
        auto res = std::make_shared<GuiMesh>();
        res->m_This = res;
        std::vector<GuiDatas> vertices;
        std::vector<uint32_t> indices;
        vertices.resize(sBUFFER_SIZE * 4);
        indices.resize(sBUFFER_SIZE * 6);
        if (!res->init(vertices, indices, {2, 2, 4}, false)) {
            res.reset();
        }
        return res;
    }
};

class GLRenderer {
private:
    ez::gl::ShaderPtr m_vertShaderPtr = nullptr;
    ez::gl::ShaderPtr m_fragShaderPtr = nullptr;
    ez::gl::ProgramPtr m_progPtr = nullptr;
    ez::gl::TexturePtr m_fontTexturePtr = nullptr;
    GuiMeshPtr m_guiMeshPtrPtr = nullptr;
    uint32_t m_quadIdx = 0U;

    uint32_t m_width = 0U;
    uint32_t m_height = 0U;
    std::array<std::array<float, 4>, 4> m_ProjMatrix = {};

    const std::string vertShader =
        u8R"(
#version 450
layout (location = 0) in vec2 Position;
layout (location = 1) in vec2 UV;
layout (location = 2) in vec4 Color;
uniform mat4 uProjMtx;
out vec2 Frag_UV;
out vec4 Frag_Color;
void main()
{
    Frag_UV = UV;
    Frag_Color = Color;
    gl_Position = uProjMtx * vec4(Position.xy, 0.0, 1.0);
};
)";

    const std::string fragShader =
        u8R"(
#version 450
in vec2 Frag_UV;
in vec4 Frag_Color;
uniform sampler2D uTexture;
layout (location = 0) out vec4 Out_Color;
void main() {
    Out_Color = Frag_Color * texture(uTexture, Frag_UV.st).a;
};
)";

public:
    bool init() {
        m_vertShaderPtr = ez::gl::Shader::createFromCode("vert", GL_VERTEX_SHADER, vertShader);
        if (m_vertShaderPtr != nullptr) {
            m_fragShaderPtr = ez::gl::Shader::createFromCode("frag", GL_FRAGMENT_SHADER, fragShader);
            if (m_fragShaderPtr != nullptr) {
                m_progPtr = ez::gl::Program::create("prog");
                if (m_progPtr != nullptr) {
                    m_progPtr->addShader(m_vertShaderPtr);
                    m_progPtr->addShader(m_fragShaderPtr);
                    if (m_progPtr->link()) {
                        /*m_fontTexturePtr = ez::gl::Texture::createFromBuffer(  //
                            atlas_texture,
                            ATLAS_WIDTH,
                            ATLAS_HEIGHT,
                            GL_ALPHA,
                            GL_ALPHA,
                            GL_UNSIGNED_BYTE,
                            "clamp",
                            "nearest",
                            false);
                        if (m_fontTexturePtr != nullptr) {*/
                        m_guiMeshPtrPtr = GuiMesh::create();
                        if (m_guiMeshPtrPtr != nullptr) {
                            m_progPtr->addUniformMatrix(GL_VERTEX_SHADER, "uProjMtx", &m_ProjMatrix[0][0], 4, false, nullptr);
                            //m_progPtr->addUniformSampler2D(GL_FRAGMENT_SHADER, "uTexture", m_fontTexturePtr->getTexId(), false);
                            m_progPtr->locateUniforms();
                            return true;
                        }
                        //}
                    }
                }
            }
        }

        return false;
    }

    void unit() {
        m_guiMeshPtrPtr.reset();
        m_fontTexturePtr.reset();
        m_progPtr.reset();
        m_fragShaderPtr.reset();
        m_vertShaderPtr.reset();
    }

    void newFrame(uint32_t vWidth, uint32_t vHeight) {
        if (m_width != vWidth || m_height != vHeight) {
            //auto rc = mu_rect(0, 0, vWidth, vHeight);
            //mu_set_clip(m_ctxPtr, rc);
            //m_updateMatrix(rc);
        }
        m_width = vWidth;
        m_height = vHeight;
    }

    void flush() {
        if (m_quadIdx == 0) {
            return;
        }

        //m_updateMatrix(mu_rect(0, 0, m_width, m_height));
        glViewport(0, 0, m_width, m_height);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);
        glEnable(GL_TEXTURE_2D);
        if (m_progPtr->use()) {
            int qidx = m_quadIdx - 1;
            int vidx = qidx * 4;
            int iidx = qidx * 6;
            m_progPtr->uploadUniforms(nullptr);
            m_guiMeshPtrPtr->render(GL_TRIANGLES, vidx, iidx);
            m_progPtr->unuse();
        }
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_SCISSOR_TEST);
        glDisable(GL_BLEND);

        m_quadIdx = 0;
    }

    /*void drawRect(mu_Rect rect, mu_Color color) {
        m_pushQuad(rect, atlas[ATLAS_WHITE], color);
    }

    void drawText(const char* text, mu_Vec2 pos, mu_Color color) {
        mu_Rect dst = {pos.x, pos.y, 0, 0};
        for (const char* p = text; *p; p++) {
            if ((*p & 0xc0) == 0x80) {
                continue;
            }
            int chr = mu_min((unsigned char)*p, 127);
            mu_Rect src = atlas[ATLAS_FONT + chr];
            dst.w = src.w;
            dst.h = src.h;
            m_pushQuad(dst, src, color);
            dst.x += dst.w;
        }
    }

    void drawIcon(int id, mu_Rect rect, mu_Color color) {
        mu_Rect src = atlas[id];
        int x = rect.x + (rect.w - src.w) / 2;
        int y = rect.y + (rect.h - src.h) / 2;
        m_pushQuad(mu_rect(x, y, src.w, src.h), src, color);
    }

    int getTextWidth(const char* text, int len) {
        int res = 0;
        for (const char* p = text; *p && len--; p++) {
            if ((*p & 0xc0) == 0x80) {
                continue;
            }
            int chr = mu_min((unsigned char)*p, 127);
            res += atlas[ATLAS_FONT + chr].w;
        }
        return res;
    }

    int getTextHeight() {
        return 18;
    }

    void setClipRect(mu_Rect rect) {
        rect.y = m_height - (rect.y + rect.h);
        glScissor(rect.x, rect.y, rect.w, rect.h);
    }

    void clear(mu_Color clr) {
        glClearColor(clr.r / 255.0f, clr.g / 255.0f, clr.b / 255.0f, clr.a / 255.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }*/

private:
    /*void m_updateMatrix(mu_Rect rc) {
        const float L = rc.x;
        const float R = rc.x + rc.w;
        const float T = rc.y;
        const float B = rc.y + rc.h;
        m_ProjMatrix[0] = {2.0f / (R - L), 0.0f, 0.0f, 0.0f};
        m_ProjMatrix[1] = {0.0f, 2.0f / (T - B), 0.0f, 0.0f};
        m_ProjMatrix[2] = {0.0f, 0.0f, -1.0f, 0.0f};
        m_ProjMatrix[3] = {(R + L) / (L - R), (T + B) / (B - T), 0.0f, 1.0f};
        m_progPtr->setUniformMatrixDatas(GL_VERTEX_SHADER, "uProjMtx", &m_ProjMatrix[0][0]);
    }

    void m_pushQuad(mu_Rect dst, mu_Rect src, mu_Color color) {
        if (m_quadIdx == (GuiMesh::sBUFFER_SIZE * 4)) {
            flush();
        }

        int vert_idx = m_quadIdx * 4;
        int ind_idx = m_quadIdx * 6;
        ++m_quadIdx;

        auto& vertices = m_guiMeshPtrPtr->getVerticesRef();
        auto& indices = m_guiMeshPtrPtr->getIndicesRef();

        GLfloat x = src.x / (float)ATLAS_WIDTH;
        GLfloat y = src.y / (float)ATLAS_HEIGHT;
        GLfloat w = src.w / (float)ATLAS_WIDTH;
        GLfloat h = src.h / (float)ATLAS_HEIGHT;

        // v0
        auto& v0 = vertices[vert_idx + 0];
        v0.v[0] = (GLfloat)(x);
        v0.v[1] = (GLfloat)(y);
        v0.t[0] = (GLfloat)(dst.x);
        v0.t[1] = (GLfloat)(dst.y);
        v0.c[0] = color.r / 255.0f;
        v0.c[1] = color.g / 255.0f;
        v0.c[2] = color.b / 255.0f;
        v0.c[3] = color.a / 255.0f;

        // v1
        auto& v1 = vertices[vert_idx + 1];
        v1.v[0] = (GLfloat)(x + w);
        v1.v[1] = (GLfloat)(y);
        v1.t[0] = (GLfloat)(dst.x + dst.w);
        v1.t[1] = (GLfloat)(dst.y);
        v1.c[0] = color.r / 255.0f;
        v1.c[1] = color.g / 255.0f;
        v1.c[2] = color.b / 255.0f;
        v1.c[3] = color.a / 255.0f;

        // v2
        auto& v2 = vertices[vert_idx + 2];
        v2.v[0] = (GLfloat)(x);
        v2.v[1] = (GLfloat)(y + h);
        v2.t[0] = (GLfloat)(dst.x);
        v2.t[1] = (GLfloat)(dst.y + dst.h);
        v2.c[0] = color.r / 255.0f;
        v2.c[1] = color.g / 255.0f;
        v2.c[2] = color.b / 255.0f;
        v2.c[3] = color.a / 255.0f;

        // v3
        auto& v3 = vertices[vert_idx + 3];
        v3.v[0] = (GLfloat)(x + w);
        v3.v[1] = (GLfloat)(y + h);
        v3.t[0] = (GLfloat)(dst.x + dst.w);
        v3.t[1] = (GLfloat)(dst.y + dst.h);
        v3.c[0] = color.r / 255.0f;
        v3.c[1] = color.g / 255.0f;
        v3.c[2] = color.b / 255.0f;
        v3.c[3] = color.a / 255.0f;

        indices[ind_idx + 0] = vert_idx + 0;
        indices[ind_idx + 1] = vert_idx + 1;
        indices[ind_idx + 2] = vert_idx + 2;
        indices[ind_idx + 3] = vert_idx + 2;
        indices[ind_idx + 4] = vert_idx + 3;
        indices[ind_idx + 5] = vert_idx + 1;

        m_guiMeshPtrPtr->needNewUpload();
    }*/
};

}  // namespace gui
}  // namespace ez
