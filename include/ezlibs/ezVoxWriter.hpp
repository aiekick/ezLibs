#pragma once

/*
MIT License

Copyright (c) 2018-2024 Stephane Cuillerdier (aka aiekick)

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

// ezVox is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <map>
#include <cmath>
#include <array>
#include <string>
#include <vector>
#include <chrono>
#include <memory>
#include <cstdint>
#include <sstream>
#include <iostream>
#include <functional>

#include "ezMath.hpp"
#include "ezStr.hpp"

// This File is a helper for write a vox file after 0.99 release to support
// the world mode editor
// just add all color with the color Index with AddColor
// And add all voxels with the method AddVoxel with the voxel in world position, and finally save the model
// that's all, the file was initially created for my Proecedural soft
// it support just my needs for the moment, but i put here because its a basis for more i thinck

namespace ez {
namespace file {

    namespace vox {
typedef uint32_t KeyFrame;

typedef size_t CubeX;
typedef size_t CubeY;
typedef size_t CubeZ;
typedef size_t CubeID;
typedef size_t VoxelX;
typedef size_t VoxelY;
typedef size_t VoxelZ;
typedef size_t VoxelID;
typedef int32_t TagID;
typedef int32_t Version;
typedef int32_t ColorID;

typedef ez::dAABBCC Volume;

typedef std::function<void(const KeyFrame& vKeyFrame, const double& vValue)> KeyFrameTimeLoggingFunctor;

inline uint32_t GetMVID(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    return (a) | (b << 8) | (c << 16) | (d << 24);
}

struct DICTstring {
    int32_t bufferSize = 0;
    std::string buffer;

    DICTstring() = default;

    void write(FILE* fp) {
        bufferSize = (int32_t)buffer.size();
        fwrite(&bufferSize, sizeof(int32_t), 1, fp);
        fwrite(buffer.data(), sizeof(char), bufferSize, fp);
    }
    size_t getSize() {
        bufferSize = (int32_t)buffer.size();
        return sizeof(int32_t) + sizeof(char) * bufferSize;
    }
};

struct DICTitem {
    DICTstring key;
    DICTstring value;

    DICTitem() = default;
    DICTitem(std::string vKey, std::string vValue) {
        key.buffer = vKey;
        value.buffer = vValue;
    }

    void write(FILE* fp) {
        key.write(fp);
        value.write(fp);
    }

    size_t getSize() {
        return key.getSize() + value.getSize();
    }
};

struct DICT {
    int32_t count = 0;
    std::vector<DICTitem> keys;

    DICT() = default;

    void write(FILE* fp) {
        count = (int32_t)keys.size();
        fwrite(&count, sizeof(int32_t), 1, fp);
        for (auto& key : keys) {
            key.write(fp);
        }
    }

    size_t getSize() {
        size_t s = sizeof(int32_t);
        for (auto& key : keys) {
            s += key.getSize();
        }
        return s;
    }

    void Add(std::string vKey, std::string vValue) {
        keys.push_back(DICTitem(vKey, vValue));
    }
};

struct nTRN {
    int32_t nodeId = 0;
    DICT nodeAttribs;
    int32_t childNodeId = 0;
    int32_t reservedId = -1;
    int32_t layerId = -1;
    int32_t numFrames = 1;
    std::vector<DICT> frames;

    nTRN(int32_t countFrames) {
        numFrames = countFrames;
        frames.resize(static_cast<size_t>(numFrames));
    }

    void write(FILE* fp) {
        // chunk header
        int32_t id = GetMVID('n', 'T', 'R', 'N');
        fwrite(&id, sizeof(int32_t), 1, fp);
        size_t contentSize = getSize();
        fwrite(&contentSize, sizeof(int32_t), 1, fp);
        size_t childSize = 0;
        fwrite(&childSize, sizeof(int32_t), 1, fp);

        // datas's
        fwrite(&nodeId, sizeof(int32_t), 1, fp);
        nodeAttribs.write(fp);
        fwrite(&childNodeId, sizeof(int32_t), 1, fp);
        fwrite(&reservedId, sizeof(int32_t), 1, fp);
        fwrite(&layerId, sizeof(int32_t), 1, fp);
        fwrite(&numFrames, sizeof(int32_t), 1, fp);
        for (auto& frame : frames) {
            frame.write(fp);
        }
    }

    size_t getSize() {
        size_t s = sizeof(int32_t) * 5 + nodeAttribs.getSize();
        for (auto& frame : frames) {
            s += frame.getSize();
        }
        return s;
    }
};

struct nGRP {
    int32_t nodeId = 0;
    DICT nodeAttribs;
    int32_t nodeChildrenNodes;
    std::vector<int32_t> childNodes;

    nGRP(int32_t vCount) {
        nodeChildrenNodes = vCount;
        childNodes.resize(nodeChildrenNodes);
    }

    void write(FILE* fp) {
        // chunk header
        int32_t id = GetMVID('n', 'G', 'R', 'P');
        fwrite(&id, sizeof(int32_t), 1, fp);
        size_t contentSize = getSize();
        fwrite(&contentSize, sizeof(int32_t), 1, fp);
        size_t childSize = 0;
        fwrite(&childSize, sizeof(int32_t), 1, fp);

        // datas's
        fwrite(&nodeId, sizeof(int32_t), 1, fp);
        nodeAttribs.write(fp);
        fwrite(&nodeChildrenNodes, sizeof(int32_t), 1, fp);
        fwrite(childNodes.data(), sizeof(int32_t), nodeChildrenNodes, fp);
    }

    size_t getSize() {
        return sizeof(int32_t) * (2 + nodeChildrenNodes) + nodeAttribs.getSize();
    }
};

struct MODEL {
    int32_t modelId = 0;
    DICT modelAttribs;

    MODEL() = default;

    void write(FILE* fp) {
        fwrite(&modelId, sizeof(int32_t), 1, fp);
        modelAttribs.write(fp);
    }

    size_t getSize() {
        return sizeof(int32_t) + modelAttribs.getSize();
    }
};

struct nSHP {
    int32_t nodeId = 0;
    DICT nodeAttribs;
    int32_t numModels;
    std::vector<MODEL> models;

    nSHP(int32_t vCount) {
        numModels = vCount;
        models.resize(numModels);
    }

    void write(FILE* fp) {
        // chunk header
        int32_t id = GetMVID('n', 'S', 'H', 'P');
        fwrite(&id, sizeof(int32_t), 1, fp);
        size_t contentSize = getSize();
        fwrite(&contentSize, sizeof(int32_t), 1, fp);
        size_t childSize = 0;
        fwrite(&childSize, sizeof(int32_t), 1, fp);

        // datas's
        fwrite(&nodeId, sizeof(int32_t), 1, fp);
        nodeAttribs.write(fp);
        fwrite(&numModels, sizeof(int32_t), 1, fp);
        for (auto& model : models) {
            model.write(fp);
        }
    }

    size_t getSize() {
        size_t s = sizeof(int32_t) * 2 + nodeAttribs.getSize();
        for (auto& model : models) {
            s += model.getSize();
        }
        return s;
    }
};

struct LAYR {
    int32_t nodeId = 0;
    int32_t reservedId = -1;
    DICT nodeAttribs;

    LAYR() = default;

    void write(FILE* fp) {
        // chunk header
        int32_t id = GetMVID('L', 'A', 'Y', 'R');
        fwrite(&id, sizeof(int32_t), 1, fp);
        size_t contentSize = getSize();
        fwrite(&contentSize, sizeof(int32_t), 1, fp);
        size_t childSize = 0;
        fwrite(&childSize, sizeof(int32_t), 1, fp);

        // datas's
        fwrite(&nodeId, sizeof(int32_t), 1, fp);
        nodeAttribs.write(fp);
        fwrite(&reservedId, sizeof(int32_t), 1, fp);
    }

    size_t getSize() {
        return sizeof(int32_t) * 2 + nodeAttribs.getSize();
    }
};

struct SIZE {
    int32_t sizex = 0;
    int32_t sizey = 0;
    int32_t sizez = 0;

    SIZE() = default;

    void write(FILE* fp) {
        // chunk header
        int32_t id = GetMVID('S', 'I', 'Z', 'E');
        fwrite(&id, sizeof(int32_t), 1, fp);
        size_t contentSize = getSize();
        fwrite(&contentSize, sizeof(int32_t), 1, fp);
        size_t childSize = 0;
        fwrite(&childSize, sizeof(int32_t), 1, fp);

        // datas's
        fwrite(&sizex, sizeof(int32_t), 1, fp);
        fwrite(&sizey, sizeof(int32_t), 1, fp);
        fwrite(&sizez, sizeof(int32_t), 1, fp);
    }

    size_t getSize() {
        return sizeof(int32_t) * 3;
    }
};

struct XYZI {
    int32_t numVoxels = 0;
    std::vector<uint8_t> voxels;

    XYZI() = default;

    void write(FILE* fp) {
        // chunk header
        int32_t id = GetMVID('X', 'Y', 'Z', 'I');
        fwrite(&id, sizeof(int32_t), 1, fp);
        size_t contentSize = getSize();
        fwrite(&contentSize, sizeof(int32_t), 1, fp);
        size_t childSize = 0;
        fwrite(&childSize, sizeof(int32_t), 1, fp);

        // datas's
        fwrite(&numVoxels, sizeof(int32_t), 1, fp);
        fwrite(voxels.data(), sizeof(uint8_t), voxels.size(), fp);
    }

    size_t getSize() {
        numVoxels = (int32_t)voxels.size() / 4;
        return sizeof(int32_t) * (1 + numVoxels);
    }
};

struct RGBA {
    std::array<int32_t, 256> colors{};

    RGBA() = default;

    void write(FILE* fp) {
        // chunk header
        int32_t id = GetMVID('R', 'G', 'B', 'A');
        fwrite(&id, sizeof(int32_t), 1, fp);
        size_t contentSize = getSize();
        fwrite(&contentSize, sizeof(int32_t), 1, fp);
        size_t childSize = 0;
        fwrite(&childSize, sizeof(int32_t), 1, fp);

        // datas's
        fwrite(colors.data(), sizeof(uint8_t), contentSize, fp);
    }

    size_t getSize() {
        return sizeof(int32_t) * colors.size();
    }
};

struct VoxCube {
    int id = 0;

    // translate
    int tx = 0;
    int ty = 0;
    int tz = 0;

    SIZE size;
    std::map<KeyFrame, XYZI> xyzis;

    VoxCube() = default;

    void write(FILE* fp) {
        for (auto& xyzi : xyzis) {
            size.write(fp);
            xyzi.second.write(fp);
        }
    }
};

class Writer {
private:
    static const uint32_t GetID(const uint8_t& a, const uint8_t& b, const uint8_t& c, const uint8_t& d) {
        return (a) | (b << 8) | (c << 16) | (d << 24);
    }

private:
    Version MV_VERSION = 150;  // the old version of MV not open another file than if version is 150 (answer by @ephtracy)

    TagID ID_VOX = GetID('V', 'O', 'X', ' ');
    TagID ID_PACK = GetID('P', 'A', 'C', 'K');
    TagID ID_MAIN = GetID('M', 'A', 'I', 'N');
    TagID ID_SIZE = GetID('S', 'I', 'Z', 'E');
    TagID ID_XYZI = GetID('X', 'Y', 'Z', 'I');
    TagID ID_RGBA = GetID('R', 'G', 'B', 'A');
    TagID ID_NTRN = GetID('n', 'T', 'R', 'N');
    TagID ID_NGRP = GetID('n', 'G', 'R', 'P');
    TagID ID_NSHP = GetID('n', 'S', 'H', 'P');

    VoxelX m_MaxVoxelPerCubeX = 0;
    VoxelY m_MaxVoxelPerCubeY = 0;
    VoxelZ m_MaxVoxelPerCubeZ = 0;

    CubeID maxCubeId = 0;
    CubeX minCubeX = (CubeX)1e7;
    CubeY minCubeY = (CubeY)1e7;
    CubeZ minCubeZ = (CubeZ)1e7;

    FILE* m_file = nullptr;

    Volume maxVolume = Volume(1e7, -1e7);

    KeyFrame m_KeyFrame = 0;

    std::vector<ColorID> colors;

    std::vector<VoxCube> cubes;

    std::map<CubeX, std::map<CubeY, std::map<CubeZ, CubeID>>> cubesId;
    std::map<KeyFrame, std::map<VoxelX, std::map<VoxelY, std::map<VoxelZ, VoxelID>>>> voxelId;

    int32_t lastError = 0;

    bool m_TimeLoggingEnabled = false;  // for log elapsed time between key frames and total

    std::chrono::steady_clock::time_point m_StartTime;
    std::chrono::steady_clock::time_point m_LastKeyFrameTime;
    std::map<KeyFrame, double> m_FrameTimes;
    double m_TotalTime = 0.0;

    KeyFrameTimeLoggingFunctor m_KeyFrameTimeLoggingFunctor;

public:
    //////////////////////////////////////////////////////////////////
    // the limit of magicavoxel is 127 for one cube, is 127 voxels (indexs : 0 -> 126)
    // vMaxVoxelPerCubeX,Y,Z define the limit of one cube
    Writer(const VoxelX& vMaxVoxelPerCubeX = 126, const VoxelY& vMaxVoxelPerCubeY = 126, const VoxelZ& vMaxVoxelPerCubeZ = 126) {
        // the limit of magicavoxel is 127 because the first voxel is 1 not 0
        // so this is 0 to 126
        // index limit, size is 127
        m_MaxVoxelPerCubeX = ez::clamp<size_t>(vMaxVoxelPerCubeX, 0, 126);
        m_MaxVoxelPerCubeY = ez::clamp<size_t>(vMaxVoxelPerCubeY, 0, 126);
        m_MaxVoxelPerCubeZ = ez::clamp<size_t>(vMaxVoxelPerCubeZ, 0, 126);
    }

    ~Writer() = default;

    Writer& clear() {
        clearVoxels();
        clearColors();
        return *this;
    }

    Writer& clearVoxels() {
        cubes.clear();
        cubesId.clear();
        voxelId.clear();
        return *this;
    }

    Writer& clearColors() {
        colors.clear();
        return *this;
    }

    Writer& startTimeLogging() {
        m_TimeLoggingEnabled = true;
        m_StartTime = std::chrono::steady_clock::now();
        m_LastKeyFrameTime = m_StartTime;
        return *this;
    };

    Writer& stopTimeLogging() {
        if (m_TimeLoggingEnabled) {
            const auto now = std::chrono::steady_clock::now();
            m_FrameTimes[m_KeyFrame] = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_LastKeyFrameTime).count() * 1e-3;
            if (m_KeyFrameTimeLoggingFunctor) {
                m_KeyFrameTimeLoggingFunctor(m_KeyFrame, m_FrameTimes.at(m_KeyFrame));
            }
            m_TotalTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_StartTime).count() * 1e-3;
            m_TimeLoggingEnabled = false;
        }
        return *this;
    }

    Writer& setKeyFrameTimeLoggingFunctor(const KeyFrameTimeLoggingFunctor& vKeyFrameTimeLoggingFunctor) {
        m_KeyFrameTimeLoggingFunctor = vKeyFrameTimeLoggingFunctor;
        return *this;
    }

    Writer& setKeyFrame(uint32_t vKeyFrame) {
        if (m_KeyFrame != vKeyFrame) {
            if (m_TimeLoggingEnabled) {
                const auto now = std::chrono::steady_clock::now();
                const auto elapsed = now - m_LastKeyFrameTime;
                m_FrameTimes[m_KeyFrame] = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() * 1e-3;
                if (m_KeyFrameTimeLoggingFunctor) {
                    m_KeyFrameTimeLoggingFunctor(m_KeyFrame, m_FrameTimes.at(m_KeyFrame));
                }
                m_LastKeyFrameTime = now;
            }
            m_KeyFrame = vKeyFrame;
        }
        return *this;
    }

    Writer& addColor(const uint8_t& r, const uint8_t& g, const uint8_t& b, const uint8_t& a, const uint8_t& index) {
        while (colors.size() <= index)
            colors.push_back(0);
        colors[index] = GetID(r, g, b, a);
        return *this;
    }

    Writer& addVoxel(const size_t& vX, const size_t& vY, const size_t& vZ, const uint8_t& vColorIndex) {
        // cube pos
        size_t ox = (size_t)std::floor((double)vX / (double)m_MaxVoxelPerCubeX);
        size_t oy = (size_t)std::floor((double)vY / (double)m_MaxVoxelPerCubeY);
        size_t oz = (size_t)std::floor((double)vZ / (double)m_MaxVoxelPerCubeZ);

        minCubeX = ez::mini<size_t>(minCubeX, ox);
        minCubeY = ez::mini<size_t>(minCubeX, oy);
        minCubeZ = ez::mini<size_t>(minCubeX, oz);

        auto cube = m_GetCube(ox, oy, oz);

        m_MergeVoxelInCube(vX, vY, vZ, vColorIndex, cube);
        return *this;
    }

    Writer& save(const std::string& vFilePathName) {
        if (m_OpenFileForWriting(vFilePathName)) {
            int32_t zero = 0;

            fwrite(&ID_VOX, sizeof(int32_t), 1, m_file);
            fwrite(&MV_VERSION, sizeof(int32_t), 1, m_file);

            // MAIN CHUNCK
            fwrite(&ID_MAIN, sizeof(int32_t), 1, m_file);
            fwrite(&zero, sizeof(int32_t), 1, m_file);

            long numBytesMainChunkPos = m_GetFilePos();
            fwrite(&zero, sizeof(int32_t), 1, m_file);

            long headerSize = m_GetFilePos();

            int count = (int)cubes.size();

            int nodeIds = 0;
            nTRN rootTransform(1);
            rootTransform.nodeId = nodeIds;
            rootTransform.childNodeId = ++nodeIds;

            nGRP rootGroup(count);
            rootGroup.nodeId = nodeIds;  //
            rootGroup.nodeChildrenNodes = count;

            std::vector<nSHP> shapes;
            std::vector<nTRN> shapeTransforms;
            size_t cube_idx = 0U;
            int32_t model_id = 0U;
            for (auto& cube : cubes) {
                cube.write(m_file);

                // trans
                nTRN trans(1);             // not a trans anim so ony one frame
                trans.nodeId = ++nodeIds;  //
                rootGroup.childNodes[cube_idx] = nodeIds;
                trans.childNodeId = ++nodeIds;
                trans.layerId = 0;
                cube.tx = (int)std::floor((cube.tx - minCubeX + 0.5f) * m_MaxVoxelPerCubeX - maxVolume.lowerBound.x - maxVolume.Size().x * 0.5);
                cube.ty = (int)std::floor((cube.ty - minCubeY + 0.5f) * m_MaxVoxelPerCubeY - maxVolume.lowerBound.y - maxVolume.Size().y * 0.5);
                cube.tz = (int)std::floor((cube.tz - minCubeZ + 0.5f) * m_MaxVoxelPerCubeZ);
                trans.frames[0].Add("_t", ez::str::toStr(cube.tx) + " " + ez::str::toStr(cube.ty) + " " + ez::str::toStr(cube.tz));
                shapeTransforms.push_back(trans);

                // shape
                nSHP shape((int32_t)cube.xyzis.size());
                shape.nodeId = nodeIds;
                size_t model_array_id = 0U;
                for (const auto& xyzi : cube.xyzis) {
                    shape.models[model_array_id].modelId = model_id;
                    shape.models[model_array_id].modelAttribs.Add("_f", ez::str::toStr(xyzi.first));
                    ++model_array_id;
                    ++model_id;
                }
                shapes.push_back(shape);

                ++cube_idx;
            }

            rootTransform.write(m_file);
            rootGroup.write(m_file);

            // trn & shp
            for (int i = 0; i < count; i++) {
                shapeTransforms[i].write(m_file);
                shapes[i].write(m_file);
            }

            // no layr in my cases

            // layr
            /*for (int i = 0; i < 8; i++)
            {
                LAYR layr;
                layr.nodeId = i;
                layr.nodeAttribs.Add("_name", ez::str::toStr(i));
                layr.write(m_file);
            }*/

            // RGBA Palette
            if (colors.size() > 0) {
                RGBA palette;
                for (int32_t i = 0; i < 255; i++) {
                    if (i < (int32_t)colors.size()) {
                        palette.colors[i] = colors[i];
                    } else {
                        palette.colors[i] = 0;
                    }
                }

                palette.write(m_file);
            }

            const long mainChildChunkSize = m_GetFilePos() - headerSize;
            m_SetFilePos(numBytesMainChunkPos);
            uint32_t size = (uint32_t)mainChildChunkSize;
            fwrite(&size, sizeof(uint32_t), 1, m_file);

            m_CloseFile();
        }
        return *this;
    }

    size_t getVoxelsCount(const KeyFrame& vKeyFrame) const {
        size_t voxel_count = 0U;
        for (const auto& cube : cubes) {
            if (cube.xyzis.find(vKeyFrame) != cube.xyzis.end()) {
                voxel_count += cube.xyzis.at(vKeyFrame).numVoxels;
            }
        }
        return voxel_count;
    }

    size_t getVoxelsCount() const {
        size_t voxel_count = 0U;
        for (const auto& cube : cubes) {
            for (auto& key_xyzi : cube.xyzis) {
                voxel_count += key_xyzi.second.numVoxels;
            }
        }
        return voxel_count;
    }

    Writer& printStats() {
        std::cout << "---- Stats ------------------------------" << std::endl;
        std::cout << "Volume : " << maxVolume.Size().x << " x " << maxVolume.Size().y << " x " << maxVolume.Size().z << std::endl;
        std::cout << "count cubes : " << cubes.size() << std::endl;
        std::map<KeyFrame, size_t> frame_counts;
        for (const auto& cube : cubes) {
            for (auto& key_xyzi : cube.xyzis) {
                frame_counts[key_xyzi.first] += key_xyzi.second.numVoxels;
            }
        }
        size_t voxels_total = 0U;
        if (frame_counts.size() > 1U) {
            std::cout << "count key frames : " << frame_counts.size() << std::endl;
            std::cout << "-----------------------------------------" << std::endl;
            for (const auto& frame_count : frame_counts) {
                std::cout << " o--\\-> key frame : " << frame_count.first << std::endl;
                std::cout << "     \\-> voxels count : " << frame_count.second << std::endl;
                if (m_FrameTimes.find(frame_count.first) != m_FrameTimes.end()) {
                    std::cout << "      \\-> elapsed time : " << m_FrameTimes.at(frame_count.first) << " secs" << std::endl;
                }
                voxels_total += frame_count.second;
            }
            std::cout << "-----------------------------------------" << std::endl;
        } else if (!frame_counts.empty()) {
            voxels_total = frame_counts.begin()->second;
        }
        std::cout << "voxels total : " << voxels_total << std::endl;
        std::cout << "total elapsed time : " << m_TotalTime << " secs" << std::endl;
        std::cout << "-----------------------------------------" << std::endl;
        return *this;
    }

