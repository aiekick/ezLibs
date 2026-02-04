#pragma once

/*
MIT License

Copyright (c) 2014-2026 Stephane Cuillerdier (aka aiekick)

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

/* Simple compute Vfx
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

class CompVfx;
typedef std::shared_ptr<CompVfx> CompVfxPtr;
typedef std::weak_ptr<CompVfx> CompVfxWeak;

class CompVfx {
public:
    typedef std::function<void()> ActionPostRenderingFunctor;

private:
    CompVfxWeak m_This;
    std::string m_Name;
    ShaderPtr m_CompShaderPtr = nullptr;
    ProgramPtr m_ProgramPtr = nullptr;
    std::array<GLsizei, 3U> m_DispatchSize{};
    GLuint m_RenderIterations = 1U;
    bool m_RenderingPause = false;
    ActionPostRenderingFunctor m_ActionPostRenderingFunctor = nullptr;

public:
    static CompVfxPtr create(             //
        const std::string& vName,         //
        const std::string& vCompFile,     //
        const GLsizei& vDispatchSizeX,    //
        const GLsizei& vDispatchSizeY,    //
        const GLsizei& vDispatchSizeZ) {  //
        auto res = std::make_shared<CompVfx>();
        res->m_This = res;
        if (!res->init(vName, vCompFile, vDispatchSizeX, vDispatchSizeY, vDispatchSizeZ)) {
            res.reset();
        }
        return res;
    }

public:
    CompVfx() = default;
    ~CompVfx() {
        unit();
    }

    bool init(                            //
        const std::string& vName,         //
        const std::string& vCompFile,     //
        const GLsizei& vDispatchSizeX,    //
        const GLsizei& vDispatchSizeY,    //
        const GLsizei& vDispatchSizeZ) {  //
        ASSERT_THROW(!vCompFile.empty(), "");
        ASSERT_THROW(vDispatchSizeX > 0U, "");
        ASSERT_THROW(vDispatchSizeX > 0U, "");
        ASSERT_THROW(vDispatchSizeZ > 0U, "");
        m_Name = vName;
        m_DispatchSize[0] = vDispatchSizeX;
        m_DispatchSize[1] = vDispatchSizeY;
        m_DispatchSize[2] = vDispatchSizeZ;
        m_CompShaderPtr = ez::gl::Shader::createFromFile(vName, GL_COMPUTE_SHADER, vCompFile);
        if (m_CompShaderPtr != nullptr) {
            m_ProgramPtr = ez::gl::Program::create(vName);
            if (m_ProgramPtr != nullptr) {
                if (m_ProgramPtr->addShader(m_CompShaderPtr)) {
                    return m_ProgramPtr->link();
                }
            }
        }
        return false;
    }
    const char* getLabelName() {
        return m_Name.c_str();
    }
    const std::array<GLsizei, 3U>& getDispatchSize() {
        return m_DispatchSize;
    }
    void setRenderingIterations(const GLuint& vRenderingIterations) {
        m_RenderIterations = vRenderingIterations;
    }
    GLuint& getRenderingIterationsRef() {
        return m_RenderIterations;
    }
    void setRenderingPause(const bool& vRenderingPause) {
        m_RenderingPause = vRenderingPause;
    }
    bool& getRenderingPauseRef() {
        return m_RenderingPause;
    }
    void setUniformPreUploadFunctor(Program::UniformPreUploadFunctor vUniformPreUploadFunctor) {
        ASSERT_THROW(m_ProgramPtr != nullptr, "");
        m_ProgramPtr->setUniformPreUploadFunctor(vUniformPreUploadFunctor);
    }
    void setActionPostRenderingFunctor(ActionPostRenderingFunctor vActionPostRenderingFunctor) {
        m_ActionPostRenderingFunctor = vActionPostRenderingFunctor; }
    void addUniformFloat(
        const GLenum vShaderType,
        const std::string& vUniformName,
        float* vUniformPtr,
        const GLuint vCountChannels,
        const GLuint vCountElements = 1U,
        const bool vShowWidget = true,
        const Program::UniformWidgetFunctor& vWidgetFunctor = nullptr) {
        ASSERT_THROW(m_ProgramPtr != nullptr, "");
        m_ProgramPtr->addUniformFloat(vShaderType, vUniformName, vUniformPtr, vCountChannels, vCountElements, vShowWidget, vWidgetFunctor);
    }
    void setUniformFloatDatas(const GLenum vShaderType, const std::string& vUniformName, float* vUniformPtr) {
        ASSERT_THROW(m_ProgramPtr != nullptr, "");
        m_ProgramPtr->setUniformFloatDatas(vShaderType, vUniformName, vUniformPtr);
    }
    void addUniformInt(
        const GLenum vShaderType,
        const std::string& vUniformName,
        int32_t* vUniformPtr,
        const GLuint vCountChannels,
        const GLuint vCountElements = 1U,
        const bool vShowWidget = true,
        const Program::UniformWidgetFunctor& vWidgetFunctor = nullptr) {
        ASSERT_THROW(m_ProgramPtr != nullptr, "");
        m_ProgramPtr->addUniformInt(vShaderType, vUniformName, vUniformPtr, vCountChannels, vCountElements, vShowWidget, vWidgetFunctor);
    }
    void setUniformIntDatas(const GLenum vShaderType, const std::string& vUniformName, int32_t* vUniformPtr) {
        ASSERT_THROW(m_ProgramPtr != nullptr, "");
        m_ProgramPtr->setUniformIntDatas(vShaderType, vUniformName, vUniformPtr);
    }
    void addUniformUInt(
        const GLenum vShaderType,
        const std::string& vUniformName,
        uint32_t* vUniformPtr,
        const GLuint vCountChannels,
        const GLuint vCountElements = 1U,
        const bool vShowWidget = true,
        const Program::UniformWidgetFunctor& vWidgetFunctor = nullptr) {
        ASSERT_THROW(m_ProgramPtr != nullptr, "");
        m_ProgramPtr->addUniformUInt(vShaderType, vUniformName, vUniformPtr, vCountChannels, vCountElements, vShowWidget, vWidgetFunctor);
    }
    void setUniformUIntDatas(const GLenum vShaderType, const std::string& vUniformName, uint32_t* vUniformPtr) {
        ASSERT_THROW(m_ProgramPtr != nullptr, "");
        m_ProgramPtr->setUniformUIntDatas(vShaderType, vUniformName, vUniformPtr);
    }
    void addUniformImage2D(               //
        const GLenum& vShaderType,        //
        const std::string& vUniformName,  //
        int32_t vBinding,                 //
        uint32_t* vImage2DPtr,            //
        GLenum vImageFormat,              //
        GLenum vImageMode,                //
        const bool& vShowWidget) {
        ASSERT_THROW(m_ProgramPtr != nullptr, "");
        m_ProgramPtr->addUniformImage2D(vShaderType, vUniformName, vBinding, vImage2DPtr, vImageFormat, vImageMode, vShowWidget);
    }
    void addUniformSampler2D(const GLenum vShaderType, const std::string& vUniformName, uint32_t* vSampler2DPtr, const bool vShowWidget = true) {
        ASSERT_THROW(m_ProgramPtr != nullptr, "");
        m_ProgramPtr->addUniformSampler2D(vShaderType, vUniformName, vSampler2DPtr, vShowWidget);
    }
    void addBufferBlock(const GLenum vShaderType, const std::string& vBufferName, const int32_t vBinding, BufferBlock** vBufferPtr) {
        ASSERT_THROW(m_ProgramPtr != nullptr, "");
        m_ProgramPtr->addBufferBlock(vShaderType, vBufferName, vBinding, vBufferPtr);
    }
    void finalizeBeforeRendering() {
        ASSERT_THROW(m_ProgramPtr != nullptr, "");
        m_ProgramPtr->locateUniforms();
    }
    bool resize(const GLsizei& vSx, const GLsizei vSy) {
        // todo
        return false;
    }
    void clearBuffers(const std::array<float, 4U>& vColor) {
        //todo
    }
    void compute(const GLbitfield vMemoryBarrierBitfield = GL_ALL_BARRIER_BITS) {
        if (m_RenderingPause) {
            return;
        }
#ifdef PROFILER_SCOPED
        auto name_c_str = m_Name.c_str();  // remvoe some warnings
        PROFILER_SCOPED("VFX", "Compute %s", name_c_str);
#endif
        glPushDebugGroupKHR(GL_DEBUG_SOURCE_APPLICATION, 0, -1, m_Name.c_str());
        ASSERT_THROW(m_ProgramPtr != nullptr, "");
        for (GLuint idx = 0; idx < m_RenderIterations; ++idx) {
#ifdef PROFILER_SCOPED
            PROFILER_SCOPED("VFX", "Iter %i", idx);
#endif
            if (m_ProgramPtr->use()) {
                m_ProgramPtr->uploadUniforms(nullptr);
                {
#ifdef PROFILER_SCOPED
                    PROFILER_SCOPED("Compute", "glDispatchCompute");
#endif
                    glDispatchCompute(m_DispatchSize[0], m_DispatchSize[1], m_DispatchSize[2]);
                    CheckGLErrors;
                }
                {
#ifdef PROFILER_SCOPED
                    PROFILER_SCOPED("Compute", "glMemoryBarrier");
#endif
                    glMemoryBarrier(vMemoryBarrierBitfield);
                    CheckGLErrors;
                }
                m_ProgramPtr->unuse();
            }
            if (m_ActionPostRenderingFunctor != nullptr) {
                m_ActionPostRenderingFunctor();
            }
        }
        glPopDebugGroupKHR();
    }
#ifdef IMGUI_INCLUDE
    bool drawImGuiThumbnail(const float& vSx, const float& vSy, const float& vScaleInv, const bool vUseButton) {
        /*ASSERT_THROW(m_FBOPipeLinePtr != nullptr, "");
        auto front_fbo_ptr = m_FBOPipeLinePtr->getFrontFBO().lock();
        if (front_fbo_ptr != nullptr) {
            const auto texId = front_fbo_ptr->getTextureId();
            if (texId > 0U) {
                if (vUseButton) {
                    return ImGui::ImageButton(m_Name.c_str(), (ImTextureID)(size_t)texId, ImVec2(vSx, vSy), ImVec2(0, vScaleInv), ImVec2(vScaleInv, 0));
                } else {
                    ImGui::Image((ImTextureID)(size_t)texId, ImVec2(vSx, vSy), ImVec2(0, vScaleInv), ImVec2(vScaleInv, 0));
                }
            }
        }*/
        return false;
    }
    void drawUniformWidgets(Program::AdditionnalWidgetsFunctor vAdditionnalWidgetsFunctor = nullptr) {
        ASSERT_THROW(m_ProgramPtr != nullptr, "");
        m_ProgramPtr->drawUniformWidgets(vAdditionnalWidgetsFunctor);
    }
#endif
    void unit() {
        m_ProgramPtr.reset();
    }
};

}  // namespace gl
}  // namespace ez
