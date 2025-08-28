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

#include <vector>
#include <memory>

namespace ez {
namespace gl {

template <typename T>
class Mesh {
private:
    std::weak_ptr<Mesh<T>> m_This;
    GLuint m_VboId = 0U;
    GLuint m_IboId = 0U;
    GLuint m_VaoId = 0U;

    std::vector<T> m_Vertices;
    std::vector<uint32_t> m_VerticesFormat;

    std::vector<uint32_t> m_Indices;

    // MultiDrawwArrays
    std::vector<GLint> m_IndicesOffsets;
    std::vector<GLsizei> m_IndicesCounts;
    GLsizei m_ArraysCount{};

    bool m_needNewMeshUpload = false;

    const GLsizei m_VerticeSize = sizeof(T);
    const GLsizei m_IndiceSize = sizeof(uint32_t);

public:
    static std::shared_ptr<Mesh<T>> createStaticDraw(
        std::vector<T> vVertices,
        std::vector<uint32_t> vVerticesFormat,
        std::vector<uint32_t> vIndices = {},
        std::vector<GLint> vIndicesOffsets = {},
        std::vector<GLsizei> vIndicesCounts = {},
        GLsizei vArraysCount = 0) {
        auto pRet = std::make_shared<Mesh<T>>();
        pRet->m_This = pRet;
        if (!pRet->init(vVertices, vVerticesFormat, vIndices, vIndicesOffsets, vIndicesCounts, vArraysCount, true)) {
            pRet.reset();
        }
        return pRet;
    }
    static std::shared_ptr<Mesh<T>> createDynamicDraw(
        std::vector<T> vVertices,
        std::vector<uint32_t> vVerticesFormat,
        std::vector<uint32_t> vIndices = {},
        std::vector<GLint> vIndicesOffsets = {},
        std::vector<GLsizei> vIndicesCounts = {},
        GLsizei vArraysCount = 0) {
        auto pRet = std::make_shared<Mesh<T>>();
        pRet->m_This = pRet;
        if (!pRet->init(vVertices, vVerticesFormat, vIndices, vIndicesOffsets, vIndicesCounts, vArraysCount, false)) {
            pRet.reset();
        }
        return pRet;
    }

public:
    Mesh() = default;
    virtual ~Mesh() { unit(); }
    uint32_t GetVaoID() { return m_VboId; }
    uint32_t GetVboID() { return m_IboId; }
    uint32_t GetIboID() { return m_VaoId; }
    bool init(
        std::vector<T> vVertices,
        std::vector<uint32_t> vVerticesFormat,
        std::vector<uint32_t> vIndices,
        std::vector<GLint> vIndicesOffsets,
        std::vector<GLsizei> vIndicesCounts,
        GLsizei vArraysCount,
        bool vIsStaticDraw) {
        assert(!vVertices.empty());
        assert(!vVerticesFormat.empty());
        m_Vertices = vVertices;
        m_VerticesFormat = vVerticesFormat;
        m_Indices = vIndices;
        m_IndicesOffsets = vIndicesOffsets;
        m_IndicesCounts = vIndicesCounts;
        m_ArraysCount = vArraysCount;
        glGenVertexArrays(1, &m_VaoId);
        CheckGLErrors;
        glGenBuffers(1, &m_VboId);
        CheckGLErrors;
        if (!m_Indices.empty()) {
            glGenBuffers(1, &m_IboId);
            CheckGLErrors;
        }

        // bind
        glBindVertexArray(m_VaoId);
        CheckGLErrors;
        glBindBuffer(GL_ARRAY_BUFFER, m_VboId);
        CheckGLErrors;
        glBufferData(GL_ARRAY_BUFFER, m_VerticeSize * m_Vertices.size(), m_Vertices.data(), vIsStaticDraw ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
        CheckGLErrors;

        // vertices
        GLuint idx = 0U;
        GLuint offset = 0U;
        for (const auto& format : m_VerticesFormat) {
            assert(format > 0U);
            glEnableVertexAttribArray(idx);
            CheckGLErrors;
            if (idx == 0U) {
                glVertexAttribPointer(idx, format, GL_FLOAT, GL_FALSE, m_VerticeSize, (void*)nullptr);
            } else {
                glVertexAttribPointer(idx, format, GL_FLOAT, GL_FALSE, m_VerticeSize, (void*)(sizeof(float) * offset));
            }
            CheckGLErrors;
            glDisableVertexAttribArray(idx);
            CheckGLErrors;
            offset += format;
            ++idx;
        }

        // indices
        if (!m_Indices.empty()) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IboId);
            CheckGLErrors;
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndiceSize * m_Indices.size(), m_Indices.data(), GL_STATIC_DRAW);
            CheckGLErrors;
        }

        // unbind
        glBindVertexArray(0);
        CheckGLErrors;
        // if i not unbind the VBOs and IBOs after the unbind of the VAO, it seem the VAO is corrupted..
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        CheckGLErrors;
        if (!m_Indices.empty()) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            CheckGLErrors;
        }

