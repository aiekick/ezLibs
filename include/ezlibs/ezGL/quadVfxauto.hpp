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

/* Simple quad Vfx
 * Quad Mesh
 * FBO with one attachment
 * One vertex Shader quad
 * One fragment Shader
 */

#include "ezGL.hpp"

#ifdef IMGUI_INCLUDE
#include IMGUI_INCLUDE
#endif

#include <array>
#include <memory>
#include <cassert>
#include <functional>

namespace ez {
namespace gl {

class QuadVfxAuto;
typedef std::shared_ptr<QuadVfxAuto> QuadVfxAutoPtr;
typedef std::weak_ptr<QuadVfxAuto> QuadVfxAutoWeak;

class QuadVfxAuto {
private:
    QuadVfxAutoWeak m_This;
    std::string m_Name;
    QuadMeshWeak m_QuadMesh;
    ShaderAutoWeak m_VertShader;
    FBOPipeLinePtr m_FBOPipeLinePtr = nullptr;
    ShaderAutoPtr m_FragShaderPtr = nullptr;
    ProgramAutoPtr m_ProgramPtr = nullptr;
    std::array<GLuint, 2U> m_Size;
    bool m_UseMipMapping = false;
    bool m_MultiPass = false;
    GLuint m_RenderIterations = 1U;
    bool m_RenderingPause = false;

public:
    static QuadVfxAutoPtr create(       //
        const std::string& vName,       //
        ShaderAutoWeak vVertShader,     //
        QuadMeshWeak vQuadMesh,         //
        const std::string& vFragFile,   //
        const GLsizei& vSx,             //
        const GLsizei& vSy,             //
        const uint32_t& vCountBuffers,  //
        const bool vUseMipMapping,      //
        const bool vMultiPass) {        //
        auto res = std::make_shared<QuadVfxAuto>();
        res->m_This = res;
        if (!res->init(vName, vVertShader, vQuadMesh, vFragFile, vSx, vSy, vCountBuffers, vUseMipMapping, vMultiPass)) {
            res.reset();
        }
        return res;
    }

public:
    QuadVfxAuto() = default;
    ~QuadVfxAuto() {
        unit();
    }

