#pragma once

/*
MIT License

Copyright (c) 1998-2025 Stephane Cuillerdier (aka aiekick)

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

// ezEmbed is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

/*
this fill is in charge of manage resources embedded inside app binary
*/

#include "ezOS.hpp"
#include "ezGL/ezGL.hpp"

#include <cstdint>

#ifdef WINDOWS_OS 
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincodec.h>
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "ole32.lib")
#endif // WINDOWS_OS

namespace ez {
namespace embed {

#ifdef WINDOWS_OS
inline void setEmbeddedIconApp(HWND vWindowHandle, const std::string& vEmbeddedIconID) {
    auto icon_h_inst = LoadIconA(GetModuleHandle(NULL), vEmbeddedIconID.c_str());
    SetClassLongPtrA(vWindowHandle, GCLP_HICON, (LONG_PTR)icon_h_inst);
}
inline GLuint extractEmbbededIconToGlTexture(const std::string& vEmbeddedIconID) {
    auto icon_h_inst = LoadIconA(GetModuleHandle(NULL), vEmbeddedIconID.c_str());
    ICONINFO app_icon_info;
    if (GetIconInfo(icon_h_inst, &app_icon_info)) {
        HDC hdc = GetDC(0);
        BITMAPINFO MyBMInfo = {0};
        MyBMInfo.bmiHeader.biSize = sizeof(MyBMInfo.bmiHeader);
        if (GetDIBits(hdc, app_icon_info.hbmColor, 0, 0, NULL, &MyBMInfo, DIB_RGB_COLORS)) {
            uint8_t* bytes = new uint8_t[MyBMInfo.bmiHeader.biSizeImage];
            MyBMInfo.bmiHeader.biCompression = BI_RGB;
            if (GetDIBits(hdc, app_icon_info.hbmColor, 0, MyBMInfo.bmiHeader.biHeight, (LPVOID)bytes, &MyBMInfo, DIB_RGB_COLORS)) {
                uint8_t R, G, B;
                int index, i;
                // swap BGR to RGB
                for (i = 0; i < MyBMInfo.bmiHeader.biWidth * MyBMInfo.bmiHeader.biHeight; i++) {
                    index = i * 4;
                    B = bytes[index];
                    G = bytes[index + 1];
                    R = bytes[index + 2];
                    bytes[index] = R;
                    bytes[index + 1] = G;
                    bytes[index + 2] = B;
                }
                // create texture from loaded bmp image
                glEnable(GL_TEXTURE_2D);
                GLuint texID = 0;
                glGenTextures(1, &texID);
                glBindTexture(GL_TEXTURE_2D, texID);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexImage2D(GL_TEXTURE_2D, 0, 4, MyBMInfo.bmiHeader.biWidth, MyBMInfo.bmiHeader.biHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
                glFinish();
                glBindTexture(GL_TEXTURE_2D, 0);
                glDisable(GL_TEXTURE_2D);
                return texID;
            }
        }
    }
    return 0;
}

inline GLuint extractEmbbededImageToGlTexture(const std::string& vEmbeddedImageID) {
    auto hBitmap = LoadImageA(GetModuleHandle(NULL), vEmbeddedImageID.c_str(), IMAGE_BITMAP, 0, 0, LR_COPYFROMRESOURCE | LR_CREATEDIBSECTION);
    BITMAP bm;
    if (GetObjectA(hBitmap, sizeof(BITMAP), &bm)) {
        uint8_t* bytes = (uint8_t*)bm.bmBits;
        uint8_t tmp_blue_to_red = 0U;
        int index = 0;
        // swap BGR to RGB
        for (int i = 0; i < bm.bmWidth * bm.bmHeight; ++i) {
            index = i * 3;
            tmp_blue_to_red = bytes[index];
            bytes[index] = bytes[index + 2];
            bytes[index + 2] = tmp_blue_to_red;
        }
        // create texture from loaded bmp image
        glEnable(GL_TEXTURE_2D);
        GLuint texID = 0;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bm.bmWidth, bm.bmHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
        glFinish();
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
        return texID;
    }
    return 0;
}

inline bool LoadResourceToMemory(const std::string& vResourceName, const std::string& vResourceType, const uint8_t** vOutData, size_t* vOutSize) {
    if (!vOutData || !vOutSize) {
        return false;
    }
    *vOutData = nullptr;
    *vOutSize = 0;

    HRSRC hResInfo = FindResourceA(nullptr, vResourceName.c_str(), vResourceType.c_str());
    if (!hResInfo) {
        return false;
    }

    HGLOBAL hRes = LoadResource(nullptr, hResInfo);
    if (!hRes) {
        return false;
    }

    DWORD size = SizeofResource(nullptr, hResInfo);
    if (size == 0) {
        return false;
    }

    void* ptr = LockResource(hRes);
    if (!ptr) {
        return false;
    }

    *vOutData = reinterpret_cast<const uint8_t*>(ptr);
    *vOutSize = static_cast<size_t>(size);
    return true;
}

inline GLuint extractEmbeddedPngToGlTexture(const std::string& vresourceName, const bool vInvY) {
    // 1) Récupérer les octets compressés du PNG
    const uint8_t* pngData = nullptr;
    size_t pngSize = 0;
    if (!LoadResourceToMemory(vresourceName.c_str(), "PNG", &pngData, &pngSize)) {
        return 0;
    }

    // 2) Init COM (idempotent si déjà fait)
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    const bool didCoInit = SUCCEEDED(hr);

    IWICImagingFactory* pFactory = nullptr;
    IWICStream* pStream = nullptr;
    IWICBitmapDecoder* pDecoder = nullptr;
    IWICBitmapFrameDecode* pFrame = nullptr;
    IWICFormatConverter* pConverter = nullptr;
    GLuint texID = 0;

    do {
        // 3) Fabrique WIC
        hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFactory));
        if (FAILED(hr)) {
            break;
        }

        // 4) Flux mémoire WIC
        hr = pFactory->CreateStream(&pStream);
        if (FAILED(hr)) {
            break;
        }

        hr = pStream->InitializeFromMemory(const_cast<BYTE*>(reinterpret_cast<const BYTE*>(pngData)), static_cast<DWORD>(pngSize));
        if (FAILED(hr)) {
            break;
        }

        // 5) Décodeur (PNG auto-détecté)
        hr = pFactory->CreateDecoderFromStream(pStream, nullptr, WICDecodeMetadataCacheOnDemand, &pDecoder);
        if (FAILED(hr)) {
            break;
        }

        // 6) On récupère la première frame
        hr = pDecoder->GetFrame(0, &pFrame);
        if (FAILED(hr)) {
            break;
        }

        // 7) Convertisseur -> 32bpp BGRA (alpha non prémultiplié)
        hr = pFactory->CreateFormatConverter(&pConverter);
        if (FAILED(hr)) {
            break;
        }

        hr = pConverter->Initialize(
            pFrame,
            GUID_WICPixelFormat32bppBGRA,  // straight alpha, pas prémultiplié
            WICBitmapDitherTypeNone,
            nullptr,
            0.0,
            WICBitmapPaletteTypeCustom);
        if (FAILED(hr)) {
            break;
        }

        // 8) Récupération dimensions + copie pixels
        UINT w = 0, h = 0;
        hr = pConverter->GetSize(&w, &h);
        if (FAILED(hr) || w == 0 || h == 0) {
            break;
        }

        const UINT stride = w * 4;  // BGRA 8bpc
        const UINT bufferSize = stride * h;
        std::vector<uint8_t> pixels(bufferSize);

        hr = pConverter->CopyPixels(nullptr, stride, bufferSize, pixels.data());
        if (FAILED(hr)) {
            break;
        }

        // 9) Upload OpenGL (BGRA -> évite le swap)
        GLint prevUnpack = 4;
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevUnpack);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (vInvY) {
            std::vector<uint8_t> tmp(stride);
            for (UINT y = 0; y < h / 2; ++y) {
                uint8_t* rowTop = pixels.data() + y * stride;
                uint8_t* rowBot = pixels.data() + (h - 1 - y) * stride;
                memcpy(tmp.data(), rowTop, stride);
                memcpy(rowTop, rowBot, stride);
                memcpy(rowBot, tmp.data(), stride);
            }
        }

        // Internal format & format externe
        // Internal: RGBA8
        // External data: BGRA + UNSIGNED_BYTE (desktop GL supporte GL_BGRA)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(w), static_cast<GLsizei>(h), 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels.data());

        // glGenerateMipmap(GL_TEXTURE_2D); // si besoin

        glBindTexture(GL_TEXTURE_2D, 0);
        glPixelStorei(GL_UNPACK_ALIGNMENT, prevUnpack);
    } while (false);

    // 10) Release WIC/COM
    if (pConverter) {
        pConverter->Release();
    }
    if (pFrame) {
        pFrame->Release();
    }
    if (pDecoder) {
        pDecoder->Release();
    }
    if (pStream) {
        pStream->Release();
    }
    if (pFactory) {
        pFactory->Release();
    }
    if (didCoInit) {
        CoUninitialize();
    }

    return texID;
}
#endif // WINDOWS_OS

}  // namespace embed
}  // namespace ez
