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

// ezScreen is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

namespace ez {

template <typename T>
inline vec4<T> getScreenRectWithSize(vec2<T> vItemSize, vec2<T> vMaxSize, bool vLogError = true) {
    vec4<T> rc;

    fvec2 visibleSize = fvec2((float)vMaxSize.x, (float)vMaxSize.y);
    if (visibleSize.x > 0.0f && visibleSize.y > 0.0f) {
        fvec2 visibleOrigin;
        const fvec2 texSize = fvec2((float)vItemSize.x, (float)vItemSize.y);

        // float visibleRatio = visibleSize.x / visibleSize.y;
        const float refRatio = texSize.x / texSize.y;

        float newX = visibleSize.y * refRatio;
        float newY = visibleSize.x / refRatio;

        if (newX < visibleSize.x) {
            // pos
            rc.x = (T)(visibleOrigin.x + (visibleSize.x - newX) * 0.5f);
            rc.y = (T)visibleOrigin.y;

            // size
            rc.z = (T)newX;
            rc.w = (T)visibleSize.y;
        } else {
            // pos
            rc.x = (T)visibleOrigin.x;
            rc.y = (T)(visibleOrigin.y + (visibleSize.y - newY) * 0.5f);

            // size
            rc.z = (T)visibleSize.x;
            rc.w = (T)newY;
        }

        if constexpr (std::is_same<T, float>::value || std::is_same<T, double>::value) {
            rc = floor<T>(rc);
        }

        /*const float newRatio = (float)rc.w / (float)rc.h;
        if (vLogError)
            if (IS_FLOAT_DIFFERENT(newRatio, refRatio))
                printf("GetScreenRectWithRatio : the new ratio is not the same as the ref ratio\n");*/
    }

    return rc;
}

template <typename T>
inline vec4<T> getScreenRectWithRatio(float vRatio, vec2<T> vMaxSize, bool vLogError = true) {
    vec4<T> rc;

    fvec2 visibleSize = fvec2((float)vMaxSize.x, (float)vMaxSize.y);
    if (visibleSize.x > 0.0f && visibleSize.y > 0.0f) {
        fvec2 visibleOrigin;

        const float refRatio = vRatio;

        float newX = visibleSize.y * refRatio;
        float newY = visibleSize.x / refRatio;

        if (newX < visibleSize.x) {
            // pos
            rc.x = (T)(visibleOrigin.x + (visibleSize.x - newX) * 0.5f);
            rc.y = (T)visibleOrigin.y;

            // size
            rc.z = (T)newX;
            rc.w = (T)visibleSize.y;
        } else {
            // pos
            rc.x = (T)visibleOrigin.x;
            rc.y = (T)(visibleOrigin.y + (visibleSize.y - newY) * 0.5f);

            // size
            rc.z = (T)visibleSize.x;
            rc.w = (T)newY;
        }

        if constexpr (std::is_same<T, float>::value || std::is_same<T, double>::value) {
            rc = floor<T>(rc);
        }

        /*const float newRatio = (float)rc.w / (float)rc.h;
        if (vLogError)
            if (IS_FLOAT_DIFFERENT(newRatio, refRatio))
                printf("GetScreenRectWithRatio : the new ratio is not the same as the ref ratio\n");*/
    }

    return rc;
}

inline fvec2 screenToWorld_centered(
    const fvec2& mousePx,
    const fvec2& viewportSize,
    const fvec2& worldCenter,
    float sPx,
    bool yDown  // true si Y écran descend
) {
    const float VW = viewportSize.x, VH = viewportSize.y;
    const float dx = mousePx.x - 0.5f * VW;
    const float dy = yDown ? (mousePx.y - 0.5f * VH) : (0.5f * VH - mousePx.y);
    return {worldCenter.x + dx / sPx, worldCenter.y + dy / sPx};
}

inline void centeredZoom(
    float factor,  // >1 zoom-in, <1 zoom-out
    const fvec2& mousePx,
    const fvec2& viewportSize,
    bool yDown,
    float sFitPx,           // pixels/unit, issu de ton fit
    float& zoom,            // IN/OUT
    fvec2& worldCenter  // IN/OUT
) {
    if (factor <= 0.0f || sFitPx <= 0.0f)
        return;

    const float VW = viewportSize.x, VH = viewportSize.y;
    const float sPx_before = zoom * sFitPx;

    // delta écran (centre -> souris), signé selon Y-up/Y-down
    const float dx = mousePx.x - 0.5f * VW;
    const float dy = yDown ? (mousePx.y - 0.5f * VH) : (0.5f * VH - mousePx.y);

    // point monde sous la souris AVANT zoom
    const fvec2 worldUnderMouse = {worldCenter.x + dx / sPx_before, worldCenter.y + dy / sPx_before};

    // nouveau zoom
    zoom *= factor;
    const float sPx_after = zoom * sFitPx;

    // ajuste le centre pour garder worldUnderMouse sous la souris
    worldCenter = {worldUnderMouse.x - dx / sPx_after, worldUnderMouse.y - dy / sPx_after};
}

inline void zoomedTranslation(
    const fvec2& dragPx,  // delta souris en pixels
    bool yDown,
    float sFitPx,
    float zoom,
    fvec2& worldCenter  // IN/OUT
) {
    if (sFitPx <= 0.0f || zoom <= 0.0f)
        return;
    const float sPx = zoom * sFitPx;

    // déplacer le centre à l'opposé du drag (comme une carte)
    const float dx = dragPx.x;
    const float dy = yDown ? dragPx.y : -dragPx.y;

    worldCenter.x -= dx / sPx;
    worldCenter.y -= dy / sPx;
}

inline float computeFitScalePx(const fvec2& worldMin, const fvec2& worldMax, const fvec2& viewportSize) {
    const float W = worldMax.x - worldMin.x;
    const float H = worldMax.y - worldMin.y;
    if (W <= 0.0f || H <= 0.0f || viewportSize.x <= 0.0f || viewportSize.y <= 0.0f)
        return 0.0f;
    const fvec4 rc = getScreenRectWithSize({W, H}, viewportSize);  // {x,y,w,h}
    return rc.z / W;                                                     // == rc.w / H
}

// Fit "contain" d'un AABB monde dans un framebuffer en pixels.
// Mapping utilisé : screen = world * scale + originPx
inline void computeFitToContent(
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

}  // namespace ez
