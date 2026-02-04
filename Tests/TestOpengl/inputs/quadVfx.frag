#version 330
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 fragColor;
layout(location = 0) in vec2 vUV;

// not used but for tests uniforms
uniform int iFrame;
uniform vec4 iMouse;
uniform vec3 iResolution;

// test backbuffer
uniform sampler2D backBuffer;

void main() {
    fragColor = texture(backBuffer, vUV);
    if (iFrame == 0) {
        fragColor = vec4(1,1,0,1);
    } else {
        fragColor = 1.0 - fragColor; // inversion frame to frame
    }
    fragColor.a = 1.0;
}
