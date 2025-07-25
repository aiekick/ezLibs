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

#include <vector>
#include <memory>
#include <string>
#include <cassert>
#include <functional>

#include "ezGL.hpp"

#ifdef IMGUI_INCLUDE
#include IMGUI_INCLUDE
#endif

namespace ez {
namespace gl {

class ProgramAuto;
typedef std::shared_ptr<ProgramAuto> ProgramAutoPtr;
typedef std::weak_ptr<ProgramAuto> ProgramAutoWeak;

class ProgramAuto {
public:
    struct Uniform;
    typedef std::map<GLenum, std::map<std::string, Uniform>> UniformPerShaderTypeContainer;
    typedef std::function<void(FBOPipeLinePtr, Uniform&)> UniformPreUploadFunctor;
    typedef std::function<void(Uniform&)> UniformWidgetFunctor;
    struct Uniform {
        std::string name;
        float* datas_f = nullptr;    // float
        int32_t* datas_i = nullptr;  // int
        int32_t data_s2d = -1;       // sampler2D
        GLint loc = -1;
        GLuint channels = 1U;
        bool used = false;
        bool showed = false;
        BufferBlock* buffer_ptr = nullptr;  // a buffer block ex: UBO /SSBO
        int32_t bufferBinding = -1;  // the binding point in the sahder of the buffer block
        UniformWidgetFunctor widget_functor = nullptr;
    };

private:
    ProgramAutoWeak m_This;
    GLuint m_ProgramAutoId = 0U;
    std::string m_ProgramAutoName;
    std::map<uintptr_t, ShaderAutoWeak> m_Shaders;  // a same shader object can be added two times
    UniformPerShaderTypeContainer m_Uniforms;
    UniformPreUploadFunctor m_UniformPreUploadFunctor = nullptr;  // lanbda to execute just before the uniform upload
    UniformsManager m_UniformsManager;

public:
    static ProgramAutoPtr create(const std::string& vProgramAutoName) {
        auto res = std::make_shared<ProgramAuto>();
        res->m_This = res;
        if (!res->init(vProgramAutoName)) {
            res.reset();
        }
        return res;
    }

public:
    ProgramAuto() = default;
    ~ProgramAuto() {
        unit();
    }
    bool init(const std::string& vProgramAutoName) {
        assert(!vProgramAutoName.empty());
        m_ProgramAutoName = vProgramAutoName;
        m_ProgramAutoId = glCreateProgram();
        CheckGLErrors;
        if (m_ProgramAutoId > 0U) {
            return true;
        }
        return false;
    }
    bool addShader(ShaderAutoWeak vShader) {
        if (!vShader.expired()) {
            m_Shaders[(uintptr_t)vShader.lock().get()] = vShader;
            return true;
        }
        return false;
    }
    bool link() {
        bool res = false;
        if (m_ProgramAutoId > 0U) {
            bool one_shader_at_least = false;
            for (auto& shader : m_Shaders) {
                auto ptr = shader.second.lock();
                if (ptr != nullptr) {
                    one_shader_at_least = true;
                    glAttachShader(m_ProgramAutoId, ptr->getShaderId());
                    CheckGLErrors;
                    // we could delete shader id after linking,
                    // but we dont since we can have many shader for the same program
                }
            }
            if (one_shader_at_least) {
                glLinkProgram(m_ProgramAutoId);
                CheckGLErrors;
                glFinish();
                GLint linked = 0;
                glGetProgramiv(m_ProgramAutoId, GL_LINK_STATUS, &linked);
                CheckGLErrors;
                if (!linked) {
                    if (!printProgramAutoLogs(m_ProgramAutoName, "Link Errors")) {
                        LogVarError("ProgramAuto \"%s\" linking fail for unknown reason", m_ProgramAutoName.c_str());
                    }
                    res = false;
                } else {
                    printProgramAutoLogs(m_ProgramAutoName, "Link Warnings");
                    res = true;
                }
            }
        }
        return res;
    }
    void setUniformPreUploadFunctor(UniformPreUploadFunctor vUniformPreUploadFunctor) { m_UniformPreUploadFunctor = vUniformPreUploadFunctor; }
    void addBufferBlock(const GLenum vShaderType, const std::string& vBufferName, const int32_t vBinding, BufferBlock* vBufferPtr) {
        assert(vShaderType > 0);
        assert(!vBufferName.empty());
        assert(vBinding > 0);
        assert(vBufferPtr != nullptr);
        Uniform uni;
        uni.bufferBinding = vBinding;
        uni.buffer_ptr = vBufferPtr;
        m_Uniforms[vShaderType][vBufferName] = uni;
    }
    void addUniformFloat(const GLenum vShaderType, const std::string& vUniformName, float* vUniformPtr, const GLuint vCountChannels,
                         const bool vShowWidget, const UniformWidgetFunctor& vWidgetFunctor) {
        assert(vShaderType > 0);
        assert(!vUniformName.empty());
        assert(vUniformPtr != nullptr);
        assert(vCountChannels > 0U);
        Uniform uni;
        uni.name = vUniformName;
        uni.showed = vShowWidget;
        uni.datas_f = vUniformPtr;
        uni.channels = vCountChannels;
        uni.widget_functor = vWidgetFunctor;
        m_Uniforms[vShaderType][vUniformName] = uni;
    }
    void addUniformInt(const GLenum vShaderType, const std::string& vUniformName, int32_t* vUniformPtr, const GLuint vCountChannels,
                       const bool vShowWidget, const UniformWidgetFunctor& vWidgetFunctor) {
        assert(vShaderType > 0);
        assert(!vUniformName.empty());
        assert(vUniformPtr != nullptr);
        assert(vCountChannels > 0U);
        Uniform uni;
        uni.name = vUniformName;
        uni.showed = vShowWidget;
        uni.datas_i = vUniformPtr;
        uni.channels = vCountChannels;
        uni.widget_functor = vWidgetFunctor;
        m_Uniforms[vShaderType][vUniformName] = uni;
    }
    void addUniformSampler2D(const GLenum vShaderType, const std::string& vUniformName, int32_t vSampler2D,
                             const bool vShowWidget, const UniformWidgetFunctor& vWidgetFunctor) {
        assert(vShaderType > 0);
        assert(!vUniformName.empty());
        // assert(vSampler2D != -1);, if the sampler must point on a buffer after, its normal to have it at -1
        Uniform uni;
        uni.name = vUniformName;
        uni.showed = vShowWidget;
        uni.data_s2d = vSampler2D;
        uni.widget_functor = vWidgetFunctor;
        m_Uniforms[vShaderType][vUniformName] = uni;
    }
    void uploadUniforms(FBOPipeLinePtr vFBOPipeLinePtr) {
#ifdef PROFILER_SCOPED
        PROFILER_SCOPED(m_ProgramAutoName, "uploadUniforms");
#endif
        int32_t textureSlotId = 0;
        for (auto& shader_type : m_Uniforms) {
            for (auto& uni : shader_type.second) {
                if (m_UniformPreUploadFunctor != nullptr) {
                    m_UniformPreUploadFunctor(vFBOPipeLinePtr, uni.second);
                }
                if (uni.second.used) {
                    if (uni.second.datas_f != nullptr) {
                        switch (uni.second.channels) {
                            case 1U: glUniform1fv(uni.second.loc, 1, uni.second.datas_f); break;
                            case 2U: glUniform2fv(uni.second.loc, 1, uni.second.datas_f); break;
                            case 3U: glUniform3fv(uni.second.loc, 1, uni.second.datas_f); break;
                            case 4U: glUniform4fv(uni.second.loc, 1, uni.second.datas_f); break;
                        }
                        CheckGLErrors;
                    } else if (uni.second.datas_i != nullptr) {
                        switch (uni.second.channels) {
                            case 1U: glUniform1iv(uni.second.loc, 1, uni.second.datas_i); break;
                            case 2U: glUniform2iv(uni.second.loc, 1, uni.second.datas_i); break;
                            case 3U: glUniform3iv(uni.second.loc, 1, uni.second.datas_i); break;
                            case 4U: glUniform4iv(uni.second.loc, 1, uni.second.datas_i); break;
                        }
                        CheckGLErrors;
                    } else if (uni.second.data_s2d > -1) {
                        glActiveTexture(GL_TEXTURE0 + textureSlotId);
                        CheckGLErrors;
                        glBindTexture(GL_TEXTURE_2D, uni.second.data_s2d);
                        CheckGLErrors;
                        glUniform1i(uni.second.loc, textureSlotId);
                        CheckGLErrors;
                        ++textureSlotId;
                    }
                }
            }
        }
    }
#ifdef IMGUI_INCLUDE
    void drawUniformWidgets() {
        ImGui::PushID(m_ProgramAutoName.c_str());
        if (ImGui::CollapsingHeader(m_ProgramAutoName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent();
            for (auto& shader_type : m_Uniforms) {
                switch (shader_type.first) {
                    case GL_VERTEX_SHADER: ImGui::Text("%s", "Stage Vertex"); break;
                    case GL_FRAGMENT_SHADER: ImGui::Text("%s", "Stage Fragment"); break;
                    case GL_TESS_EVALUATION_SHADER: ImGui::Text("%s", "Stage Tesselation Evaluation"); break;
                    case GL_TESS_CONTROL_SHADER: ImGui::Text("%s", "Stage Tesselation Control"); break;
                }
                ImGui::Indent();
                for (auto& uni : shader_type.second) {
                    if (uni.second.showed && uni.second.used) {
                        if (uni.second.widget_functor != nullptr) {
                            uni.second.widget_functor(uni.second);
                        } else {
                            if (uni.second.datas_f != nullptr) {
                                switch (uni.second.channels) {
                                    case 1U: ImGui::DragFloat(uni.second.name.c_str(), uni.second.datas_f); break;
                                    case 2U: ImGui::DragFloat2(uni.second.name.c_str(), uni.second.datas_f); break;
                                    case 3U: ImGui::DragFloat3(uni.second.name.c_str(), uni.second.datas_f); break;
                                    case 4U: ImGui::DragFloat4(uni.second.name.c_str(), uni.second.datas_f); break;
                                }
                            } else if (uni.second.datas_i != nullptr) {
                                switch (uni.second.channels) {
                                    case 1U: ImGui::DragInt(uni.second.name.c_str(), uni.second.datas_i); break;
                                    case 2U: ImGui::DragInt2(uni.second.name.c_str(), uni.second.datas_i); break;
                                    case 3U: ImGui::DragInt3(uni.second.name.c_str(), uni.second.datas_i); break;
                                    case 4U: ImGui::DragInt4(uni.second.name.c_str(), uni.second.datas_i); break;
                                }
                            }
                        }
                    }
                }
                ImGui::Unindent();
            }
            ImGui::Unindent();
        }
        ImGui::PopID();
    }
#endif
    void locateUniforms() {
        assert(m_ProgramAutoId > 0U);
        const char* stage_name = nullptr;
        for (auto& shader_type : m_Uniforms) {
            switch (shader_type.first) {
                case GL_VERTEX_SHADER: stage_name = "VERTEX"; break;
                case GL_FRAGMENT_SHADER: stage_name = "FRAGMENT"; break;
                case GL_TESS_EVALUATION_SHADER: stage_name = "TESSEVAL"; break;
                case GL_TESS_CONTROL_SHADER: stage_name = "TESSCTRL"; break;
            }
            for (auto& uni : shader_type.second) {
                uni.second.loc = glGetUniformLocation(m_ProgramAutoId, uni.second.name.c_str());
                CheckGLErrors;
                uni.second.used = (uni.second.loc > -1);
                if (uni.second.loc == -1) {
                    LogVarInfo("ProgramAuto \'%s\' Stage \'%s\' is not using the uniform \'%s\'", m_ProgramAutoName.c_str(), stage_name, uni.second.name.c_str());
                }
            }
        }
    }
    void unit() {
        if (m_ProgramAutoId > 0U) {
            glDeleteProgram(m_ProgramAutoId);
            CheckGLErrors;
            m_ProgramAutoId = 0U;
        }
    }
    bool use() {
        if (m_ProgramAutoId > 0U) {
            glUseProgram(m_ProgramAutoId);
            CheckGLErrors;
            return true;
        }
        return false;
    }
    void unuse() {
        glUseProgram(0);
    }

private:
    bool printProgramAutoLogs(const std::string& vProgramAutoName, const std::string& vLogTypes) {
        assert(!vProgramAutoName.empty());
        assert(!vLogTypes.empty());
        if (m_ProgramAutoId > 0U) {
            GLint infoLen = 0;
            glGetProgramiv(m_ProgramAutoId, GL_INFO_LOG_LENGTH, &infoLen);
            CheckGLErrors;
            if (infoLen > 1) {
                char* infoLog = new char[infoLen];
                glGetProgramInfoLog(m_ProgramAutoId, infoLen, nullptr, infoLog);
                CheckGLErrors;
                LogVarLightInfo("#### PROGRAM %s ####", vProgramAutoName.c_str());
                LogVarLightInfo("%s : %s", vLogTypes.c_str(), infoLog);
                delete[] infoLog;
                return true;
            }
        }
        return false;
    }
};

}  // namespace gl
}  // namespace ez
