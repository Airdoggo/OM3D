#version 450

#include "utils.glsl"

layout(location = 0) in vec3 in_pos;

layout(binding = 0) uniform Data {
    FrameData frame;
};

uniform mat4 model;

void main() {
    gl_Position = frame.camera.view_proj * (model * vec4(in_pos, 1.0));
}

