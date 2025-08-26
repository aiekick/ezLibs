﻿#pragma once

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
#include <unordered_map>
#include <vector>
#include <string>
#include <cstring> // memset, memcpy
#include <cmath>

#define PRINT_BLOCK_DATAS

namespace ez {
namespace gl {

class BufferBlock {
protected:
    GLuint m_buffer = 0;  // buffer id
    GLenum m_target = 0;  // ex : GL_UNIFORM_BUFFER;
    GLenum m_usage = 0;  // ex: GL_DYNAMIC_DRAW;
    std::string m_name;

public:
    BufferBlock() = default;
    explicit BufferBlock(const std::string& vName, GLenum target, GLenum usage) : m_name(vName), m_target(target), m_usage(usage) {}
    virtual ~BufferBlock() { destroy(); }
    void create(size_t size, const void* data) {
        glGenBuffers(1, &m_buffer);
        CheckGLErrors;
        glBindBuffer(m_target, m_buffer);
        glBufferData(m_target, size, data, m_usage);
        CheckGLErrors;
    }
    void upload(size_t size, const void* data) {
#ifdef PROFILER_SCOPED_PTR
        PROFILER_SCOPED_PTR(this, "upload BufferBlock ", "%s", m_name.c_str());
#endif
        glBindBuffer(m_target, m_buffer);
        glBufferData(m_target, size, data, m_usage);
        CheckGLErrors;
    }
    void bind(GLuint binding) {
        glBindBufferBase(m_target, binding, m_buffer);
        CheckGLErrors;
    }
    void destroy() {
        if (m_buffer != 0) {
            glDeleteBuffers(1, &m_buffer);
            CheckGLErrors;
            m_buffer = 0;
        }
    }
    GLuint id() const { return m_buffer; }
};

// UBOAuto : layout std140
// this ubo can be udpated dynamically by code and will manage automatically resources and layout std140

class UBOAuto {
private:
    BufferBlock m_buffer;
    // key = uniform name, offset in buffer for have op on uniform data
    std::unordered_map<std::string, uint32_t> m_offsets;
    // uniforms datas buffer
    std::vector<uint8_t> m_datas;
    // if he is dirty, value has been changed and mut be uploaded in gpu memory
    // dirty at first for init in gpu memory
    bool m_isDirty = true;

public:
    UBOAuto(const std::string& vName) : m_buffer(vName, GL_UNIFORM_BUFFER, GL_DYNAMIC_DRAW) {}
    ~UBOAuto() { unit(); }
    bool init() { return true; }
    void unit() {
        m_buffer.destroy();
        clear();
    }
    void clear() {
        m_datas.clear();
        m_offsets.clear();
        m_isDirty = false;
    }
    void setDirty() { m_isDirty = true; }
    bool isDirty() const { return m_isDirty; }
    void create() { m_buffer.create(m_datas.size(), m_datas.data()); }
    void destroy() { m_buffer.destroy(); }
    bool recreate() {
        bool res = false;
        if (!m_datas.empty() && m_buffer.id() != 0U) {
            destroy();
            create();
            res = true;
        }
        return res;
    }
    void uploadIfDirty() {
        if (m_isDirty && !m_datas.empty()) {
            m_buffer.upload(m_datas.size(), m_datas.data());
            m_isDirty = false;
        }
    }
    void bind(const uint32_t vBinding) { m_buffer.bind(vBinding); }
    GLuint id() const { return m_buffer.id(); }
    // add size to uniform block, return startOffset
    bool registerByteSize(const std::string& vKey, uint32_t vSizeInBytes, uint32_t* vStartOffset) {
        if (offsetExist(vKey)) {
            LogVarDebugWarning("Debug : key %s is already defined in buffer. %s fail.", vKey.c_str(), __FUNCTION__);
        } else if (vSizeInBytes > 0) {
            uint32_t newSize = vSizeInBytes;
            uint32_t lastOffset = (uint32_t)m_datas.size();
            auto baseAlign = getStd140Alignment(newSize);
            // il faut trouver le prochain offset qui est multiple de baseAlign
            auto startOffset = baseAlign * static_cast<uint32_t>(std::ceil(static_cast<double>(lastOffset) / static_cast<double>(baseAlign)));
            auto newSizeToAllocate = startOffset - lastOffset + newSize;
#ifdef PRINT_BLOCK_DATAS
            auto endOffset = startOffset + newSize;
            LogVarDebugInfo(
                "key %s, size %u, align %u, Offsets : %u => %u, size to alloc %u", vKey.c_str(), newSize, baseAlign, startOffset, endOffset, newSizeToAllocate);
#endif
            m_datas.resize(lastOffset + newSizeToAllocate);
            // on set de "lastOffset" a "lastOffset + newSizeToAllocate"
            std::memset(m_datas.data() + lastOffset, 0, newSizeToAllocate);
            addOffsetForKey(vKey, startOffset);
            if (vStartOffset) {
                *vStartOffset = startOffset;
            }
            return true;
        }
        return false;
    }

