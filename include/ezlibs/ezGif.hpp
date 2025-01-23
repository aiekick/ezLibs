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

// ezGif is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdint>
#include <string>
#include <vector>
#include <cstdio>
#include <cmath>
#include <map>

namespace ez {
namespace img {

// Gif file format :
// https://www.loc.gov/preservation/digital/formats/fdd/fdd000133.shtml
// https://www.matthewflickinger.com/lab/whatsinagif/index.html
// https://www.matthewflickinger.com/lab/whatsinagif/gif_explorer.asp

// https://www.matthewflickinger.com/lab/whatsinagif/lzw_image_data.asp

class Gif {
public:
    typedef uint8_t ColorIndex;

    struct RGB {
        uint8_t r = 0U;
        uint8_t g = 0U;
        uint8_t b = 0U;

        RGB() = default;

        RGB(uint8_t vR, uint8_t vG, uint8_t vB) : r(vR), g(vG), b(vB) {
        }
    };

private:
    FILE *mp_file = nullptr;
    int32_t m_LastError = 0;
    uint16_t m_width = 0U;
    uint16_t m_height = 0U;
    uint8_t m_minCodeSize = 0U;
    std::map<ColorIndex, RGB> m_colors;
    std::vector<ColorIndex> m_pixels;

public:
    Gif& setSize(uint16_t vWidth, uint16_t vHeight) {
        m_width = vWidth;
        m_height = vHeight;
        // todo : il faudra sauver les pixel
        // avant resize et transferer apres
        m_pixels.resize(m_width * m_height);
        return *this;
    }

    Gif &addColor(ColorIndex vIndex, const RGB &vColor) {
        m_colors[vIndex] = vColor;
        return *this;
    }

    Gif &addPixel(uint16_t vX, uint16_t vY, ColorIndex vIndex) {
        m_pixels[vY * m_width + vX] = vIndex;
        return *this;
    }

    bool save(const std::string &vFilePathName) {
        if (vFilePathName.empty()) {
            return false;
        }
        #ifdef _MSC_VER
        #else
        #endif
        if (m_openFileForWriting(vFilePathName)) {
            m_writeHeader();
            m_writeLogicalScreenDescriptor();
            m_writeGlobalColorTable();
            m_writeGraphicControlExtension();
            m_writeImageDescriptor();
            m_writeImageData();
            m_writeTrailer();
            m_closeFile();
        }
        return true;
    }

private:
    bool m_openFileForWriting(const std::string &vFilePathName) {
#if _MSC_VER
        m_LastError = fopen_s(&mp_file, vFilePathName.c_str(), "wb");
#else
        mp_file = fopen(vFilePathName.c_str(), "wb");
        m_LastError = mp_file ? 0 : errno;
#endif
        return (m_LastError == 0);
    }

    void m_closeFile() {
        fclose(mp_file);
    }

    void m_writeHeader() {
        m_writerString("GIF89a");
    }

    void m_writeLogicalScreenDescriptor() {
        m_writeShort(m_width);   // width
        m_writeShort(m_height);  // height
        m_writeByte(0x91);       // packed field. to detail in few times
        m_writeByte(0);          // bg color index
        m_writeByte(0);          // pixel aspect ratio
    }

    void m_writeGlobalColorTable() {
        auto bits_count = m_getBitsCount(m_colors.size());
        m_minCodeSize = static_cast<uint8_t>(bits_count);
        auto colorCount = m_getMaxNumForBitCount(bits_count);
        // colors
        for (const auto &color : m_colors) {
            m_writeByte(color.second.r);
            m_writeByte(color.second.g);
            m_writeByte(color.second.b);
            --colorCount;
        }
        // padding
        for (size_t idx = 0; idx < colorCount; ++idx) {
            m_writeByte(0x00);
            m_writeByte(0x00);
            m_writeByte(0x00);
        }
    }

    void m_writeGraphicControlExtension() {
        m_writeByte(0x21);     // always 21
        m_writeByte(0xF9);     // always F9
        m_writeByte(0x04);     // byte size
        m_writeByte(0x00);     // packed field
        m_writeShort(0x0000);  // delay time
        m_writeByte(0x00);     // transparent color index
        m_writeByte(0x00);     // always 0
    }

    void m_writeImageDescriptor() {
        m_writeByte(0x2C);     // always 2C
        m_writeShort(0x0000);  // image left
        m_writeShort(0x0000);  // image top
        m_writeShort(10);      // image width
        m_writeShort(10);      // image height
        m_writeByte(0x00);     // packed field
    }

    void m_writeImageData() {
        m_writeByte(m_minCodeSize);
        m_writeCodeTable();
        m_writeByte(0x00);  // Bloc de terminaison pour l'image
    }

    static size_t m_getBitsCount(size_t vValue) {
        return (size_t)std::floor(log2((double)vValue)) + 1;
    }

    static size_t m_getMaxNumForBitCount(size_t vValue) {
        return (1ULL << vValue);
    }

