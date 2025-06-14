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

#pragma once

#include "ezGL.hpp"

#include <map>
#include <string>
#include <memory>

namespace ez {
namespace gl {

struct Infos {
    int majorGLVersion = 0;
    int minorGLVersion = 0;
    std::string driverVersion;
    std::string driverVendor;
    std::string driverRenderer;
    std::string driverGlslsVer;

    int workgroup_count[3] = {0, 0, 0};
    int workgroup_size[3] = {0, 0, 0};
    int workgroup_invocations = 0;

    int maxTextureSize = 0;
    int max3DTextureSize = 0;
    int maxCubeMapTextureSize = 0;
    int maxArrayTextureSize = 0;
    int maxColorTextureSize = 0;
    int maxDepthTextureSize = 0;
    int maxRectTextureSize = 0;
    int maxTextureBufferSize = 0;
    int maxTextureImageUnits = 0;
    int maxTextureLodBias = 0;

    int maxFragmentAtomicCounters = 0;
    int maxFragmentShaderStorageBlocks = 0;
    int maxFragmentInputComponents = 0;
    int maxFragmentUniformComponents = 0;
    int maxFragmentUniformVectors = 0;
    int maxFragmentUniformBlocks = 0;

    int maxFramebufferWidth = 0;
    int maxFramebufferHeight = 0;
    int maxFramebufferLayers = 0;
    int maxFramebufferSampler = 0;

    int maxGeometryAtomicCounters = 0;
    int maxGeometryShaderStorageBlocks = 0;
    int maxGeometryInputComponents = 0;
    int maxGeometryOutputComponents = 0;
    int maxGeometryTextureImageUnits = 0;
    int maxGeometryUniformBlocks = 0;
    int maxGeometryUniformComponent = 0;

    int maxTesselationControlAtomicCounters = 0;
    int maxTesselationControlShaderStorageBlocks = 0;
    int maxTesselationEvalAtomicCounters = 0;
    int maxTesselationEvalShaderStorageBlocks = 0;
    int maxPatchVertices = 0;

    int maxVertexAtomicCounters = 0;
    int maxVertexAttribs = 0;
    int maxVertexShaderStorageBlocks = 0;
    int maxVertexTextureImageUnits = 0;
    int maxVertexUniformComponents = 0;
    int maxVertexUniformVectors = 0;
    int maxVertexOutputComponents = 0;
    int maxVertexUniformBlocks = 0;

    int maxRenderbufferSize = 0;

    int maxShaderStorageBufferBindings = 0;

    int maxUniformBufferBindings = 0;
    int maxUniformBlockSize = 0;
    int maxUniformLocations = 0;

    int maxVaryingComponents = 0;
    int maxVaryingVectors = 0;
    int maxVaryingFloats = 0;

    int maxClipDistances = 0;

    int maxDrawBuffers = 0;
    int maxDualSourceDrawBuffers = 0;

    int maxElementsIndexs = 0;
    int maxElementsIndices = 0;
    int maxElementsVertices = 0;

    int maxViewports = 0;
    int rangeViewportBounds[2] = {0, 0};
    int maxViewportSize[2] = {0, 0};

    int maxTrasnformFeedbackInterleavedComponents = 0;
    int maxTrasnformFeedbackSeparateAttribs = 0;
    int maxTrasnformFeedbackSeparateComponents = 0;

    int maxExtentionCount = 0;
    std::map<std::string, int> extentions;

    std::string getString(GLenum vGLenum) {
        const char* s = (const char*)glGetString(vGLenum);
        if (s)
            return std::string(s);
        return "";
    }

    void fill() {
        // GL Driver Infos
        glGetIntegerv(GL_MAJOR_VERSION, &majorGLVersion);
        glGetIntegerv(GL_MINOR_VERSION, &minorGLVersion);
        driverVersion = getString(GL_VERSION);
        driverVendor = getString(GL_VENDOR);
        driverRenderer = getString(GL_RENDERER);
        driverGlslsVer = getString(GL_SHADING_LANGUAGE_VERSION);

        // compute
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workgroup_count[0]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workgroup_count[1]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workgroup_count[2]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workgroup_size[0]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workgroup_size[1]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workgroup_size[2]);
        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workgroup_invocations);