        return (glIsVertexArray(m_VaoId) == GL_TRUE);
    }

    bool bind() {
#ifdef PROFILER_SCOPED
        PROFILER_SCOPED("Mesh", "bin");
#endif
        if (glIsVertexArray(m_VaoId) == GL_TRUE) {
            CheckGLErrors;
            glBindVertexArray(m_VaoId);
            CheckGLErrors;
            for (size_t idx = 0U; idx < m_VerticesFormat.size(); ++idx) {
                glEnableVertexAttribArray((GLuint)idx);
                CheckGLErrors;
            }
            return true;
        }
        return false;
    }

    void unbind() {
#ifdef PROFILER_SCOPED
        PROFILER_SCOPED("Mesh", "bin");
#endif
        for (size_t idx = 0U; idx < m_VerticesFormat.size(); ++idx) {
            glDisableVertexAttribArray((GLuint)idx);
            CheckGLErrors;
        }
        glBindVertexArray(0);
    }

    void render(GLenum vRenderMode, const GLsizei vVerticesIdx = 0, const GLsizei vIndicesIdx = 0, const GLsizei vInstanceCount = 0) {
#ifdef PROFILER_SCOPED
        PROFILER_SCOPED("Mesh", "render");
#endif
        m_uploadMeshDatasIfNeeded(vVerticesIdx, vIndicesIdx);
        if (bind()) {
            m_render(vRenderMode, vIndicesIdx, vInstanceCount);
            unbind();
        }
    }

    void unit() {
        if (m_VaoId > 0) {
            glDeleteVertexArrays(1, &m_VaoId);
            CheckGLErrors;
            m_VaoId = 0U;
        }
        if (m_VboId > 0) {
            glDeleteBuffers(1, &m_VboId);
            CheckGLErrors;
            m_VboId = 0U;
        }
        if (m_IboId > 0) {
            glDeleteBuffers(1, &m_IboId);
            CheckGLErrors;
            m_IboId = 0U;
        }
    }

    std::vector<T> getVertices() const { return m_Vertices; }

    std::vector<T>& getVerticesRef() { return m_Vertices; }

    std::vector<uint32_t> getIndices() const { return m_Indices; }

    std::vector<uint32_t>& getIndicesRef() { return m_Indices; }

    void needNewUpload() { m_needNewMeshUpload = true; }

private:
    void m_render(GLenum vRenderMode, const GLsizei vIndicesIdx = 0, const GLsizei vInstanceCount = 0) {
        if (!m_Indices.empty()) {
            if (vInstanceCount > 0) {
#ifdef PROFILER_SCOPED
                PROFILER_SCOPED("Opengl", "glDrawElementsInstanced");
#endif
                if (vIndicesIdx == 0) {
                    glDrawElementsInstanced(vRenderMode, (GLsizei)m_Indices.size(), GL_UNSIGNED_INT, nullptr, vInstanceCount);
                } else {
                    glDrawElementsInstanced(vRenderMode, vIndicesIdx, GL_UNSIGNED_INT, nullptr, vInstanceCount);
                }
            } else {
#ifdef PROFILER_SCOPED
                PROFILER_SCOPED("Opengl", "glDrawElements");
#endif
                if (vIndicesIdx == 0) {
                    glDrawElements(vRenderMode, (GLsizei)m_Indices.size(), GL_UNSIGNED_INT, nullptr);
                } else {
                    glDrawElements(vRenderMode, vIndicesIdx, GL_UNSIGNED_INT, nullptr);
                }
            }
        } else if ((!m_IndicesOffsets.empty()) && (!m_IndicesCounts.empty()) && (m_ArraysCount > 0)) {
#ifdef PROFILER_SCOPED
            PROFILER_SCOPED("Opengl", "glMultiDrawArrays");
#endif
            glMultiDrawArrays(vRenderMode, m_IndicesOffsets.data(), m_IndicesCounts.data(), m_ArraysCount);
        } else {
#ifdef PROFILER_SCOPED
            PROFILER_SCOPED("Opengl", "glDrawArrays");
#endif
            glDrawArrays(vRenderMode, 0, GLsizei(m_Vertices.size()));
        }
        CheckGLErrors;
    }
    void m_uploadMeshDatasIfNeeded(const GLsizei vVerticesIdx, const GLsizei vIndicesIdx) {
        if (m_needNewMeshUpload) {
            m_needNewMeshUpload = false;
#ifdef PROFILER_SCOPED
            PROFILER_SCOPED("Opengl", "uploadMeshDatas");
#endif
            // bind
            glBindVertexArray(m_VaoId);
            CheckGLErrors;
            glBindBuffer(GL_ARRAY_BUFFER, m_VboId);
            CheckGLErrors;
            glBufferSubData(GL_ARRAY_BUFFER, 0, m_VerticeSize * vVerticesIdx, m_Vertices.data());
            CheckGLErrors;

            // indices
            if (!m_Indices.empty()) {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IboId);
                CheckGLErrors;
                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_IndiceSize * vIndicesIdx, m_Indices.data());
                CheckGLErrors;
            }

            // unbind
            glBindVertexArray(0);
            CheckGLErrors;
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            if (!m_Indices.empty()) {
                CheckGLErrors;
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }
        }
    }
};

}  // namespace gl
}  // namespace ez