private:
    bool m_OpenFileForWriting(const std::string& vFilePathName) {
#if _MSC_VER
        lastError = fopen_s(&m_file, vFilePathName.c_str(), "wb");
#else
        m_file = fopen(vFilePathName.c_str(), "wb");
        lastError = m_file ? 0 : errno;
#endif
        if (lastError != 0) return false;
        return true;
    }

    void m_CloseFile() {
        fclose(m_file);
    }

    long m_GetFilePos() const {
        return ftell(m_file);
    }

    void m_SetFilePos(const long& vPos) {
        //  SEEK_SET	Beginning of file
        //  SEEK_CUR	Current position of the file pointer
        //	SEEK_END	End of file
        fseek(m_file, vPos, SEEK_SET);
    }

    const size_t m_GetCubeId(const VoxelX& vX, const VoxelY& vY, const VoxelZ& vZ) {
        if (cubesId.find(vX) != cubesId.end()) {
            if (cubesId[vX].find(vY) != cubesId[vX].end()) {
                if (cubesId[vX][vY].find(vZ) != cubesId[vX][vY].end()) {
                    return cubesId[vX][vY][vZ];
                }
            }
        }

        cubesId[vX][vY][vZ] = maxCubeId++;

        return cubesId[vX][vY][vZ];
    }

    // Wrap a position inside a particular cube dimension
    inline uint8_t Wrap(size_t v, size_t lim) {
        v = v % lim;
        if (v < 0) {
            v += lim;
        }
        return (uint8_t)v;
    }

    void m_MergeVoxelInCube(const VoxelX& vX, const VoxelY& vY, const VoxelZ& vZ, const uint8_t& vColorIndex, VoxCube* vCube) {
        maxVolume.Combine(ez::dvec3((double)vX, (double)vY, (double)vZ));

        bool exist = false;
        if (voxelId.find(m_KeyFrame) != voxelId.end()) {
            auto& vidk = voxelId.at(m_KeyFrame);
            if (vidk.find(vX) != vidk.end()) {
                auto& vidkx = vidk.at(vX);
                if (vidkx.find(vY) != vidkx.end()) {
                    auto& vidkxy = vidkx.at(vY);
                    if (vidkxy.find(vZ) != vidkxy.end()) {
                        exist = true;
                    }
                }
            }
        }

        if (!exist) {
            auto& xyzi = vCube->xyzis[m_KeyFrame];
            xyzi.voxels.push_back(Wrap(vX, m_MaxVoxelPerCubeX));  // x
            xyzi.voxels.push_back(Wrap(vY, m_MaxVoxelPerCubeY));  // y
            xyzi.voxels.push_back(Wrap(vZ, m_MaxVoxelPerCubeZ));  // z

            // correspond a la loc de la couleur du voxel en question
            voxelId[m_KeyFrame][vX][vY][vZ] = (int)xyzi.voxels.size();

            xyzi.voxels.push_back(vColorIndex);  // color index
        }
    }

    VoxCube* m_GetCube(const VoxelX& vX, const VoxelY& vY, const VoxelZ& vZ) {
        const auto& id = m_GetCubeId(vX, vY, vZ);

        if (id == cubes.size()) {
            VoxCube c;

            c.id = (int32_t)id;

            c.tx = (int32_t)vX;
            c.ty = (int32_t)vY;
            c.tz = (int32_t)vZ;

            c.size.sizex = (int32_t)m_MaxVoxelPerCubeX;
            c.size.sizey = (int32_t)m_MaxVoxelPerCubeY;
            c.size.sizez = (int32_t)m_MaxVoxelPerCubeZ;

            cubes.push_back(c);
        }

        if (id < cubes.size()) {
            return &cubes[id];
        }

        return nullptr;
    }
};

}  // namespace vox
}  // namespace file
}  // namespace ez