    void m_writeCodeTable() {
        // m_writeCodeTableHeader();
        m_writeCompressedData(m_compress(m_pixels));
    }

    uint8_t m_getClearCode() const {
        return 1 << m_minCodeSize;
    }

    uint8_t m_getEndOfInfoCode() const {
        return m_getClearCode() + 1;
    }

    void m_writeCodeTableHeader() {
        auto colorCount = m_getMaxNumForBitCount(m_minCodeSize);
        // colors
        for (const auto &color_index : m_colors) {
            m_writeByte(color_index.first);
            --colorCount;
        }
        // colors padding
        for (size_t idx = 0; idx < colorCount; ++idx) {
            m_writeByte(static_cast<uint8_t>(m_colors.size() + idx));
        }
        // clear code
        m_writeByte(m_getClearCode());
        // end of information code
        m_writeByte(m_getEndOfInfoCode());
    }

    void m_writeTrailer() {
        m_writeByte(0x3B);
    }

    void m_writeBuffer(const std::vector<uint8_t> &vBuffer, size_t vOffset = 0, size_t vMaxSize = 0) {
        if (!vMaxSize) {
            vMaxSize = vBuffer.size();
        }
        fwrite(&vBuffer.at(vOffset), sizeof(uint8_t), vMaxSize, mp_file);
    }

    void m_writerString(const std::string &vValue) {
        fwrite(vValue.c_str(), sizeof(uint8_t), vValue.size(), mp_file);
    }

    void m_writeByte(uint8_t vValue) {
        fputc(vValue, mp_file);
        std::cout << "write "
                  << "0x" << std::setfill('0') << std::setw(2) << std::right << std::uppercase << std::hex << (int32_t)vValue << std::endl;
    }

    void m_writeShort(uint16_t vValue) {
        m_writeByte(vValue & 0xFF);
        m_writeByte((vValue >> 8) & 0xFF);
    }

    std::vector<uint16_t> m_compress(const std::vector<uint8_t> &vDatas) {
        std::vector<uint16_t> result;
        result.push_back(m_getClearCode());
        std::map<std::vector<uint16_t>, uint16_t> dico;
        for (uint16_t i = 0; i < 256; ++i) {
            dico[{i}] = i;
        }
        uint16_t code = 256U;
        uint16_t c = vDatas.at(0);
        std::vector<uint16_t> p{c};
        std::vector<uint16_t> pc;
        for (size_t idx = 1; idx < vDatas.size(); ++idx) {
            c = vDatas.at(idx);
            pc = p;
            pc.push_back(c);
            if (dico.find(pc) != dico.end()) {
                p.push_back(c);
            } else {
                result.push_back(dico.at(p));
                dico[pc] = code++;
                p = {c};
                if (code >= 4096) {
                    result.push_back(m_getClearCode());
                    dico.clear();
                    for (uint16_t i = 0; i < 256; ++i) {
                        dico[{i}] = i;
                    }
                    code = 258;
                }
            }
        }
        result.push_back(dico.at(p));
        result.push_back(m_getEndOfInfoCode());
        return result;
    }

    void m_writeCompressedData(const std::vector<uint16_t> &compressedData) {
        uint8_t bitBuffer = 0;                        // Buffer de bits pour l'�criture des octets
        int bitCount = 0;                             // Nombre de bits accumul�s dans le buffer
        uint8_t currentCodeSize = m_minCodeSize + 1;  // Taille actuelle des codes

        std::vector<uint8_t> outputBytes;

        for (auto code : compressedData) {
            // Ajouter le code au buffer de bits
            bitBuffer |= (code << bitCount);
            bitCount += currentCodeSize;

            // �crire les octets entiers dans le buffer jusqu'� ce qu'on n'ait plus de 8 bits
            while (bitCount >= 8) {
                outputBytes.push_back(bitBuffer & 0xFF);  // �crire le byte entier
                bitBuffer >>= 8;                          // D�caler le buffer vers la droite de 8 bits
                bitCount -= 8;
            }

            // Augmenter la taille des codes au fur et � mesure que le dictionnaire grandit
            if (code == (1 << currentCodeSize) - 1 && currentCodeSize < 12) {
                ++currentCodeSize;
            }
        }

        // �crire les bits restants dans le buffer (si non vide)
        if (bitCount > 0) {
            outputBytes.push_back(bitBuffer & 0xFF);
        }

        // �crire les donn�es en blocs de 255 octets maximum
        size_t index = 0;
        while (index < outputBytes.size()) {
            size_t blockSize = std::min(size_t(255), outputBytes.size() - index);
            m_writeByte(static_cast<uint8_t>(blockSize));  // Taille du bloc
            m_writeBuffer(outputBytes, index, blockSize);
            index += blockSize;
        }
        m_writeByte(0x00);  // Bloc de terminaison pour l'image
    }
};

}  // namespace img
}  // namespace ez
