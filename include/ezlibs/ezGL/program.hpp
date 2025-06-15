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

namespace ez {
namespace gl {

class Program;
typedef std::shared_ptr<Program> ProgramPtr;
typedef std::weak_ptr<Program> ProgramWeak;

class Program {
public:
    struct Uniform;
    typedef std::map<GLenum, std::map<std::string, Uniform>> UniformPerShaderTypeContainer;
    typedef std::function<void(FBOPipeLinePtr, Uniform&)> UniformPreUploadFunctor;
    typedef std::function<void(Uniform&)> UniformWidgetFunctor;
    struct Uniform {
        std::string name;
        float* datas_f = nullptr;  // float
        int32_t* datas_i = nullptr;  // int
        uint32_t* datas_u = nullptr;  // uint
        bool* datas_b = nullptr;  // bool
        int32_t data_s2d = -1;  // sampler2D
        int32_t matrix_size = 0;  // matrixSize 2,3,4
        GLint loc = -1;
        GLuint channels = 0U;
        GLuint elements = 0U;
        bool canbeDirty = false; // to uplaod when needed and not each frames
        bool dirty = false; // need a new upload
        bool used = false;
        bool showed = false;
        BufferBlock* buffer_ptr = nullptr;  // a buffer block ex: UBO /SSBO
        int32_t bufferBinding = -1;  // the binding point in the sahder of the buffer block
        UniformWidgetFunctor widget_functor = nullptr;
    };

private:
    ProgramWeak m_This;
    GLuint m_ProgramId = 0U;
    std::string m_ProgramName;
    std::map<uintptr_t, ShaderWeak> m_Shaders;  // a same shader object can be added two times
    UniformPerShaderTypeContainer m_Uniforms;
    UniformPreUploadFunctor m_UniformPreUploadFunctor = nullptr;  // lanbda to execute just before the uniform upload

public:
    static ProgramPtr create(const std::string& vProgramName) {
        auto res = std::make_shared<Program>();
        res->m_This = res;
        if (!res->init(vProgramName)) {
            res.reset();
        }
        return res;
    }

public:
    Program() = default;
    ~Program() { unit(); }
    bool init(const std::string& vProgramName) {
        assert(!vProgramName.empty());
        m_ProgramName = vProgramName;
        m_ProgramId = glCreateProgram();
        CheckGLErrors;
        if (m_ProgramId > 0U) {
            return true;
        }
        return false;
    }
    bool addShader(ShaderWeak vShader) {
        if (!vShader.expired()) {
            m_Shaders[(uintptr_t)vShader.lock().get()] = vShader;
            return true;
        }
        return false;
    }
    bool link() {
        bool res = false;
        if (m_ProgramId > 0U) {
            bool one_shader_at_least = false;
            for (auto& shader : m_Shaders) {
                auto ptr = shader.second.lock();
                if (ptr != nullptr) {
                    one_shader_at_least = true;
                    glAttachShader(m_ProgramId, ptr->getShaderId());
                    CheckGLErrors;
                    // we could delete shader id after linking,
                    // but we dont since we can have many shader for the same program
                }
            }
            if (one_shader_at_least) {
                glLinkProgram(m_ProgramId);
                CheckGLErrors;
                glFinish();
                GLint linked = 0;
                glGetProgramiv(m_ProgramId, GL_LINK_STATUS, &linked);
                CheckGLErrors;
                if (!linked) {
                    if (!printProgramLogs(m_ProgramName, "Link Errors")) {
                        LogVarError("Program \"%s\" linking fail for unknown reason", m_ProgramName.c_str());
                    }
                    res = false;
                } else {
                    printProgramLogs(m_ProgramName, "Link Warnings");
                    res = true;
                }
            }
        }
        return res;
    }
    const char* getLabelName() { return m_ProgramName.c_str(); }
    void setUniformPreUploadFunctor(UniformPreUploadFunctor vUniformPreUploadFunctor) { m_UniformPreUploadFunctor = vUniformPreUploadFunctor; }
    void addBufferBlock(const GLenum vShaderType, const std::string& vBufferName, const int32_t vBinding, BufferBlock* vBufferPtr) {
        assert(vShaderType > 0);
        assert(!vBufferName.empty());
        assert(vBinding > -1);
        assert(vBufferPtr != nullptr);
        Uniform uni;
        uni.name = vBufferName;
        uni.bufferBinding = vBinding;
        uni.buffer_ptr = vBufferPtr;
        m_Uniforms[vShaderType][vBufferName] = uni;
    }
    void addUniformFloat(
        const GLenum vShaderType,
        const std::string& vUniformName,
        float* vUniformPtr,
        const GLuint vCountChannels,
        const GLuint vCountElements,
        const bool vShowWidget,
        const UniformWidgetFunctor& vWidgetFunctor) {
        assert(vShaderType > 0);
        assert(!vUniformName.empty());
        assert(vUniformPtr != nullptr);
        assert(vCountChannels > 0U);
        assert(vCountElements > 0U);
        Uniform uni;
        uni.name = vUniformName;
        uni.datas_f = vUniformPtr;
        uni.showed = vShowWidget;
        uni.channels = vCountChannels;
        uni.elements = vCountElements;
        uni.widget_functor = vWidgetFunctor;
        m_Uniforms[vShaderType][vUniformName] = uni;
    }
    void setUniformFloatDatas(const GLenum vShaderType, const std::string& vUniformName, float* vUniformPtr) {
        auto itShaderType = m_Uniforms.find(vShaderType);
        assert(itShaderType != m_Uniforms.end());
        auto itUniformName = itShaderType->second.find(vUniformName);
        assert(itUniformName != itShaderType->second.end());
        itUniformName->second.datas_f = vUniformPtr;
    }
    void addUniformMatrix(
        const GLenum vShaderType,
        const std::string& vUniformName,
        float* vUniformPtr,
        const int32_t vMatrixSize,
        const GLuint vCountElements,
        const bool vShowWidget,
        const UniformWidgetFunctor& vWidgetFunctor) {
        assert(vShaderType > 0);
        assert(!vUniformName.empty());
        assert(vUniformPtr != nullptr);
        assert((vMatrixSize == 2U) || (vMatrixSize == 3U) || (vMatrixSize == 4U));
        assert(vCountElements > 0U);
        Uniform uni;
        uni.name = vUniformName;
        uni.datas_f = vUniformPtr;
        uni.showed = vShowWidget;
        uni.matrix_size = vMatrixSize;
        uni.elements = vCountElements;
        uni.widget_functor = vWidgetFunctor;
        m_Uniforms[vShaderType][vUniformName] = uni;
    }
    void setUniformMatrixDatas(const GLenum vShaderType, const std::string& vUniformName, float* vUniformPtr) {
        auto itShaderType = m_Uniforms.find(vShaderType);
        assert(itShaderType != m_Uniforms.end());
        auto itUniformName = itShaderType->second.find(vUniformName);
        assert(itUniformName != itShaderType->second.end());
        itUniformName->second.datas_f = vUniformPtr;
    }
    void addUniformInt(
        const GLenum vShaderType,
        const std::string& vUniformName,
        int32_t* vUniformPtr,
        const GLuint vCountChannels,
        const GLuint vCountElements,
        const bool vShowWidget,
        const UniformWidgetFunctor& vWidgetFunctor) {
        assert(vShaderType > 0);
        assert(!vUniformName.empty());
        assert(vUniformPtr != nullptr);
        assert(vCountChannels > 0U);
        assert(vCountElements > 0U);
        Uniform uni;
        uni.name = vUniformName;
        uni.datas_i = vUniformPtr;
        uni.showed = vShowWidget;
        uni.channels = vCountChannels;
        uni.elements = vCountElements;
        uni.widget_functor = vWidgetFunctor;
        m_Uniforms[vShaderType][vUniformName] = uni;
    }
    void setUniformIntDatas(const GLenum vShaderType, const std::string& vUniformName, int32_t* vUniformPtr) {
        auto itShaderType = m_Uniforms.find(vShaderType);
        assert(itShaderType != m_Uniforms.end());
        auto itUniformName = itShaderType->second.find(vUniformName);
        assert(itUniformName != itShaderType->second.end());
        itUniformName->second.datas_i = vUniformPtr;
    }
    void addUniformUInt(
        const GLenum vShaderType,
        const std::string& vUniformName,
        uint32_t* vUniformPtr,
        const GLuint vCountChannels,
        const GLuint vCountElements,
        const bool vShowWidget,
        const UniformWidgetFunctor& vWidgetFunctor) {
        assert(vShaderType > 0);
        assert(!vUniformName.empty());
        assert(vUniformPtr != nullptr);
        assert(vCountChannels > 0U);
        assert(vCountElements > 0U);
        Uniform uni;
        uni.name = vUniformName;
        uni.datas_u = vUniformPtr;
        uni.showed = vShowWidget;
        uni.channels = vCountChannels;
        uni.elements = vCountElements;
        uni.widget_functor = vWidgetFunctor;
        m_Uniforms[vShaderType][vUniformName] = uni;
    }
    void setUniformUIntDatas(const GLenum vShaderType, const std::string& vUniformName, uint32_t* vUniformPtr) {
        auto itShaderType = m_Uniforms.find(vShaderType);
        assert(itShaderType != m_Uniforms.end());
        auto itUniformName = itShaderType->second.find(vUniformName);
        assert(itUniformName != itShaderType->second.end());
        itUniformName->second.datas_u = vUniformPtr;
    }
    void addUniformBool(
        const GLenum vShaderType,
        const std::string& vUniformName,
        bool* vUniformPtr,
        const GLuint vCountChannels,
        const GLuint vCountElements,
        const bool vShowWidget,
        const UniformWidgetFunctor& vWidgetFunctor) {
        assert(vShaderType > 0);
        assert(!vUniformName.empty());
        assert(vUniformPtr != nullptr);
        assert(vCountChannels > 0U);
        assert(vCountElements > 0U);
        Uniform uni;
        uni.name = vUniformName;
        uni.datas_b = vUniformPtr;
        uni.showed = vShowWidget;
        uni.channels = vCountChannels;
        uni.elements = vCountElements;
        uni.widget_functor = vWidgetFunctor;
        m_Uniforms[vShaderType][vUniformName] = uni;
    }
    void setUniformBoolDatas(const GLenum vShaderType, const std::string& vUniformName, bool* vUniformPtr) {
        auto itShaderType = m_Uniforms.find(vShaderType);
        assert(itShaderType != m_Uniforms.end());
        auto itUniformName = itShaderType->second.find(vUniformName);
        assert(itUniformName != itShaderType->second.end());
        itUniformName->second.datas_b = vUniformPtr;
    }
    void addUniformSampler2D(const GLenum vShaderType, const std::string& vUniformName, int32_t vSampler2D, const bool vShowWidget) {
        assert(vShaderType > 0);
        assert(!vUniformName.empty());
        // assert(vSampler2D != -1);, if the sampler must point on a buffer after, its normal to have it at -1
        Uniform uni;
        uni.name = vUniformName;
        uni.data_s2d = vSampler2D;
        uni.channels = 0;
        uni.showed = vShowWidget;
        m_Uniforms[vShaderType][vUniformName] = uni;
    }
    void uploadUniforms(FBOPipeLinePtr vFBOPipeLinePtr) {
#ifdef PROFILER_SCOPED
        PROFILER_SCOPED(m_ProgramName, "uploadUniforms");
#endif
        int32_t textureSlotId = 0;
        for (auto& shader_type : m_Uniforms) {
            for (auto& uni : shader_type.second) {
                if (m_UniformPreUploadFunctor != nullptr) {
                    m_UniformPreUploadFunctor(vFBOPipeLinePtr, uni.second);
                }
                if (uni.second.used) {
#ifdef PROFILER_SCOPED_PTR
                    auto name_c_str = uni.second.name.c_str();  // remove some warnings
#endif
                    if (uni.second.datas_f != nullptr) {
                        switch (uni.second.channels) {
                            case 1U: {
#ifdef PROFILER_SCOPED_PTR
                                PROFILER_SCOPED_PTR(&uni, "upload float", "%s", name_c_str);
#endif
                                glUniform1fv(uni.second.loc, uni.second.elements, uni.second.datas_f);
                            } break;
                            case 2U: {
#ifdef PROFILER_SCOPED_PTR
                                PROFILER_SCOPED_PTR(&uni, "upload vec2", "%s", name_c_str);
#endif
                                glUniform2fv(uni.second.loc, uni.second.elements, uni.second.datas_f);
                            } break;
                            case 3U: {
#ifdef PROFILER_SCOPED_PTR
                                PROFILER_SCOPED_PTR(&uni, "upload vec3", "%s", name_c_str);
#endif
                                glUniform3fv(uni.second.loc, uni.second.elements, uni.second.datas_f);
                            } break;
                            case 4U: {
#ifdef PROFILER_SCOPED_PTR
                                PROFILER_SCOPED_PTR(&uni, "upload vec4", "%s", name_c_str);
#endif
                                glUniform4fv(uni.second.loc, uni.second.elements, uni.second.datas_f);
                            } break;
                        }
                        switch (uni.second.matrix_size) {
                            case 2U: {
#ifdef PROFILER_SCOPED_PTR
                                PROFILER_SCOPED_PTR(&uni, "upload mat2", "%s", name_c_str);
#endif
                                glUniformMatrix2fv(uni.second.loc, uni.second.elements, GL_FALSE, uni.second.datas_f);
                            } break;
                            case 3U: {
#ifdef PROFILER_SCOPED_PTR
                                PROFILER_SCOPED_PTR(&uni, "upload mat3", "%s", name_c_str);
#endif
                                glUniformMatrix3fv(uni.second.loc, uni.second.elements, GL_FALSE, uni.second.datas_f);
                            } break;
                            case 4U: {
#ifdef PROFILER_SCOPED_PTR
                                PROFILER_SCOPED_PTR(&uni, "upload mat4", "%s", name_c_str);
#endif
                                glUniformMatrix4fv(uni.second.loc, uni.second.elements, GL_FALSE, uni.second.datas_f);
                            } break;
                        }
                        CheckGLErrors;
                    } else if (uni.second.datas_i != nullptr) {
                        switch (uni.second.channels) {
                            case 1U: {
#ifdef PROFILER_SCOPED_PTR
                                PROFILER_SCOPED_PTR(&uni, "upload int", "%s", name_c_str);
#endif
                                glUniform1iv(uni.second.loc, uni.second.elements, uni.second.datas_i);
                            } break;
                            case 2U: {
#ifdef PROFILER_SCOPED_PTR
                                PROFILER_SCOPED_PTR(&uni, "upload iec2", "%s", name_c_str);
#endif
                                glUniform2iv(uni.second.loc, uni.second.elements, uni.second.datas_i);
                            } break;
                            case 3U: {
#ifdef PROFILER_SCOPED_PTR
                                PROFILER_SCOPED_PTR(&uni, "upload ivec3", "%s", name_c_str);
#endif
                                glUniform3iv(uni.second.loc, uni.second.elements, uni.second.datas_i);
                            } break;
                            case 4U: {
#ifdef PROFILER_SCOPED_PTR
                                PROFILER_SCOPED_PTR(&uni, "upload ivec4", "%s", name_c_str);
#endif
                                glUniform4iv(uni.second.loc, uni.second.elements, uni.second.datas_i);
                            } break;
                        }
                        CheckGLErrors;
                    } else if (uni.second.datas_u != nullptr) {
                        switch (uni.second.channels) {
                            case 1U: {
#ifdef PROFILER_SCOPED_PTR
                                PROFILER_SCOPED_PTR(&uni, "upload uint", "%s", name_c_str);
#endif
                                glUniform1uiv(uni.second.loc, uni.second.elements, uni.second.datas_u);
                            } break;
                            case 2U: {
#ifdef PROFILER_SCOPED_PTR
                                PROFILER_SCOPED_PTR(&uni, "upload uvec2", "%s", name_c_str);
#endif
                                glUniform2uiv(uni.second.loc, uni.second.elements, uni.second.datas_u);
                            } break;
                            case 3U: {
#ifdef PROFILER_SCOPED_PTR
                                PROFILER_SCOPED_PTR(&uni, "upload uvec3", "%s", name_c_str);
#endif
                                glUniform3uiv(uni.second.loc, uni.second.elements, uni.second.datas_u);
                            } break;
                            case 4U: {
#ifdef PROFILER_SCOPED_PTR
                                PROFILER_SCOPED_PTR(&uni, "upload uvec4", "%s", name_c_str);
#endif
                                glUniform4uiv(uni.second.loc, uni.second.elements, uni.second.datas_u);
                            } break;
                        }
                        CheckGLErrors;
                    } else if (uni.second.data_s2d > -1) {
#ifdef PROFILER_SCOPED_PTR
                        PROFILER_SCOPED_PTR(&uni, "upload sampler2D", "%s", name_c_str);
#endif
                        glActiveTexture(GL_TEXTURE0 + textureSlotId);
                        CheckGLErrors;
                        glBindTexture(GL_TEXTURE_2D, uni.second.data_s2d);
                        CheckGLErrors;
                        glUniform1i(uni.second.loc, textureSlotId);
                        CheckGLErrors;
                        ++textureSlotId;
                    }
                }
                // buffer have no widgets, and no use infos
                if (uni.second.bufferBinding > -1 && uni.second.buffer_ptr != nullptr && uni.second.buffer_ptr->id() > 0U) {
                    uni.second.buffer_ptr->bind(uni.second.bufferBinding);
                }
            }
        }
    }
#ifdef IMGUI_INCLUDE
    void drawUniformWidgets() {
        ImGui::PushID(m_ProgramName.c_str());
        if (ImGui::CollapsingHeader(m_ProgramName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
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
                                for (GLuint i = 0; i < uni.second.elements; ++i) {
                                    switch (uni.second.channels) {
                                        case 1U: ImGui::DragFloat(uni.second.name.c_str(), uni.second.datas_f + i); break;
                                        case 2U: ImGui::DragFloat2(uni.second.name.c_str(), uni.second.datas_f + i); break;
                                        case 3U: ImGui::DragFloat3(uni.second.name.c_str(), uni.second.datas_f + i); break;
                                        case 4U: ImGui::DragFloat4(uni.second.name.c_str(), uni.second.datas_f + i); break;
                                    }
                                }
                            } else if (uni.second.datas_i != nullptr) {
                                for (GLuint i = 0; i < uni.second.elements; ++i) {
                                    switch (uni.second.channels) {
                                        case 1U: ImGui::DragInt(uni.second.name.c_str(), uni.second.datas_i + i); break;
                                        case 2U: ImGui::DragInt2(uni.second.name.c_str(), uni.second.datas_i + i); break;
                                        case 3U: ImGui::DragInt3(uni.second.name.c_str(), uni.second.datas_i + i); break;
                                        case 4U: ImGui::DragInt4(uni.second.name.c_str(), uni.second.datas_i + i); break;
                                    }
                                }
                            } else if (uni.second.data_s2d != 0U) {
                                ImGui::Text(uni.second.name.c_str());
                                ImGui::Indent();
                                ImGui::Image((ImTextureID)uni.second.data_s2d, ImVec2(64.0f, 64.0f));
                                ImGui::Unindent();
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

    UniformPerShaderTypeContainer getUniforms() const { return m_Uniforms; }
    UniformPerShaderTypeContainer& getUniformsRef() { return m_Uniforms; }

    void locateUniforms() {
        assert(m_ProgramId > 0U);
        const char* stage_name = nullptr;
        for (auto& shader_type : m_Uniforms) {
            switch (shader_type.first) {
                case GL_VERTEX_SHADER: stage_name = "VERTEX"; break;
                case GL_FRAGMENT_SHADER: stage_name = "FRAGMENT"; break;
                case GL_TESS_EVALUATION_SHADER: stage_name = "TESSEVAL"; break;
                case GL_TESS_CONTROL_SHADER: stage_name = "TESSCTRL"; break;
            }
            for (auto& uni : shader_type.second) {
                uni.second.loc = glGetUniformLocation(m_ProgramId, uni.second.name.c_str());
                CheckGLErrors;
                uni.second.used = (uni.second.loc > -1);
                if (uni.second.loc == -1) {
                    LogVarInfo("Program \'%s\' Stage \'%s\' is not using the uniform \'%s\'", m_ProgramName.c_str(), stage_name, uni.second.name.c_str());
                }
            }
        }
    }
    void unit() {
        if (m_ProgramId > 0U) {
            glDeleteProgram(m_ProgramId);
            CheckGLErrors;
            m_ProgramId = 0U;
        }
    }
    bool use() {
        if (m_ProgramId > 0U) {
            glUseProgram(m_ProgramId);
            CheckGLErrors;
            return true;
        }
        return false;
    }
    void unuse() { glUseProgram(0); }

private:
    bool printProgramLogs(const std::string& vProgramName, const std::string& vLogTypes) {
        assert(!vProgramName.empty());
        assert(!vLogTypes.empty());
        if (m_ProgramId > 0U) {
            GLint infoLen = 0;
            glGetProgramiv(m_ProgramId, GL_INFO_LOG_LENGTH, &infoLen);
            CheckGLErrors;
            if (infoLen > 1) {
                char* infoLog = new char[infoLen];
                glGetProgramInfoLog(m_ProgramId, infoLen, nullptr, infoLog);
                CheckGLErrors;
                LogVarLightInfo("#### PROGRAM %s ####", vProgramName.c_str());
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