        // texture
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max3DTextureSize);
        glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxCubeMapTextureSize);
        glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxArrayTextureSize);
        glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &maxColorTextureSize);
        glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &maxDepthTextureSize);
        glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE, &maxRectTextureSize);
        glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &maxTextureBufferSize);
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureImageUnits);
        glGetIntegerv(GL_MAX_TEXTURE_LOD_BIAS, &maxTextureLodBias);

        // clipping
        glGetIntegerv(GL_MAX_CLIP_DISTANCES, &maxClipDistances);

        // buffers
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffers);
        glGetIntegerv(GL_MAX_DUAL_SOURCE_DRAW_BUFFERS, &maxDualSourceDrawBuffers);

        // elements
        glGetIntegerv(GL_MAX_ELEMENT_INDEX, &maxElementsIndexs);
        glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &maxElementsIndices);
        glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &maxElementsVertices);

        // fragment
        glGetIntegerv(GL_MAX_FRAGMENT_ATOMIC_COUNTERS, &maxFragmentAtomicCounters);
        glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, &maxFragmentShaderStorageBlocks);
        glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, &maxFragmentInputComponents);
        glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &maxFragmentUniformComponents);
        glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &maxFragmentUniformVectors);
        glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &maxFragmentUniformBlocks);

        // fbo
        glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &maxFramebufferWidth);
        glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &maxFramebufferHeight);
        glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, &maxFramebufferLayers);
        glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &maxFramebufferSampler);

        // geometry
        glGetIntegerv(GL_MAX_GEOMETRY_ATOMIC_COUNTERS, &maxGeometryAtomicCounters);
        glGetIntegerv(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS, &maxGeometryShaderStorageBlocks);
        glGetIntegerv(GL_MAX_GEOMETRY_INPUT_COMPONENTS, &maxGeometryInputComponents);
        glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, &maxGeometryOutputComponents);
        glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &maxGeometryTextureImageUnits);
        glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &maxGeometryUniformBlocks);
        glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, &maxGeometryUniformComponent);

        // tesselation
        glGetIntegerv(GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS, &maxTesselationControlAtomicCounters);
        glGetIntegerv(GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS, &maxTesselationControlShaderStorageBlocks);
        glGetIntegerv(GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS, &maxTesselationEvalAtomicCounters);
        glGetIntegerv(GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS, &maxTesselationEvalShaderStorageBlocks);
        glGetIntegerv(GL_MAX_PATCH_VERTICES, &maxPatchVertices);

        // renderbuffer
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderbufferSize);

        // shader storage
        glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxShaderStorageBufferBindings);

        // uniform
        glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxUniformBufferBindings);
        glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUniformBlockSize);
        glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &maxUniformLocations);

        // varying
        glGetIntegerv(GL_MAX_VARYING_COMPONENTS, &maxVaryingComponents);
        glGetIntegerv(GL_MAX_VARYING_VECTORS, &maxVaryingVectors);
        glGetIntegerv(GL_MAX_VARYING_FLOATS, &maxVaryingFloats);

        // vertex
        glGetIntegerv(GL_MAX_VERTEX_ATOMIC_COUNTERS, &maxVertexAtomicCounters);
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
        glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS, &maxVertexShaderStorageBlocks);
        glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxVertexTextureImageUnits);
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &maxVertexUniformComponents);
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxVertexUniformVectors);
        glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, &maxVertexOutputComponents);
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &maxVertexUniformBlocks);

        // viewports
        glGetIntegerv(GL_MAX_VIEWPORTS, &maxViewports);
        glGetIntegerv(GL_VIEWPORT_BOUNDS_RANGE, &rangeViewportBounds[0]);
        glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &maxViewportSize[0]);

        // transform feedback
        glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS, &maxTrasnformFeedbackInterleavedComponents);
        glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS, &maxTrasnformFeedbackSeparateAttribs);
        glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS, &maxTrasnformFeedbackSeparateComponents);

        // extentions
        glGetIntegerv(GL_NUM_EXTENSIONS, &maxExtentionCount);
        for (int i = 0; i < maxExtentionCount; i++) {
            auto str = glGetStringi(GL_EXTENSIONS, i);
            extentions[std::string((char*)str)] = i;
        }
    }