    bool init(                          //
        const std::string& vName,       //
        ShaderAutoWeak vVertShader,     //
        QuadMeshWeak vQuadMesh,         //
        const std::string& vFragFile,   //
        const GLsizei& vSx,             //
        const GLsizei& vSy,             //
        const uint32_t& vCountBuffers,  //
        const bool vUseMipMapping,      //
        const bool vMultiPass) {        //
        assert(!vVertShader.expired());
        assert(!vQuadMesh.expired());
        assert(!vFragFile.empty());
        assert(vSx > 0);
        assert(vSy > 0);
        assert(vCountBuffers > 0U);
        m_Name = vName;
        m_VertShader = vVertShader;
        m_QuadMesh = vQuadMesh;
        m_Size[0] = vSx;
        m_Size[1] = vSy;
        m_UseMipMapping = vUseMipMapping;
        m_MultiPass = vMultiPass;
        m_FBOPipeLinePtr = FBOPipeLine::create(vSx, vSy, vCountBuffers, vUseMipMapping, m_MultiPass);
        if (m_FBOPipeLinePtr != nullptr) {
            m_FragShaderPtr = ez::gl::ShaderAuto::createFromFile(vName, GL_FRAGMENT_SHADER, vFragFile);
            if (m_FragShaderPtr != nullptr) {
                m_ProgramPtr = ez::gl::ProgramAuto::create(vName);
                if (m_ProgramPtr != nullptr) {
                    if (m_ProgramPtr->addShader(m_VertShader)) {
                        if (m_ProgramPtr->addShader(m_FragShaderPtr)) {
                            return m_ProgramPtr->link();
                        }
                    }
                }
            }
        }
        return false;
    }
    const char* getLabelName() {
        return m_Name.c_str();
    }
    const std::array<GLuint, 2U>& getSize() {
        return m_Size;
    }
    void setRenderingIterations(const GLuint vRenderingIterations) {
        m_RenderIterations = vRenderingIterations;
    }
    GLuint& getRenderingIterationsRef() {
        return m_RenderIterations;
    }
    void setRenderingPause(const bool vRenderingPause) {
        m_RenderingPause = vRenderingPause;
    }
    bool& getRenderingPauseRef() {
        return m_RenderingPause;
    }
    void setUniformPreUploadFunctor(ProgramAuto::UniformPreUploadFunctor vUniformPreUploadFunctor) {
        assert(m_ProgramPtr != nullptr);
        m_ProgramPtr->setUniformPreUploadFunctor(vUniformPreUploadFunctor);
    }
    void addBufferBlock(const GLenum vShaderType, const std::string& vBufferName, const int32_t vBinding, BufferBlock* vBufferPtr) {
        assert(m_ProgramPtr != nullptr);
        m_ProgramPtr->addBufferBlock(vShaderType, vBufferName, vBinding, vBufferPtr);
    }
    void addUniformFloat(                 //
        const GLenum vShaderType,         //
        const std::string& vUniformName,  //
        float* vUniformPtr,               //
        const GLuint vCountChannels,      //
        const bool vShowWidget,           //
        const ProgramAuto::UniformWidgetFunctor& vWidgetFunctor) {
        assert(m_ProgramPtr != nullptr);
        m_ProgramPtr->addUniformFloat(vShaderType, vUniformName, vUniformPtr, vCountChannels, vShowWidget, vWidgetFunctor);
    }
    void addUniformInt(                   //
        const GLenum vShaderType,         //
        const std::string& vUniformName,  //
        int32_t* vUniformPtr,             //
        const GLuint vCountChannels,      //
        const bool vShowWidget,           //
        const ProgramAuto::UniformWidgetFunctor& vWidgetFunctor) {
        assert(m_ProgramPtr != nullptr);
        m_ProgramPtr->addUniformInt(vShaderType, vUniformName, vUniformPtr, vCountChannels, vShowWidget, vWidgetFunctor);
    }
    void addUniformSampler2D(             //
        const GLenum vShaderType,         //
        const std::string& vUniformName,  //
        int32_t vSampler2D,               //
        const bool vShowWidget,           //
        const ProgramAuto::UniformWidgetFunctor& vWidgetFunctor) {
        assert(m_ProgramPtr != nullptr);
        m_ProgramPtr->addUniformSampler2D(vShaderType, vUniformName, vSampler2D, vShowWidget, vWidgetFunctor);
    }
    void finalizeBeforeRendering() {
        assert(m_ProgramPtr != nullptr);
        m_ProgramPtr->locateUniforms();
    }
    bool resize(const GLsizei& vSx, const GLsizei vSy) {
        assert(m_FBOPipeLinePtr != nullptr);
        if (m_FBOPipeLinePtr->resize(vSx, vSy)) {
            m_Size[0] = vSx;
            m_Size[1] = vSy;
            return true;
        }
        return false;
    }
    void clearBuffers(const std::array<float, 4U>& vColor) {
        assert(m_FBOPipeLinePtr != nullptr);
        m_FBOPipeLinePtr->clearBuffer(vColor);
    }
    void render() {
        if (m_RenderingPause) {
            return;
        }
#ifdef PROFILER_SCOPED
        auto name_c_str = m_Name.c_str();  // remvoe some warnings
        PROFILER_SCOPED("VFX", "Render %s", name_c_str);
#endif
        auto quad_ptr = m_QuadMesh.lock();
        assert(quad_ptr != nullptr);
        assert(m_FBOPipeLinePtr != nullptr);
        assert(m_ProgramPtr != nullptr);
        for (GLuint idx = 0; idx < m_RenderIterations; ++idx) {
#ifdef PROFILER_SCOPED
            PROFILER_SCOPED("VFX", "Iter %i", idx);
#endif
            if (m_FBOPipeLinePtr->bind()) {
                if (m_ProgramPtr->use()) {
                    m_ProgramPtr->uploadUniforms(m_FBOPipeLinePtr);
                    m_FBOPipeLinePtr->selectBuffers();
                    {
#ifdef PROFILER_SCOPED
                        PROFILER_SCOPED("Opengl", "glViewport");
#endif
                        glViewport(0, 0, m_Size[0], m_Size[1]);
                    }
                    quad_ptr->render(GL_TRIANGLES);
                    m_ProgramPtr->unuse();
                }
                m_FBOPipeLinePtr->updateMipMaping();
                m_FBOPipeLinePtr->unbind();
            }
        }
    }
    GLuint getTextureId() {
        assert(m_FBOPipeLinePtr != nullptr);
        auto front_fbo_ptr = m_FBOPipeLinePtr->getFrontFBO().lock();
        if (front_fbo_ptr != nullptr) {
            return front_fbo_ptr->getTextureId();
        }
        return 0U;
    }
#ifdef IMGUI_INCLUDE
    void drawImGuiThumbnail(const float& vSx, const float& vSy, const float& vScaleInv) {
        assert(m_FBOPipeLinePtr != nullptr);
        auto front_fbo_ptr = m_FBOPipeLinePtr->getFrontFBO().lock();
        if (front_fbo_ptr != nullptr) {
            const auto texId = front_fbo_ptr->getTextureId();
            if (texId > 0U) {
                ImGui::ImageButton(m_Name.c_str(), (ImTextureID)(size_t)texId, ImVec2(vSx, vSy), ImVec2(0, vScaleInv), ImVec2(vScaleInv, 0));
            }
        }
    }
    void drawUniformWidgets() {
        assert(m_ProgramPtr != nullptr);
        m_ProgramPtr->drawUniformWidgets();
    }
#endif
    void unit() {
        m_ProgramPtr.reset();
        m_FragShaderPtr.reset();
        m_FBOPipeLinePtr.reset();
    }
};

}  // namespace gl
}  // namespace ez