    // templates (defined under class)
    // add a variable
    template <typename T>
    void registerVar(const std::string& vKey, T vValue);  // add var to uniform block
    template <typename T>
    void registerVar(const std::string& vKey, T* vValue, uint32_t vSizeInBytes);  // add var to uniform block

    // Get / set + op on variables
    template <typename T>
    bool getVar(const std::string& vKey, T& vValue);  // Get
    template <typename T>
    bool setVar(const std::string& vKey, T vValue);  // set
    template <typename T>
    bool setVar(const std::string& vKey, T* vValue, uint32_t vSizeInBytes);  // set
    template <typename T>
    bool setAddVar(const std::string&, T vValue);  // add and set like +=

private:
    void addOffsetForKey(const std::string& vKey, uint32_t vOffset) { m_offsets[vKey] = vOffset; }
    bool offsetExist(const std::string& vKey) { return m_offsets.find(vKey) != m_offsets.end(); }

protected:
#if 1
    // tested
    uint32_t getStd140Alignment(uint32_t vSize) {
        auto goodAlign = static_cast<uint32_t>(std::pow(2, std::ceil(std::log(vSize) / std::log(2))));
        if (goodAlign > 16U) {
            return 16U;
        }
        return goodAlign;
    }
#else
    // maybe closer to the std140, to test
    uint32_t getStd140Alignment(uint32_t vSize) {
        if (vSize <= 4) {
            return 4;
        }
        if (vSize <= 8) {
            return 8;
        }
        return 16;
    }
    // ou un mix des deux ?
    uint32_t getStd140Alignment(uint32_t vSize) {
        if (vSize <= 16) {
            if (vSize <= 4)
                return 4;  // float, int, uint
            if (vSize <= 8)
                return 8;  // vec2, ivec2
            return 16;  // vec3 (12), vec4 (16), mat3 row (12)
        }

        // vSize > 16 → struct, array, mat*, etc.
        uint32_t pow2 = 1 << (uint32_t)std::ceil(std::log2(vSize));
        return std::min(pow2, 16u);  // clip max align à 16
    }
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// PUBLIC TEMPLATES //////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void UBOAuto::registerVar(const std::string& vKey, T* vValue, uint32_t vSizeInBytes) {
    uint32_t startOffset;
    if (registerByteSize(vKey, vSizeInBytes, &startOffset)) {
        // on copy de "startOffset" a "startOffset + vSizeInBytes"
        std::memcpy(m_datas.data() + startOffset, vValue, vSizeInBytes);
    }
}

template <typename T>
void UBOAuto::registerVar(const std::string& vKey, T vValue) {
    registerVar(vKey, &vValue, sizeof(vValue));
}

template <typename T>
bool UBOAuto::getVar(const std::string& vKey, T& vValue) {
    if (offsetExist(vKey)) {
        uint32_t offset = m_offsets[vKey];
        uint32_t size = sizeof(vValue);
        std::memcpy(&vValue, m_datas.data() + offset, size);
        return true;
    }
    LogVarDebugInfo("Debug : key %s not exist in buffer. %s fail.", vKey.c_str(), __FUNCTION__);
    return false;
}

template <typename T>
bool UBOAuto::setVar(const std::string& vKey, T* vValue, uint32_t vSizeInBytes) {
    if (offsetExist(vKey) && vSizeInBytes > 0) {
        uint32_t newSize = vSizeInBytes;
        uint32_t offset = m_offsets[vKey];
        std::memcpy(m_datas.data() + offset, vValue, newSize);
        m_isDirty = true;
        return true;
    }
    LogVarDebugInfo("Debug : key %s not exist in buffer. %s fail.", vKey.c_str(), __FUNCTION__);
    return false;
}

template <typename T>
bool UBOAuto::setVar(const std::string& vKey, T vValue) {
    return setVar(vKey, &vValue, sizeof(vValue));
}

template <typename T>
bool UBOAuto::setAddVar(const std::string& vKey, T vValue) {
    T v;
    if (getVar(vKey, v)) {
        v += vValue;
        return setVar(vKey, v);
    }
    LogVarDebugInfo("Debug : key %s not exist in buffer. %s fail.", vKey.c_str(), __FUNCTION__);
    return false;
}

}  // namespace gl
}  // namespace ez
