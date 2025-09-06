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

#include <map>
#include <string>
#include <memory>
#include "../ezScreen.hpp"

namespace ez {
namespace gl {

class Canvas {
public:
    struct MouseDatas {
        ez::fvec2 displayRect;
        ez::fvec2 mousePos;
        float mouseWheel{};
        bool isLeftButtonDown{};
    };
    struct Transform {
        ez::fvec2 origin;
        float scale{1.0f};  // px / unit� monde
        float invScale{1.0f};
    };
    struct UniformTransform {
        ez::fvec2 uScale{1.0f};
        ez::fvec2 uOffset;
    };


private:
    bool m_isRenderingActive{true};
    ez::fvec4 m_clearColor;

    ez::fvec4 m_displayRect;
    ez::gl::FBOPipeLinePtr mp_fboPipeline;
    ez::fvec2 m_fboSize;

    MouseDatas m_mouseDatas;
    Transform m_transform;

    // Uniforms NDC pour le shader (uScale/uOffset)
    UniformTransform m_uniformTransform;

    // Souris (interne)
    ez::fvec2 m_lastMousePos;
    bool m_isPanning{false};

public:
    bool init(const ez::ivec4& vDisplayRect) {
        bool ret = true;
        m_displayRect = vDisplayRect;
        mp_fboPipeline = ez::gl::FBOPipeLine::create(vDisplayRect.z, vDisplayRect.w, 1, false, false);
        ret &= (mp_fboPipeline != nullptr);
        m_fboSize = m_displayRect.zw();
        return ret;
    }
    void unit() {}
    void startFrame(const MouseDatas& vMouseDatas) {
        if (m_mouseActions(vMouseDatas)) {
            m_computeTransform();
        }
    }

    void endFrame() {}

    bool resize(const ez::ivec4& vNewDisplayRect) {
        if (mp_fboPipeline->resize(vNewDisplayRect.z, vNewDisplayRect.w)) {
            m_displayRect = vNewDisplayRect;
            if (!m_fboSize.emptyOR()) {
                ez::fvec2 rescale = m_displayRect.zw() / m_fboSize;
                m_transform.origin *= rescale;
                m_transform.scale *= ez::mini(rescale.x, rescale.y);
            }
            m_fboSize = vNewDisplayRect.zw();
            m_computeTransform();
            return true;
        }
        return false;
    }

    ez::fvec2 worldToLocal(const ez::fvec2& vWorldPos) { 
        return (vWorldPos - m_transform.origin) * m_transform.invScale;
    }

    ez::fvec2 localToWorld(const ez::fvec2& vCanvasPos) { 
       return vCanvasPos * m_transform.scale + m_transform.origin; 
    }

    bool startOffscreenRender() {
        if (m_isRenderingActive) {
            clearBuffers(m_clearColor);
            if (mp_fboPipeline->bind()) {
                mp_fboPipeline->selectBuffers();
                return true;
            }
        }
        return false;
    }
    void endOffscreenRender() { mp_fboPipeline->unbind(); }

    void blitOnScreen() {
        auto fbo_ptr = mp_fboPipeline->getFrontFBO().lock();
        if (fbo_ptr != nullptr) {
            fbo_ptr->blitOnScreen(
                m_displayRect.x,  //
                m_displayRect.y,  //
                m_displayRect.z,  //
                m_displayRect.w,  //
                0,
                GL_COLOR_BUFFER_BIT,
                GL_NEAREST);
        }
    }
    void clearBuffers(const ez::fvec4& vColorPtr) { mp_fboPipeline->clearBuffer({vColorPtr.x, vColorPtr.y, vColorPtr.z, vColorPtr.w}); }
    bool drawUI() {
#ifdef IMGUI_API
#ifdef _DEBUG
        if (ImGui::CollapsingHeader("Debug##Renderer")) {
            ImGui::Text("Origin Px : %f,%f", m_transform.origin.x, m_transform.origin.y);
            ImGui::Text("FBO Size : %f,%f", m_fboSize.x, m_fboSize.y);
            ImGui::Text("Scale : %f", m_transform.scale);
            ImGui::Text("Sclae inv : %f", m_transform.invScale);
            ImGui::DragFloat2("Offset", &m_uniformTransform.uOffset.x);
            ImGui::DragFloat2("Scale", &m_uniformTransform.uScale.x);
        }
#endif
#endif
        return false;
    }

    void updateTransform() { m_computeTransform(); }