#ifdef IMGUI_INCLUDE
    void drawImGui() {
        if (ImGui::BeginMenu("Opengl Infos")) {
            ImGui::Text("Opengl Version %i.%i", majorGLVersion, minorGLVersion);

            if (ImGui::BeginMenu("Driver Infos")) {
                ImGui::Text("Version :      %s", driverVersion.c_str());
                ImGui::Text("Vendor :       %s", driverVendor.c_str());
                ImGui::Text("Renderer :     %s", driverRenderer.c_str());
                ImGui::Text("GLSL Version : %s", driverGlslsVer.c_str());

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Shader Infos")) {
                if (ImGui::BeginMenu("Storage Infos")) {
                    ImGui::Text("GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS : %i", maxShaderStorageBufferBindings);

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Uniform Infos")) {
                    ImGui::Text("GL_MAX_UNIFORM_BUFFER_BINDINGS : %i", maxUniformBufferBindings);
                    ImGui::Text("GL_MAX_UNIFORM_BLOCK_SIZE :      %i", maxUniformBlockSize);
                    ImGui::Text("GL_MAX_UNIFORM_LOCATIONS :       %i", maxUniformLocations);

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Varying Infos")) {
                    ImGui::Text("GL_MAX_VARYING_COMPONENTS : %i", maxVaryingComponents);
                    ImGui::Text("GL_MAX_VARYING_VECTORS :    %i", maxVaryingVectors);
                    ImGui::Text("GL_MAX_VARYING_FLOATS :     %i", maxVaryingFloats);

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Vertex Infos")) {
                    ImGui::Text("GL_MAX_VERTEX_ATOMIC_COUNTERS :       %i", maxVertexAtomicCounters);
                    ImGui::Text("GL_MAX_VERTEX_ATTRIBS :               %i", maxVertexAttribs);
                    ImGui::Text("GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS : %i", maxVertexShaderStorageBlocks);
                    ImGui::Text("GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS :   %i", maxVertexTextureImageUnits);
                    ImGui::Text("GL_MAX_VERTEX_UNIFORM_COMPONENTS :    %i", maxVertexUniformComponents);
                    ImGui::Text("GL_MAX_VERTEX_UNIFORM_VECTORS :       %i", maxVertexUniformVectors);
                    ImGui::Text("GL_MAX_VERTEX_OUTPUT_COMPONENTS :     %i", maxVertexOutputComponents);
                    ImGui::Text("GL_MAX_VERTEX_UNIFORM_BLOCKS :        %i", maxVertexUniformBlocks);

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Geometry Infos")) {
                    ImGui::Text("GL_MAX_GEOMETRY_ATOMIC_COUNTERS :       %i", maxGeometryAtomicCounters);
                    ImGui::Text("GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS : %i", maxGeometryShaderStorageBlocks);
                    ImGui::Text("GL_MAX_GEOMETRY_INPUT_COMPONENTS :      %i", maxGeometryInputComponents);
                    ImGui::Text("GL_MAX_GEOMETRY_OUTPUT_COMPONENTS :     %i", maxGeometryOutputComponents);
                    ImGui::Text("GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS :   %i", maxGeometryTextureImageUnits);
                    ImGui::Text("GL_MAX_GEOMETRY_UNIFORM_BLOCKS :        %i", maxGeometryUniformBlocks);
                    ImGui::Text("GL_MAX_GEOMETRY_UNIFORM_COMPONENTS :    %i", maxGeometryUniformComponent);

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Tesselation Infos")) {
                    ImGui::Text("Control");
                    ImGui::Text("GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS :          %i", maxTesselationControlAtomicCounters);
                    ImGui::Text("GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS :    %i", maxTesselationControlShaderStorageBlocks);
                    ImGui::Text("Evaluation");
                    ImGui::Text("GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS :       %i", maxTesselationEvalAtomicCounters);
                    ImGui::Text("GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS : %i", maxTesselationEvalShaderStorageBlocks);
                    ImGui::Text("Patches");
                    ImGui::Text("GL_MAX_PATCH_VERTICES :                        %i", maxPatchVertices);

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Fragment Infos")) {
                    ImGui::Text("GL_MAX_FRAGMENT_ATOMIC_COUNTERS :       %i", maxFragmentAtomicCounters);
                    ImGui::Text("GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS : %i", maxFragmentShaderStorageBlocks);
                    ImGui::Text("GL_MAX_FRAGMENT_INPUT_COMPONENTS :      %i", maxFragmentInputComponents);
                    ImGui::Text("GL_MAX_FRAGMENT_UNIFORM_COMPONENTS :    %i", maxFragmentUniformComponents);
                    ImGui::Text("GL_MAX_FRAGMENT_UNIFORM_VECTORS :       %i", maxFragmentUniformVectors);
                    ImGui::Text("GL_MAX_FRAGMENT_UNIFORM_BLOCKS :        %i", maxFragmentUniformBlocks);

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Compute Infos")) {
                    ImGui::Text("GL_MAX_COMPUTE_WORK_GROUP_COUNT :       x:%i y:%i z:%i", workgroup_count[0], workgroup_count[1], workgroup_count[2]);
                    ImGui::Text("GL_MAX_COMPUTE_WORK_GROUP_SIZE :        x:%i y:%i z:%i", workgroup_size[0], workgroup_size[1], workgroup_size[2]);
                    ImGui::Text("GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS : %i", workgroup_invocations);

                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Texture Infos")) {
                ImGui::Text("GL_MAX_TEXTURE_SIZE :           %i", maxTextureSize);
                ImGui::Text("GL_MAX_3D_TEXTURE_SIZE :        %i", max3DTextureSize);
                ImGui::Text("GL_MAX_CUBE_MAP_TEXTURE_SIZE :  %i", maxCubeMapTextureSize);
                ImGui::Text("GL_MAX_ARRAY_TEXTURE_LAYERS :   %i", maxArrayTextureSize);
                ImGui::Text("GL_MAX_COLOR_TEXTURE_SAMPLES :  %i", maxColorTextureSize);
                ImGui::Text("GL_MAX_DEPTH_TEXTURE_SAMPLES :  %i", maxDepthTextureSize);
                ImGui::Text("GL_MAX_RECTANGLE_TEXTURE_SIZE : %i", maxRectTextureSize);
                ImGui::Text("GL_MAX_TEXTURE_BUFFER_SIZE :    %i", maxTextureBufferSize);
                ImGui::Text("GL_MAX_TEXTURE_IMAGE_UNITS :    %i", maxTextureImageUnits);
                ImGui::Text("GL_MAX_TEXTURE_LOD_BIAS :       %i", maxTextureLodBias);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("FrameBuffer Infos")) {
                ImGui::Text("GL_MAX_FRAMEBUFFER_WIDTH :   %i", maxFramebufferWidth);
                ImGui::Text("GL_MAX_FRAMEBUFFER_HEIGHT :  %i", maxFramebufferHeight);
                ImGui::Text("GL_MAX_FRAMEBUFFER_LAYERS :  %i", maxFramebufferLayers);
                ImGui::Text("GL_MAX_FRAMEBUFFER_SAMPLES : %i", maxFramebufferSampler);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("RenderBuffer Infos")) {
                ImGui::Text("GL_MAX_RENDERBUFFER_SIZE : %i", maxRenderbufferSize);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Draw Buffers Infos")) {
                ImGui::Text("GL_MAX_DRAW_BUFFERS :             %i", maxDrawBuffers);
                ImGui::Text("GL_MAX_DUAL_SOURCE_DRAW_BUFFERS : %i", maxDualSourceDrawBuffers);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Clipping Infos")) {
                ImGui::Text("GL_MAX_CLIP_DISTANCES : %i", maxClipDistances);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Elements Infos")) {
                ImGui::Text("GL_MAX_ELEMENTS_INDEXS :  %i", maxElementsIndexs);
                ImGui::Text("GL_MAX_ELEMENTS_INDICES :  %i", maxElementsIndices);
                ImGui::Text("GL_MAX_ELEMENTS_VERTICES : %i", maxElementsVertices);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("ViewPort Infos")) {
                ImGui::Text("GL_MAX_VIEWPORTS :         %i", maxViewports);
                ImGui::Text("GL_VIEWPORT_BOUNDS_RANGE : x:%i y:%i", rangeViewportBounds[0], rangeViewportBounds[1]);
                ImGui::Text("GL_MAX_VIEWPORT_DIMS :     x:%i y:%i", maxViewportSize[0], maxViewportSize[1]);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Transform Feedback Infos")) {
                ImGui::Text("GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS : %i", maxTrasnformFeedbackInterleavedComponents);
                ImGui::Text("GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS :       %i", maxTrasnformFeedbackSeparateAttribs);
                ImGui::Text("GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS :    %i", maxTrasnformFeedbackSeparateComponents);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Extentions Infos")) {
                ImGui::Text("Extention Count : %i", maxExtentionCount);

                for (auto it = extentions.begin(); it != extentions.end(); ++it) {
                    ImGui::Text("%s", it->first.c_str());
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }
    }
#endif
};

struct Version {
    int major = 0;
    int minor = 0;
    std::string OpenglVersion;
    int DefaultGlslVersionInt = 0;
    std::string DefineCode;
    bool supported = true;
    bool attribLayoutSupportedExtention = false;
    bool geometryShaderSupported = false;
    bool attribLayoutSupportedCore = false;
    bool tesselationShaderSupported = false;
    bool computeShaderSupported = false;

    Version() {
        major = 0;
        minor = 0;
    }

    Version(int vMajor,
            int vMinor,
            const std::string& vOpenglVersion,
            int vDefaultGlslVersionInt,
            const std::string& vDefineCode,
            bool AttribLayoutSupportedByExt,
            bool vGeomShaderSupported,
            bool AttribLayoutSupportedInCore,
            bool vTessShaderSupported,
            bool vCompShaderSupported) {
        major = vMajor;
        minor = vMinor;
        OpenglVersion = vOpenglVersion;
        DefaultGlslVersionInt = vDefaultGlslVersionInt;
        DefineCode = vDefineCode;
        attribLayoutSupportedExtention = AttribLayoutSupportedByExt;
        geometryShaderSupported = vGeomShaderSupported;
        attribLayoutSupportedCore = AttribLayoutSupportedInCore;
        tesselationShaderSupported = vTessShaderSupported;
        computeShaderSupported = vCompShaderSupported;
    }
};

class Checker {
private:
    std::map<std::string, Version> m_VersionsMap;
    std::map<std::string, std::string> m_ContextParamsMap;
    std::string m_DefaultGlslVersionHeader;
    int m_DefaultGlslVersionInt;
    std::string m_Version;

public:  // extention
    bool m_GeometryShaderSupported = false;
    bool m_TesselationShaderSupported = false;
    bool m_ComputeShaderSupported = false;
    bool m_AttribLayoutSupportedCore = false;
    bool m_AttribLayoutSupportedExtention = false;

public:
    Infos m_Infos;

public:
    static Checker* Instance() {
        static auto Instance = std::unique_ptr<Checker>(new Checker());
        return Instance.get();
    }

public:
    Checker() {
        m_initSupportedVars();
        m_fillOpenglVersionMap();
    }                         
    Checker(const Checker&) = delete;
    Checker& operator=(const Checker&) = delete;
    ~Checker() = default;

public:
    std::string getVersion() {
        return m_Version;
    }
    bool getVersion(const std::string& vVersion, Version& vOutVersion) {
        if (m_VersionsMap.find(vVersion) != m_VersionsMap.end()) {
            vOutVersion = m_VersionsMap[vVersion];
            return true;
        }
        return false;
    }
    bool getVersion(Version& vOutVersion) {
        return getVersion(getVersion(), vOutVersion);
    }
    const std::map<std::string, Version>& getVersionMap() {
        return m_VersionsMap;
    }
    int getGlslVersionInt() {
        return m_DefaultGlslVersionInt;
    }
    std::string getGlslVersionHeader() {
        return m_DefaultGlslVersionHeader;
    }
    bool checkVersions() {
        if (!m_checkVersion(4, 5))
            if (!m_checkVersion(4, 4))
                if (!m_checkVersion(4, 3))
                    if (!m_checkVersion(4, 2))
                        if (!m_checkVersion(4, 1))
                            if (!m_checkVersion(4, 0))
                                if (!m_checkVersion(3, 3))
                                    if (!m_checkVersion(3, 2))
                                        if (!m_checkVersion(3, 1))
                                            if (!m_checkVersion(3, 0))
                                                if (!m_checkVersion(2, 1))
                                                    m_checkVersion(2, 0);

        m_Infos.fill();
        return true;
    }
    void printSupport() {
        Version version;
        if (getVersion(m_Version, version)) {
            LogVarLightInfo("OpenGl version : %i.%i", version.major, version.minor);
            if (m_AttribLayoutSupportedCore)
                LogVarLightInfo("%s", "- Attrib Location Available in Core");
            else if (m_AttribLayoutSupportedExtention)
                LogVarLightInfo("%s", "- Attrib Location Available in Extension");
            else
                LogVarLightInfo("%s", "- Attrib Location Not Available");
            if (m_GeometryShaderSupported)
                LogVarLightInfo("%s", "- Geometry Stage Available");
            else
                LogVarLightInfo("%s", "- Geometry Stage Not Available");
            if (m_TesselationShaderSupported)
                LogVarLightInfo("%s", "- Tesselation Stage Available");
            else
                LogVarLightInfo("%s", "- Tesselation Stage Not Available");
            if (m_ComputeShaderSupported)
                LogVarLightInfo("%s", "- Compute Stage Available");
            else
                LogVarLightInfo("%s", "- Compute Stage Not Available");
        } else {
            LogVarLightError("%s", "OpenGl version : Not Found !");
        }
    }

private:
    void m_initSupportedVars() {
        m_GeometryShaderSupported = false;
        m_TesselationShaderSupported = false;
        m_ComputeShaderSupported = false;
        m_AttribLayoutSupportedCore = false;
        m_AttribLayoutSupportedExtention = false;
    }
    void m_fillOpenglVersionMap() {
        // https://en.wikipedia.org/wiki/OpenGL_Shading_Language
        /*
        2.0 es #version 100
        3.0 es #version 300 es
        1.10.59[1] 	2.0 	April 2004 		#version 110
        1.20.8[2] 	2.1 	September 2006 	#version 120
        1.30.10[3] 	3.0 	August 2008 	#version 130
        1.40.08[4] 	3.1 	March 2009 		#version 140
        1.50.11[5] 	3.2 	August 2009 	#version 150
        3.30.6[6] 	3.3 	February 2010 	#version 330
        4.00.9[7] 	4.0 	March 2010 		#version 400
        4.10.6[8] 	4.1 	July 2010 		#version 410
        4.20.11[9] 	4.2 	August 2011 	#version 420
        4.30.8[10] 	4.3 	August 2012 	#version 430
        4.40[11] 	4.4 	July 2013 		#version 440
        4.50[12] 	4.5 	August 2014 	#version 450
        */
        m_VersionsMap["2.0 ES"] = Version(2, -1, "2.0 ES", 100, "#version 100", false, false, false, false, false);
        m_VersionsMap["2.0"] = Version(2, 0, "2.0", 110, "#version 110", true, false, false, false, false);
        m_VersionsMap["2.1"] = Version(2, 1, "2.1", 120, "#version 120", true, false, false, false, false);
        m_VersionsMap["3.0 ES"] = Version(3, -1, "3.0 ES", 300, "#version 300 es", true, false, false, false, false);
        m_VersionsMap["3.0"] = Version(3, 0, "3.0", 130, "#version 130", true, false, false, false, false);
        m_VersionsMap["3.1"] = Version(3, 1, "3.1", 140, "#version 140", true, false, false, false, false);
        m_VersionsMap["3.2"] = Version(3, 2, "3.2", 150, "#version 150", true, true, false, false, false);
        m_VersionsMap["3.3"] = Version(3, 3, "3.3", 330, "#version 330", false, true, true, false, false);
        m_VersionsMap["4.0"] = Version(4, 0, "4.0", 400, "#version 400", false, true, true, true, false);
        m_VersionsMap["4.1"] = Version(4, 1, "4.1", 410, "#version 410", false, true, true, false, false);
        m_VersionsMap["4.2"] = Version(4, 2, "4.2", 420, "#version 420", false, true, true, false, false);
        m_VersionsMap["4.3"] = Version(4, 3, "4.3", 430, "#version 430", false, true, true, true, true);
        m_VersionsMap["4.4"] = Version(4, 4, "4.4", 440, "#version 440", false, true, true, true, true);
        m_VersionsMap["4.5"] = Version(4, 5, "4.5", 450, "#version 450", false, true, true, true, true);
    }
    bool m_isGlSupported(int vMajorGLVersion, int MinorGLVersion) {
#ifdef __glad_h_
        if (GLVersion.major < 3)
            return false;
        if (GLVersion.major == vMajorGLVersion)
            return GLVersion.minor >= MinorGLVersion;
        return GLVersion.major >= vMajorGLVersion;
#endif // __glad_h_
    }
    bool m_checkVersion(int vMajorGLVersion, int MinorGLVersion) {
        Version version;
        bool is_supported_version = m_isGlSupported(vMajorGLVersion, MinorGLVersion);
        const auto& tmp_version = str::toStr(vMajorGLVersion) + "." + str::toStr(MinorGLVersion);
        if (getVersion(tmp_version, version)) {
            if (is_supported_version) {
                m_Version = tmp_version;
            }
            version.supported = is_supported_version;
            m_DefaultGlslVersionInt = version.DefaultGlslVersionInt;
            m_DefaultGlslVersionHeader = version.DefineCode;
            m_AttribLayoutSupportedExtention = version.attribLayoutSupportedExtention;
            m_AttribLayoutSupportedCore = version.attribLayoutSupportedCore;
            m_GeometryShaderSupported = version.geometryShaderSupported;
            m_TesselationShaderSupported = version.tesselationShaderSupported;
            m_ComputeShaderSupported = version.computeShaderSupported;
        }
        return is_supported_version;
    }
};

}  // namespace gl
}  // namespace ez