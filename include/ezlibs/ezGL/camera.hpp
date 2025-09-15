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

// ezOrbitCamera is part of the ezLibs project : https://github.com/aiekick/ezLibs.git

#include <array>
#include <cmath>
#include "../ezVec3.hpp"
#include "../ezMat4.hpp"

namespace ez {
namespace gl {

template <typename T>
class Camera3D {
    static_assert(std::is_arithmetic<T>::value, "Camera3D requires arithmetic T");
    vec3<T> m_position{T(0), T(0), T(1)};
    vec3<T> m_target{T(0), T(0), T(0)};
    vec3<T> m_up{T(0), T(1), T(0)};

public:  // methods
    Camera3D() {}

    // --- Setters/Getters ---
    void setPosition(const vec3<T>& vPosition) { m_position = vPosition; }
    void setTarget(const vec3<T>& vTarget) { m_target = vTarget; }
    void setUp(const vec3<T>& vUp) { m_up = vUp; }

    const vec3<T>& getPosition() const { return m_position; }
    const vec3<T>& getTarget() const { return m_target; }
    const vec3<T>& getUp() const { return m_up; }

    // --- Core matrices ---
    // View matrix (right-handed)
    Mat4<T> computeViewMatrix() const { return Mat4<T>::LookAt(m_position, m_target, m_up); }

    // Perspective (OpenGL clip space: z in [-1,1])
    static Mat4<T> makeGLPerspective(T vFovYRadians, T vAspect, T vNear, T vFar) { return Mat4<T>::PerspectiveGL(vFovYRadians, vAspect, vNear, vFar); }

    // Perspective (Vulkan clip space: z in [0,1] + Y flip)
    static Mat4<T> makeVKPerspective(T vFovYRadians, T vAspect, T vNear, T vFar) { return Mat4<T>::PerspectiveVK(vFovYRadians, vAspect, vNear, vFar); }

    // Orthographic (OpenGL)
    static Mat4<T> makeGLOrtho(T vLeft, T vRight, T vBottom, T vTop, T vNear, T vFar) { return Mat4<T>::OrthoGL(vLeft, vRight, vBottom, vTop, vNear, vFar); }

    // Orthographic (Vulkan)
    static Mat4<T> makeVKOrtho(T vLeft, T vRight, T vBottom, T vTop, T vNear, T vFar) { return Mat4<T>::OrthoVK(vLeft, vRight, vBottom, vTop, vNear, vFar); }

    // Frustum (OpenGL)
    static Mat4<T> makeGLFrustum(T vLeft, T vRight, T vBottom, T vTop, T vNear, T vFar) { return Mat4<T>::FrustumGL(vLeft, vRight, vBottom, vTop, vNear, vFar); }

    // Frustum (Vulkan)
    static Mat4<T> makeVKFrustum(T vLeft, T vRight, T vBottom, T vTop, T vNear, T vFar) { return Mat4<T>::FrustumVK(vLeft, vRight, vBottom, vTop, vNear, vFar); }
};

template <typename T>
class Camera3DOrbit : public Camera3D<T> {
    vec3<T> m_orbitTarget{T(0), T(0), T(0)};
    T m_distance{T(3)};
    T m_yawRadians{T(0)};    // azimuth around world up (y)
    T m_pitchRadians{T(0)};  // elevation
    T m_minDistance{T(0.01)};

public:
    void setOrbitTarget(const vec3<T>& vTarget) { m_orbitTarget = vTarget; }
    void setDistance(T vDistance) { m_distance = (vDistance < m_minDistance) ? m_minDistance : vDistance; }
    void setAngles(T vYawRadians, T vPitchRadians) {
        m_yawRadians = vYawRadians;
        m_pitchRadians = vPitchRadians;
    }
    void setMinDistance(T vMinDistance) {
        m_minDistance = (vMinDistance <= T(0)) ? T(0.000001) : vMinDistance;
        if (m_distance < m_minDistance)
            m_distance = m_minDistance;
    }

    // Update base camera position and target from orbit parameters
    void update() {
        const T cosYaw = static_cast<T>(std::cos(static_cast<double>(m_yawRadians)));
        const T sinYaw = static_cast<T>(std::sin(static_cast<double>(m_yawRadians)));
        const T cosPitch = static_cast<T>(std::cos(static_cast<double>(m_pitchRadians)));
        const T sinPitch = static_cast<T>(std::sin(static_cast<double>(m_pitchRadians)));

        // World axes: X(right), Y(up), Z(forward)
        const vec3<T> worldRight{T(1), T(0), T(0)};
        const vec3<T> worldUp{T(0), T(1), T(0)};
        const vec3<T> worldFwd{T(0), T(0), T(1)};

        // Spherical direction from yaw/pitch (right-handed)
        // Forward dir in world space:
        vec3<T> forwardDir = {
            cosPitch * sinYaw,  // x
            sinPitch,           // y
            cosPitch * cosYaw   // z
        };

        // Eye = target - forward * distance
        vec3<T> eye = {m_orbitTarget[0] - forwardDir[0] * m_distance, m_orbitTarget[1] - forwardDir[1] * m_distance, m_orbitTarget[2] - forwardDir[2] * m_distance};

        this->setPosition(eye);
        this->setTarget(m_orbitTarget);
        this->setUp(worldUp);
    }
};

template <typename T>
class Camera3DTurntable : public Camera3D<T> {
    vec3<T> m_pivot{T(0), T(0), T(0)};
    vec3<T> m_axis{T(0), T(1), T(0)};  // normalized axis of rotation
    T m_angleRadians{T(0)};
    T m_distance{T(3)};

public:
    void setPivot(const vec3<T>& vPivot) { m_pivot = vPivot; }
    void setAxis(const vec3<T>& vAxisNorm) { m_axis = vAxisNorm; }
    void setAngle(T vAngleRadians) { m_angleRadians = vAngleRadians; }
    void setDistance(T vDistance) { m_distance = vDistance; }