    void fitToContent(const ez::fAABB& vBoundingBox) {
        m_computeFitToContent(  //
            vBoundingBox.lowerBound,
            vBoundingBox.upperBound,
            m_fboSize,
            m_transform.origin,
            m_transform.scale,
            m_transform.invScale);
        m_computeTransform();
    }

    ez::fvec4& getBackgroundColorRef() { return m_clearColor; }
    MouseDatas& getMouseDatasRef() { return m_mouseDatas; }
    Transform& getTransfomRef() { return m_transform; }
    UniformTransform& getUniformTransfomRef() { return m_uniformTransform; }

private:
    bool m_mouseActions(const MouseDatas& vMouseDatas) {
        bool ret = false;
        const float steps = vMouseDatas.mouseWheel;
        if (fabsf(steps) > 0.0001f) {
            const float factor = powf(1.1f, steps);
            m_applyZoomAtMouse(factor, vMouseDatas.mousePos);
            ret = true;
        }
        if (vMouseDatas.isLeftButtonDown) {
            const ez::fvec2 delta = vMouseDatas.mousePos - m_lastMousePos;
            m_isPanning = true;
            m_applyPanDrag(delta);
            ret = true;
        } else {
            m_isPanning = false;
        }
        m_lastMousePos = vMouseDatas.mousePos;
        return ret;
    }

    void m_setScale(float v) {
        m_transform.scale = v;
        m_transform.invScale = (v != 0.0f) ? (1.0f / v) : 0.0f;
    }

    void m_applyPanDrag(const ez::fvec2& dragPx) {
        m_transform.origin.x += dragPx.x;
        m_transform.origin.y += dragPx.y;
    }

    void m_applyZoomAtMouse(float factor, const ez::fvec2& mousePx) {
        if (factor <= 0.0f) {
            return;
        }
        float newScale = m_transform.scale * factor;
        newScale = std::max(1e-6f, std::min(newScale, 1e6f));
        const float applied = (m_transform.scale > 0.0f) ? (newScale / m_transform.scale) : 1.0f;
        if (applied == 1.0f) {
            return;
        }
        m_transform.origin.x = m_transform.origin.x + (1.0f - applied) * (mousePx.x - m_transform.origin.x);
        m_transform.origin.y = m_transform.origin.y + (1.0f - applied) * (mousePx.y - m_transform.origin.y);
        m_setScale(newScale);
    }

    // computation of uScale/uOffset (NDC) for
    // gl_Position = vec4(aPos * uScale + uOffset, 0, 1)
    void m_computeTransform() {
        const float VW = m_fboSize.x;
        const float VH = m_fboSize.y;
        if (VW <= 0.0f || VH <= 0.0f) {
            m_transform = {};
            return;
        }
        m_setScale(m_transform.scale);
        m_uniformTransform.uScale.x = (2.0f * m_transform.scale) / VW;
        m_uniformTransform.uOffset.x = (2.0f * m_transform.origin.x) / VW - 1.0f;
        // Y-down (framebuffer in top-left, classic UI)
        m_uniformTransform.uScale.y = -(2.0f * m_transform.scale) / VH;
        m_uniformTransform.uOffset.y = 1.0f - (2.0f * m_transform.origin.y) / VH;
    }

    // Fit "contain" d'un AABB monde dans un framebuffer en pixels.
    // Mapping utilis� : screen = world * scale + originPx
    void m_computeFitToContent(
        const ez::fvec2& vWorldMin,
        const ez::fvec2& vWorldMax,
        const ez::fvec2& vFramebufferSizePx,
        ez::fvec2& voOriginPx,
        float& voScale,
        float& voInvScale) {
        const float W = vWorldMax.x - vWorldMin.x;
        const float H = vWorldMax.y - vWorldMin.y;
        const float VW = vFramebufferSizePx.x;
        const float VH = vFramebufferSizePx.y;

        if (W <= 0.0f || H <= 0.0f || VW <= 0.0f || VH <= 0.0f) {
            voOriginPx = {0.0f, 0.0f};
            voScale = 1.0f;
            voInvScale = 1.0f;
            return;
        }

        const float sx = VW / W;
        const float sy = VH / H;
        const float s = (sx < sy) ? sx : sy;

        const float padX = 0.5f * (VW - W * s);
        const float padY = 0.5f * (VH - H * s);

        voOriginPx.x = padX - vWorldMin.x * s;
        voOriginPx.y = padY - vWorldMin.y * s;
        voScale = s;
        voInvScale = (s != 0.0f) ? (1.0f / s) : 0.0f;
    }
};

}  // namespace gl
}  // namespace ez