    // Simple axis-angle rotation around pivot
    void update() {
        // Build a minimal rotation around m_axis by m_angleRadians to place the eye on a circle
        // Start from a reference point along Z (pivot + distance * Z), then rotate.
        const vec3<T> refDir{T(0), T(0), T(1)};
        const vec3<T> start = {m_pivot[0] + refDir[0] * m_distance, m_pivot[1] + refDir[1] * m_distance, m_pivot[2] + refDir[2] * m_distance};

        // Rodrigues' rotation formula
        const T c = static_cast<T>(std::cos(static_cast<double>(m_angleRadians)));
        const T s = static_cast<T>(std::sin(static_cast<double>(m_angleRadians)));
        const T oneMinusC = T(1) - c;

        const T ax = m_axis[0], ay = m_axis[1], az = m_axis[2];

        // Rotate (start - pivot)
        const vec3<T> v = {start[0] - m_pivot[0], start[1] - m_pivot[1], start[2] - m_pivot[2]};
        vec3<T> vRot = {
            v[0] * (c + ax * ax * oneMinusC) + v[1] * (ax * ay * oneMinusC - az * s) + v[2] * (ax * az * oneMinusC + ay * s),
            v[0] * (ay * ax * oneMinusC + az * s) + v[1] * (c + ay * ay * oneMinusC) + v[2] * (ay * az * oneMinusC - ax * s),
            v[0] * (az * ax * oneMinusC - ay * s) + v[1] * (az * ay * oneMinusC + ax * s) + v[2] * (c + az * az * oneMinusC)};

        const vec3<T> eye = {m_pivot[0] + vRot[0], m_pivot[1] + vRot[1], m_pivot[2] + vRot[2]};

        this->setPosition(eye);
        this->setTarget(m_pivot);
        this->setUp({T(0), T(1), T(0)});
    }
};

template <typename T>
class Camera3DFreeFlight : public Camera3D<T> {
    vec3<T> m_forward{T(0), T(0), T(1)};  // normalized
    vec3<T> m_right{T(1), T(0), T(0)};    // normalized
    vec3<T> m_up{T(0), T(1), T(0)};       // normalized

public:
    // Init from yaw/pitch and position
    void setFromYawPitch(const vec3<T>& vPosition, T vYawRadians, T vPitchRadians) {
        const T cosYaw = static_cast<T>(std::cos(static_cast<double>(vYawRadians)));
        const T sinYaw = static_cast<T>(std::sin(static_cast<double>(vYawRadians)));
        const T cosPitch = static_cast<T>(std::cos(static_cast<double>(vPitchRadians)));
        const T sinPitch = static_cast<T>(std::sin(static_cast<double>(vPitchRadians)));

        m_forward = {cosPitch * sinYaw, sinPitch, cosPitch * cosYaw};
        m_right = {cosYaw, T(0), -sinYaw};
        m_up = {-sinPitch * sinYaw, cosPitch, -sinPitch * cosYaw};

        this->setPosition(vPosition);
        this->setTarget({vPosition[0] + m_forward[0], vPosition[1] + m_forward[1], vPosition[2] + m_forward[2]});
        this->setUp(m_up);
    }

    // Move along local axes
    void moveLocal(T vDeltaForward, T vDeltaRight, T vDeltaUp) {
        const vec3<T> pos = this->getPosition();
        vec3<T> newPos = {
            pos[0] + m_forward[0] * vDeltaForward + m_right[0] * vDeltaRight + m_up[0] * vDeltaUp,
            pos[1] + m_forward[1] * vDeltaForward + m_right[1] * vDeltaRight + m_up[1] * vDeltaUp,
            pos[2] + m_forward[2] * vDeltaForward + m_right[2] * vDeltaRight + m_up[2] * vDeltaUp};
        this->setPosition(newPos);
        this->setTarget({newPos[0] + m_forward[0], newPos[1] + m_forward[1], newPos[2] + m_forward[2]});
        this->setUp(m_up);
    }
};

}  // namespace gl
} // namespace ez
